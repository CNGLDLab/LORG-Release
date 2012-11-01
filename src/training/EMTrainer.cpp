#include <iostream>
#include "EMTrainer.h"

#include <algorithm>
#include <numeric>


#include <boost/thread.hpp>

#include <tbb/tick_count.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>

using namespace tbb;

#define bumap boost::unordered_map

namespace {

void calculate_occurrences(const TrainingNode* n,
                           bumap<BRuleTraining*,std::vector<brule_occurrence> >& bmap,
                           bumap<URuleTraining*,std::vector<urule_occurrence> >& umap,
                           bumap<LexicalRuleTraining*,std::vector<lrule_occurrence> >& lmap)
{
    n->add_occurrences(bmap,umap,lmap, n);
}

void calculate_occurrences(const std::vector<BinaryTrainingTree>& trees,
                           bumap<BRuleTraining*,std::vector<brule_occurrence> >& bmap,
                           bumap<URuleTraining*,std::vector<urule_occurrence> >& umap,
                           bumap<LexicalRuleTraining*,std::vector<lrule_occurrence> >& lmap)
{
    for(std::vector<BinaryTrainingTree>::const_iterator i(trees.begin()); i != trees.end(); ++i)
        calculate_occurrences(i->get_root(), bmap,umap, lmap);
}
}


namespace multithread {

//thread calculating inside/outside probabilities of a portion of trees stored in a vector
// also computing the log likelihood of this portion
class inout_thread
{
    /*std::vector<BinaryTrainingTree>& trees;
          unsigned trees_begin;
          unsigned trees_end;*/
    unsigned ignored;

public:
    double logprob;

    inout_thread(): ignored(0), logprob(0.0) {}
    //inout_thread(std::vector<BinaryTrainingTree>& ts, unsigned tsb, unsigned tse, double& lp)
    //: trees(ts), trees_begin(tsb), trees_end(tse), logprob(lp), ignored(0) {};

    inout_thread( inout_thread& x, split ) : ignored(0), logprob(0.0) {}

    void operator()(const blocked_range<std::vector<BinaryTrainingTree>::iterator>& r)
    {
        for(std::vector<BinaryTrainingTree>::iterator i = r.begin(); i != r.end(); ++i) {
            //for(unsigned i = trees_begin; i != trees_end; ++i) {

            // calculate the inside probability of all nodes in the tree
            BinaryTrainingTree t = *i;
            /*trees[i]*/
            t.compute_inside_probability();

            // update the likelihood with the inside probability of the tree
            // (this will be the inside probability of the root node which has no annotations)

            double log_tree_prob = /*trees[i]*/t.get_log_probability();

            if(std::isnan(log_tree_prob) || log_tree_prob == - std::numeric_limits<double>::infinity())
                ++ignored;
            else {
                logprob +=  log_tree_prob;
                /*trees[i]*/t.compute_outside_probability();
            }
        }
    }

    void join( const inout_thread& y ) {logprob += y.logprob;}
};

// updating rule count/prob in the multithreaded implementation
// for a portion of rules whose occurrences in the treebank have been calculated
template <typename T, typename U>
struct update_thread_tbb
{
    typedef T rule_type;
    typedef U occurrence_type;
    typedef std::vector< std::pair<rule_type*, std::vector<occurrence_type> > > vector_type;

    update_thread_tbb() {}

    void operator()(const blocked_range<typename vector_type::iterator>& r) const
    {
        for(typename vector_type::iterator i = r.begin(); i < r.end(); ++i) {
            //for(unsigned i = begin; i < end; ++i) {

            std::pair<rule_type*, std::vector<occurrence_type> > p = *i;
            rule_type * rule = p.first; //v[i].first;
            const std::vector<occurrence_type>& occurrences = p.second; //v[i].second;

            for(typename std::vector<occurrence_type>::const_iterator it(occurrences.begin()); it != occurrences.end(); ++ it) {

                double logprob = it->root->get_annotations().get_inside(0);

                if(std::isnan(logprob) || logprob == - std::numeric_limits<double>::infinity())
                    break;
                update_count_in_occurrences(rule, *it);
            }
        }
    }

    void update_count_in_occurrences(BRuleTraining* r, const brule_occurrence& bo) const
    {
        r->update_rule_frequencies(bo.left->get_annotations().inside_probabilities,
                                   bo.right->get_annotations().inside_probabilities,
                                   bo.up->get_annotations().outside_probabilities,
                                   bo.root->get_annotations().inside_probabilities);
    }

    void update_count_in_occurrences(URuleTraining* r, const urule_occurrence& o) const
    {
        r->update_rule_frequencies(o.left->get_annotations().inside_probabilities,
                                   o.up->get_annotations().outside_probabilities,
                                   o.root->get_annotations().inside_probabilities);
    }
};

struct update_counts_unary
{
    std::vector< std::pair<URuleTraining*,std::vector<urule_occurrence> > >&vu;
    update_counts_unary(std::vector< std::pair<URuleTraining*,
                        std::vector<urule_occurrence> > >& _vu)
        :vu(_vu){}

    void operator()()const
    {
        typedef std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > > VectorPairsURules;
        multithread::update_thread_tbb<URuleTraining, urule_occurrence> uut;
        parallel_for(blocked_range<VectorPairsURules::iterator>(vu.begin(), vu.end(), vu.size()/2), uut);
    }
};

struct update_counts_binary
{
    std::vector< std::pair<BRuleTraining*,std::vector<brule_occurrence> > >&vb;
    update_counts_binary(std::vector< std::pair<BRuleTraining*,
                         std::vector<brule_occurrence> > >& _vb)
        :vb(_vb){}

    void operator()()const
    {
        typedef std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > > VectorPairsBRules;
        std::random_shuffle(vb.begin(),vb.end());
        multithread::update_thread_tbb<BRuleTraining, brule_occurrence> but;
        parallel_for(blocked_range<VectorPairsBRules::iterator>(vb.begin(), vb.end(), vb.size()/2), but);
    }
};
}


void EMTrainer::do_em(std::vector<BinaryTrainingTree>& trees,
                      TrainingGrammar& em_grammar, unsigned num_iterations,
                      double smooth_grammar, double smooth_lexicon,
                      bool remove_unlikes, TrainingGrammar::SmoothType type)
{

    //  std::random_shuffle(trees.begin(), trees.end());
    unsigned i = 1;

    std::clog.precision(30);
    std::cout.precision(30);

    // structures to store occurrences of rules in the treebank
    // used by the multithreaded implementation
    bumap<BRuleTraining*,std::vector<brule_occurrence> > bmap;
    bumap<URuleTraining*,std::vector<urule_occurrence> > umap;
    bumap<LexicalRuleTraining*,std::vector<lrule_occurrence> > lmap;

    std::clog << "before calculate_occurrences" << std::endl;
    calculate_occurrences(trees, bmap, umap, lmap);
    std::clog << "after calculate_occurrences" << std::endl;
    //convert maps to vectors
    std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > > vb(bmap.begin(),bmap.end());
    std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > > vu(umap.begin(),umap.end());
    std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > > vl(lmap.begin(),lmap.end());

    while (i <= num_iterations) {
        //tick_count iter_start = tick_count::now();
        reset_counts(em_grammar);


        //   std::cout << "tree likelihoods for iteration " <<(i-1) << std::endl;
        //   std::cout << "total likelihood is " << calculate_likelihood(trees) << std::endl;

        if(verbose) std::clog << "starting EM iteration: " << i << std::endl;
        if(verbose) std::clog << "about to perform expectation " << std::endl;

        //EXPECTATION STEP
        expectation(trees, em_grammar, i == num_iterations, vb, vu, vl);


        ///// Why i-1 ???
        ///because the log likelihood will be the log likelihood of the previous iteration.
        // in iteration i-1 the E step and M step are performed for iteration i-1, to get the likelihood of the
        // corpus based on the probabilities calculated in the i-1 step you need to get the likelihoods of the trees based
        // on the new i-1 probabilities.  this is done in the expectation part of the ith iteration.
        if(verbose) {
            std::clog << "training set likelihood for iteration " << (i-1) << " is "
                      << log_likelihood << std::endl;
        }

        if(verbose) std::clog << "about to perform maximisation " << std::endl;

        //MAXIMISATION STEP
        maximisation(em_grammar);

        if(remove_unlikes)
            em_grammar.remove_unlikely_annotations_all_rules(threshold);


        if(smooth_grammar > 0.0 || smooth_lexicon > 0.0) {

            smooth_grammar_rules(em_grammar, smooth_grammar,smooth_lexicon, type);

            if(remove_unlikes)
                em_grammar.remove_unlikely_annotations_all_rules(threshold);
        }
        //std::cout <<"### iter:\t" << i << "\t" << (tick_count::now() - iter_start).seconds() << std::endl;
        ++i;
    }
}

void EMTrainer::smooth_grammar_rules( TrainingGrammar& em_grammar, double smooth_grammar, double smooth_lexicon,
                                      TrainingGrammar::SmoothType type)
{
    if(verbose)
        std::clog << "smoothing rules" << std::endl;
    em_grammar.uncompact_all_rules();
    em_grammar.smooth_all_rules(smooth_grammar, smooth_lexicon, type);
    em_grammar.compact_all_rules();
}

void EMTrainer::expectation(std::vector<BinaryTrainingTree>& trees, TrainingGrammar& em_grammar,
                            bool last_iteration,
                            std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > >& vb,
                            std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > >& vu,
                            std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& vl
                            )
{

    log_likelihood = 0;

    std::cout << "Before in/out" << std::endl;
    task_scheduler_init init(threads);
    // TODO make  a separate function
    {
        multithread::inout_thread io;
        parallel_reduce(blocked_range<std::vector<BinaryTrainingTree>::iterator>(trees.begin(), trees.end(), trees.size()/(threads*3)), io);
        log_likelihood += io.logprob;
    }


    std::cout << "Before update counts" << std::endl;

    {
        typedef std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > > VectorPairsURules;
        multithread::update_thread_tbb<URuleTraining, urule_occurrence> uut;
        parallel_for(blocked_range<VectorPairsURules::iterator>(vu.begin(), vu.end()), uut);

        typedef std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > > VectorPairsBRules;
        std::random_shuffle(vb.begin(),vb.end());
        multithread::update_thread_tbb<BRuleTraining, brule_occurrence> but;
        parallel_for(blocked_range<VectorPairsBRules::iterator>(vb.begin(), vb.end()), but);

        //Parallel_invoke
        //parallel_invoke(multithread::update_counts_binary(vb), multithread::update_counts_unary(vu));
    }

    std::cout << "Before lexical" << std::endl;

    em_grammar.update_lexical_counts(trees, last_iteration, vl, threads);

    if (verbose) {
        std::clog << "tree: " << trees.size() << std::endl;
        //    std::clog << "ignored: " << tree_ignored << " tree(s)" << std::endl;
    }
}


//grammar (as opposed to lexicon).
void EMTrainer::calculate_grammar_node_counts(const TrainingGrammar& grammar)
{

    for (std::vector<BRuleTraining>::const_iterator iter(grammar.get_binary_rules().begin());
         iter != grammar.get_binary_rules().end(); ++iter) {

        const std::vector< std::vector< std::vector<double> > >& rule_counts = iter->get_counts();

        std::vector<double>& current_label_counts = annotated_node_counts[iter->get_lhs()];
        current_label_counts.resize(rule_counts.size(),0.0);

        for(unsigned i = 0; i < rule_counts.size(); ++i) {
            double& current_count = current_label_counts[i];

            for(unsigned j = 0; j < rule_counts[i].size(); ++j)

                current_count += std::accumulate(rule_counts[i][j].begin(), rule_counts[i][j].end(), 0.0);
        }
    }

    for (std::vector<URuleTraining>::const_iterator iter(grammar.get_unary_rules().begin());
         iter != grammar.get_unary_rules().end(); ++iter) {

        const std::vector< std::vector<double> >& rule_counts = iter->get_counts();
        std::vector<double>& current_label_counts = annotated_node_counts[iter->get_lhs()];
        current_label_counts.resize(rule_counts.size(),0.0);
        for(unsigned i = 0; i < rule_counts.size(); ++i) {

            current_label_counts[i] += std::accumulate(rule_counts[i].begin(), rule_counts[i].end(),0.0);
        }
    }

}

struct update_probability_helper
{
    const AnnotatedNodeCountMap& anc_map;
    update_probability_helper(const AnnotatedNodeCountMap& a) : anc_map(a) {}

    template<class T>
    void operator()(T& rule) const {rule.update_probability(anc_map);}
};

void EMTrainer::maximisation(TrainingGrammar& em_grammar)
{
    //calculate_node_counts(em_grammar);
    calculate_grammar_node_counts(em_grammar);

    std::vector<BRuleTraining>& binary_rules = em_grammar.get_binary_rules();
    std::vector<URuleTraining>& unary_rules = em_grammar.get_unary_rules();

    update_probability_helper update(annotated_node_counts);

    std::for_each(binary_rules.begin(),binary_rules.end(), update);
    std::for_each(unary_rules.begin(),unary_rules.end(), update);

    // std::cout << "about to max lex rules.." << std::endl;
    em_grammar.maximise_lexical_rules();
}


void EMTrainer::split(std::vector<BinaryTrainingTree>& trees,
                      TrainingGrammar& em_grammar, unsigned randomness){

    em_grammar.uncompact_all_rules();

    em_grammar.update_split_annotations(split_number);

    resize_annotations_in_treebank(trees,em_grammar);

    em_grammar.split_all_rules(split_number, randomness);

    em_grammar.compact_all_rules();

}



void EMTrainer::calculate_proportions(const AnnotatedLabelsInfo& annotation_info,
                                      AnnotatedNodeCountMap& proportions)
{
    static int top_idx = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);

    unsigned number_of_labels = annotation_info.get_number_of_unannotated_labels();
    //clear proportions
    proportions.resize(number_of_labels);

    //for each non-terminal label in the grammar
    for (unsigned label = 0; label < number_of_labels; ++label){
        //skip top as it's not annotated
        if ((int) label == top_idx) continue;

        //number of latent annotation sets for this label
        unsigned number_of_latent_annotations = annotation_info.get_number_of_annotations(label);
        //unsigned number_of_split_sets = number_of_latent_annotations / split_number;
        // std::cout << "number of latent annotations for this label " << number_of_latent_annotations
        //         << " split number " << split_number << std::endl;
        assert( number_of_latent_annotations % split_number == 0);


        proportions[label].resize(number_of_latent_annotations,0.0);
        //for each set of latent annotations
        for (unsigned start = 0; start < number_of_latent_annotations; start += split_number){


            //merge_denominators[label, latent_annotation] = sum of counts for the merge set containing label, latent_annotation
            //eg.. split size = 2:
            // merge_denominators[A, 0] = merge_denominators[A, 1] = count(A,0) + count(A,1)
            // merge_denominators[A, 2] = merge_denominators[A, 3] = count(A,2) + count(A,3)
            double denominator= 0.0;
            for(unsigned latent_annotation=start; latent_annotation < (start+split_number); ++latent_annotation ){
                denominator+=annotated_node_counts[label][latent_annotation];
            }
            for(unsigned latent_annotation=start; latent_annotation < (start+split_number); ++latent_annotation){
                proportions[label][latent_annotation] = annotated_node_counts[label].at(latent_annotation) / denominator;
            }
        }
    }
}


void EMTrainer::calculate_delta_scores(const std::vector<BinaryTrainingTree> & trees, DeltaMap& delta_scores,
                                       const AnnotatedNodeCountMap& proportions)
{
    for(std::vector<BinaryTrainingTree>::const_iterator tree_iter(trees.begin()); tree_iter != trees.end(); ++tree_iter) {
        tree_iter->update_delta_scores(delta_scores,split_number,proportions,
                                       tree_iter->get_root()->get_annotations().inside_probabilities);
    }
}

///what's this?  comments would be nice!!

// it's a definition of an order relation over pairs
// (a,b) < (c,d) <=> b < d
// it's used to sort split symbols according to their delta

//struct order_2nd_dimension
//{
// template <typename T1, typename T2>
// bool operator()(const std::pair<T1,T2>& p1, const std::pair<T1,T2>& p2) const {return p1.second < p2.second;}
//};


//data structures:
//DeltaMap: key is a pair of ints where the first int in pair is nt label id, second int in pair is start index of split in annotations
//                      The value is the delta score
//
//DeltaMultMap: is used to sort the DeltaMap according to delta score - the score is the key and the map value is the pair of ints
//
//sets_to_merge: map contains the same information (minus the delta score), reorganised to facilate access when it is used in call to em_grammar.
// sets_to_merge[LHS label] -> vector of annotation sets (each element of the vector stores the first annotation index of the annotation set
//
//sets_to_merge_lookup: like sets_to_merge except its a map of key->set instead of a map of  key->vector
//TODO is there a smarter way to do this? - don't really need the sets_to_merge...

void EMTrainer::merge(std::vector<BinaryTrainingTree>& trees,
                      TrainingGrammar& em_grammar, int merge_percentage,
                      bool final_lex_em)
{
    if(verbose) std::cout << "about to perform expectation for merge" << std::endl;

    //EXPECTATION STEP
    reset_counts(em_grammar);

    bumap<BRuleTraining*,std::vector<brule_occurrence> > bmap;
    bumap<URuleTraining*,std::vector<urule_occurrence> > umap;
    bumap<LexicalRuleTraining*,std::vector<lrule_occurrence> > lmap;

    if(verbose) std::cout << "before calculate_occurrences" << std::endl;
    calculate_occurrences(trees, bmap, umap,lmap);
    if(verbose) std::cout << "after calculate_occurrences" << std::endl;


    std::vector< std::pair<BRuleTraining*, std::vector<brule_occurrence> > > vb(bmap.begin(),bmap.end());
    std::vector< std::pair<URuleTraining*, std::vector<urule_occurrence> > > vu(umap.begin(),umap.end());
    std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > > vl(lmap.begin(),lmap.end());
    expectation(trees,em_grammar,false, vb, vu, vl);


    if(verbose) {
        std::cout << "Before merging the training set likelihood " << log_likelihood << std::endl;
        std::cout << "Calculating delta scores...\n";
    }

    const AnnotatedLabelsInfo& a_infos = em_grammar.get_annotations_info();
    //print_split_sets(a_infos);

    AnnotatedNodeCountMap proportions;
    //first int in pair is nt label id, second int in pair is start index of split in annotations
    // typedef boost::unordered_map< std::pair<int, int >, double> DeltaMap;
    //  typedef std::multimap<double,std::pair<int,int> > DeltaMultiMap;
    DeltaMap delta_scores;

    //calculate node counts of the final (post-split) em iteration
    //calculate_node_counts(em_grammar);
    calculate_grammar_node_counts(em_grammar);
    //calculate node counts for the lexicon and add to general annotated node count data structure
    em_grammar.add_lexicon_annotated_node_counts(annotated_node_counts);
    //print_node_counts();

    //calculate proportions for delta score calculation
    calculate_proportions(a_infos,proportions);
    //print_proportions(proportions);

    //calculate delta scores
    calculate_delta_scores(trees,delta_scores, proportions);
    //print_delta_scores(delta_scores);

    // //sort splits according to delta score
    DeltaMultiMap delta_sortedByValue;
    int merge_count=0;

    //hack jlr
    //bool exclude_zeros = true;
    bool exclude_zeros = false;

    for(DeltaMap::const_iterator itr(delta_scores.begin()); itr != delta_scores.end(); ++itr){
        //   //don't bother putting the root node in the delta_sorted table
        if (! SymbolTable::instance_nt().is_root_label(itr->first.first)){
            //unless we're excluding zero delta scores and this delta score is zero
            if (!((exclude_zeros) && (itr->second == 0.0))){
                std::pair<double,std::pair<int,int> > element(itr->second,itr->first);
                ++merge_count;
                delta_sortedByValue.insert(element);
            }
        }
    }


    /*   std::vector<std::pair<std::pair<int,int>,double> > temp;
                 temp.reserve(delta_scores.size());
                 temp.insert(temp.begin(),delta_scores.begin(),delta_scores.end());

                 std::clog << "Out of " << temp.size() << " splits, we'll merge " << temp.size() * merge_percentage / 100 << std::endl;

                 std::nth_element(temp.begin(), temp.begin() + (temp.size() * merge_percentage / 100), temp.end(),
                 order_2nd_dimension());
                 temp.resize(temp.size() * merge_percentage / 100);*/



    //keep merge_percentage of smallest delta values, store in various data structures -essentially different views of the
    //same data, built for quick lookup- maybe a more elegant way of doing this?

    //map: label -> [vector of start indices of annotation sets that will be merged]
    //the start index of an annotation set is the first annotataion of the annotation set.
    //eg an annotation with start index 3 and split size 4, will contain annotations: {3,4,5,6}
    std::map<int,std::vector<int> > sets_to_merge_map;

    // map: label -> [set of start indices of annoation sets that will be merged]
    std::map<int,std::set<int> > sets_to_merge_map_lookup;

    //map: label -> [set of all indices of annotation sets that will be merged]
    std::map<int,std::set<int> > full_sets_to_merge_map_lookup;

    //hack jlr
    unsigned x_percent = 1 + (merge_count * merge_percentage )/ 100;
    //unsigned x_percent = (merge_count * merge_percentage )/ 100;

    std::cout << "Out of " << delta_scores.size() << " splits, of which  " << delta_sortedByValue.size() << " had non-zero scores, we'll merge " << x_percent << std::endl;



    /*  for(unsigned i = 0; i < temp.size(); ++i) {

                if (verbose)
                std::clog << "MERGING label: " << temp[i].first.first
                << " " << SymbolTable::instance_nt()->translate(temp[i].first.first)
                << " index of start annotations: " << temp[i].first.second << std::endl;

                sets_to_merge_map[temp[i].first.first].push_back(temp[i].first.second);
                sets_to_merge_map_lookup[temp[i].first.first].insert(temp[i].first.second);

                int begin_index = temp[i].first.second;

                for(int idx=begin_index; idx<(begin_index + split_number); ++idx){
                full_sets_to_merge_map_lookup[temp[i].first.first].insert(idx);
                }
                }*/



    unsigned y=0;
    DeltaMultiMap::const_iterator itr =delta_sortedByValue.begin();

    while(itr != delta_sortedByValue.end() && y < x_percent) {
        if (verbose)
            std::cout << "MERGING label: " << itr->second.first << " " << SymbolTable::instance_nt().translate(itr->second.first)
                      << " index of start annotations: " << itr->second.second << " score: " << itr->first << std::endl;

        sets_to_merge_map[itr->second.first].push_back(itr->second.second);
        sets_to_merge_map_lookup[itr->second.first].insert(itr->second.second);

        int begin_index = itr->second.second;

        // what is it ?
        for(int i=begin_index; i<(begin_index + split_number); ++i){
            full_sets_to_merge_map_lookup[itr->second.first].insert(i);
        }
        ++y;
        ++itr;

    }


    AnnotatedLabelsInfo save = em_grammar.get_annotations_info();
    em_grammar.uncompact_all_rules();
    em_grammar.update_merge_annotations(sets_to_merge_map, split_number);
    resize_annotations_in_treebank(trees,em_grammar);

    //merge the annotation sets in the grammar
    em_grammar.merge_rules(save, sets_to_merge_map, split_number, proportions,full_sets_to_merge_map_lookup);
    em_grammar.compact_all_rules();

    annotated_node_counts.clear();

    // std::clog << "testing annotations map figures tally with tree annotations..." << std::endl;
    // test_annotations_number( trees, em_grammar);

    if(final_lex_em) {
        reset_counts(em_grammar);
        expectation(trees,em_grammar,true, vb,vu, vl);
        one_em_cycle_lexicon_only(em_grammar,trees, vl);
    }

    //if(verbose) {
    std::cout << "After merging the grammar the training set likelihood is "
              << calculate_likelihood(trees)  << std::endl;
    //}
}

void EMTrainer::resize_annotations_in_treebank(std::vector<BinaryTrainingTree> & trees, const TrainingGrammar& grammar) const
{
    const AnnotatedLabelsInfo& a_infos = grammar.get_annotations_info();

    for(std::vector<BinaryTrainingTree>::iterator iter(trees.begin()); iter != trees.end(); ++iter) {
        iter->resize_annotations(a_infos);
    }
}


struct reset_counts_helper
{
    template <class T>
    void operator()(T& rule) const {rule.reset_counts();}
};

void EMTrainer::reset_counts(TrainingGrammar& grammar)
{
    reset_counts_helper reset;

    std::for_each(grammar.get_binary_rules().begin(),grammar.get_binary_rules().end(), reset);
    std::for_each(grammar.get_unary_rules().begin(),grammar.get_unary_rules().end(), reset);


    grammar.resize_lexical_datastructs();

    // clear annotated_node_counts
    annotated_node_counts.resize(grammar.get_annotations_info().get_number_of_unannotated_labels());
    for(unsigned i = 0; i < annotated_node_counts.size(); ++i) {
        annotated_node_counts[i].clear();
    }
}


struct reset_counts_soft_helper
{
    template <class T>
    void operator()(T& rule) const {rule.reset_counts_soft();}
};

void EMTrainer::reset_counts_soft(TrainingGrammar& grammar)
{
    reset_counts_soft_helper reset;

    std::for_each(grammar.get_binary_rules().begin(),grammar.get_binary_rules().end(), reset);
    std::for_each(grammar.get_unary_rules().begin(),grammar.get_unary_rules().end(), reset);

    grammar.reset_lexical_counts();

    // clear annotated_node_counts
    for(unsigned i = 0; i < annotated_node_counts.size(); ++i) {
        annotated_node_counts[i].clear();
    }
}

double EMTrainer::calculate_likelihood(std::vector<BinaryTrainingTree>& trees) const
{
    unsigned tree = 0;
    double llikelihood = 0;

    unsigned tree_ignored = 0;

    for(std::vector<BinaryTrainingTree>::iterator tree_iter(trees.begin()); tree_iter != trees.end(); ++tree_iter) {

        ++tree;
        if(verbose){
            if( tree % 100 == 0)
                std::cout << "tree: " << tree << '\r';
        }
        // don't forget to reset probabilities !
        tree_iter->reset_inside_probabilities(0);

        //calculate the inside probability of all nodes in the tree
        tree_iter->compute_inside_probability();

        //update the likelihood with the inside probability of the tree
        //(this will be the inside probability of the root node which has no annotations)
        double log_tree_prob = tree_iter->get_log_probability();

        // std::cout << "Tree " << tree << " " << tree_iter->get_log_probability() << std::endl;
        // std::cout << "\t" << *tree_iter << std::endl;
        if(std::isnan(log_tree_prob)  || log_tree_prob == - std::numeric_limits<double>::infinity()) {
            ++tree_ignored;
            //      std::cout << "ignoring tree with logprob:" << log_tree_prob << std::endl;
            //    std::cout << tree_prob << std::endl << std::endl;
        }
        else {
            llikelihood +=  log_tree_prob;
        }
    }

    if(verbose) {
        std::cout << "tree: " << tree << std::endl;
        std::cout << "ignored: " << tree_ignored << " tree(s)" << std::endl;
    }

    return llikelihood;
}



void EMTrainer::one_em_cycle_lexicon_only(TrainingGrammar& em_grammar,std::vector<BinaryTrainingTree>& trees, std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& vl){

    em_grammar.update_lexical_counts(trees,true, vl, threads);
    em_grammar.maximise_lexical_rules();
    em_grammar.remove_unlikely_annotations_all_rules(threshold);
}


////////////////////
////////debugging code/////////
////////////////////////////////

namespace debug_emtrainer {

void print_sorted_delta_scores(const DeltaMultiMap& sorted_multi_map)
{
    std::cout << "This is the delta map....sorted by value...." <<std::endl;
    DeltaMultiMap::const_iterator itrr;
    for (itrr=sorted_multi_map.begin(); itrr != sorted_multi_map.end(); ++itrr){
        std::cout << "label: " << itrr->second.first
                  << SymbolTable::instance_nt().translate(itrr->second.first)
                  << " " << itrr->second.second << " value: " << itrr->first
                  << std::endl;
    }
}

void print_proportions(const AnnotatedNodeCountMap& proportions)
{
    std::cout.precision(30);

    std::cout << "Printing propostions" << std::endl;
    for(unsigned label = 0;  label < proportions.size(); ++label) {
        unsigned max_annotation = proportions[label].size();
        for(unsigned annotation = 0 ; annotation < max_annotation; ++annotation)
            std::cout << "Label: " << label << " "
                      << SymbolTable::instance_nt().translate(label) << std::endl
                      << "\t Latent annotations: " << annotation << ": "
                      << proportions[label].at(annotation) << std::endl;
    }
}

void print_node_counts(const AnnotatedNodeCountMap& annotated_node_counts)
{
    std::cout.precision(30);
    std::cout << "Printing annotated node counts " << std::endl;
    for(unsigned label = 0;  label < annotated_node_counts.size(); ++label) {
        unsigned max_annotation = annotated_node_counts[label].size();
        for(unsigned annotation = 0 ; annotation < max_annotation; ++annotation)
            std::cout << "Label: " << label << " "
                      << SymbolTable::instance_nt().translate(label) << std::endl
                      << "\tLatent annotations: " << annotation << ": "
                      << annotated_node_counts[label].at(annotation) << std::endl;
    }
}

void print_split_sets(const AnnotatedLabelsInfo& annotation_info)
{

    std::cout << "Printing split sets...  " << std::endl;
    int total_num_labels = annotation_info.get_number_of_unannotated_labels();
    for(int i = 0; i<total_num_labels; ++i){
        std::cout << "Label: "  << SymbolTable::instance_nt().translate(i)
                  << "\tnum of Latent annotations: "
                  << annotation_info.get_number_of_annotations(i) << std::endl;
    }
}

void print_delta_scores(const DeltaMap& delta_scores)
{
    std::cout.precision(30);
    std::cout << "Printing the delta scores..." << std::endl;

    for(DeltaMap::const_iterator itr(delta_scores.begin()); itr != delta_scores.end();
        ++itr) {
        std::cout << "Label: " << itr->first.first <<  " "
                  << SymbolTable::instance_nt().translate(itr->first.first) << std::endl;
        std::cout << "\tMerge begin index: " << itr->first.second
                  << ": " << itr->second << std::endl;
    }
}
}
