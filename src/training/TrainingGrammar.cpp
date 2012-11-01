#include "TrainingGrammar.h"


namespace helpers {


    struct split
    {
        unsigned split_size, randomness;
        split(unsigned split_size_, unsigned randomness_)
            : split_size(split_size_), randomness(randomness_) {}

        template<class T>
            void operator()(T& rule) const  {rule.split(split_size,randomness);}
    };


    struct merge
    {
        const Merge_map& annotation_sets_to_merge;
        int split_size;
        const ProportionsMap& proportions;
        const AnnotatedLabelsInfo& ali;
        const std::vector<std::map<int,int> >& annot_reorder;

        merge(const Merge_map& annotation_sets_to_merge_, int split_size_,
                const ProportionsMap& proportions_, const AnnotatedLabelsInfo& a_,
                const std::vector<std::map<int,int> >& annot_reorder_)
            :
                annotation_sets_to_merge(annotation_sets_to_merge_), split_size(split_size_), proportions(proportions_),
                ali(a_), annot_reorder(annot_reorder_)
        {};

        template <class T>
            void operator()(T& rule) const {rule.merge(annotation_sets_to_merge,
                    split_size,
                    proportions,
                    ali,
                    annot_reorder);
            }
    };


    struct linear_smooth
    {
        double alpha;
        linear_smooth(const double& alpha_) : alpha(alpha_) {}

        template<class T>
            void operator() (T& rule) const {rule.linear_smooth(alpha);}
    };


    struct weighted_smooth
    {
        double alpha;
        std::vector<std::vector<double> > weights;

        void reset_alpha(const double& alpha_) {alpha = alpha_;}

        weighted_smooth(const double& alpha_, const std::vector< Tree<unsigned> >& annot_histories) :
            alpha(alpha_), weights(annot_histories.size())
        {
            for(unsigned i = 0; i < weights.size(); ++i) {
                const Tree<unsigned>& tree = annot_histories[i];
                unsigned height = tree.height();

                weights[i].resize(tree.number_of_leaves());

                for(Tree<unsigned>::const_leaf_iterator l(tree.lbegin()); l != tree.lend(); ++l) {
                    unsigned idx = *l;
                    unsigned branching = 1;

                    Tree<unsigned>::const_depth_first_iterator m =l;

                    //assume binary branching
                    //update branching
                    // ugly ! check that iterators to the 1st and last daughters are different
                    while(m.up() != tree.dfend()) {
                        // test if n-ary branching
                        Tree<unsigned>::const_depth_first_iterator save;

                        save = m;
                        if(m.down_first()->has_right_sister() ) {++branching;}
                        m = save;


                    }

                    //assume binary branching
                    // TODO remove assumption and work with n-ary
                    weights[i][idx] = double(1 << (height - branching));
                }
            }
        }

        template<class T> void operator() (T& rule) const {rule.weighted_smooth(alpha, weights);}

    };

    // template <typename T>
    // std::vector< Tree<T> >get_at_depth(unsigned depth, const Tree<T>& tree)
    // {
    //   std::vector< Tree<T> > result;
    //   unsigned target_height = tree.height() - depth;

    //   for (typename Tree<T>::const_depth_first_iterator i(tree.dfbegin()); i != tree.dfend(); ++i) {

    //     Tree<T> t = tree.subtree(i);

    //     if( t.height == target_height)
    // 	result.push_back(t);
    //   }


    //   return result;
    // }


    struct generation_smooth
    {
        double alpha;
        std::vector<std::vector<std::vector<double> > > weights;
        //       std::vector< std::vector< std::map<unsigned,unsigned> > > reverse_generation_mapping;


        void compute_weight(double alpha, const Tree<unsigned>& annot_history, std::vector<std::vector<double> >& myweights)
        {

            //reset myweights
            std::vector<std::vector<double> >().swap(myweights);

            typedef std::vector< Tree<unsigned>::const_depth_first_iterator >::const_iterator iter;

            unsigned tree_height = annot_history.height();

            //for all layers of the tree
            //	 std::cout << annot_history.height() << " " << annot_history << std::endl;
            for (unsigned h = 1; h < tree_height; ++h) {
                //	   std::cout << "h = " << h << std::endl;

                // 1. calculate generation mappings
                std::vector< Tree<unsigned>::const_depth_first_iterator > subtree_roots = annot_history.get_at_depth(h);
                std::map<unsigned,unsigned> nod; //number of descendants
                std::map<unsigned,unsigned> reverse_generation_mapping;


                //	   std::cout << "size subtree_roots: " << subtree_roots.size() << std::endl;
                for(iter j(subtree_roots.begin()); j != subtree_roots.end(); ++j) {
                    Tree<unsigned>::const_depth_first_iterator tree_iter = *j;
                    Tree<unsigned>::const_depth_first_iterator save = tree_iter;

                    tree_iter.up();
                    Tree<unsigned> up_tree = annot_history.subtree(tree_iter);
                    //	     std::cout << "\t\t" <<up_tree.height() << " " << up_tree << std::endl;

                    reverse_generation_mapping[*save] = *tree_iter;
                    nod[*tree_iter]++;

                    // std::cout << "reverse_generation_mapping[" << *save << "]: " << reverse_generation_mapping[*save] << std::endl;
                    // std::cout << "nod[" << *tree_iter <<"]: " << nod[*tree_iter] << std::endl;
                }


                std::vector<std::vector<double > > new_weights(subtree_roots.size(), std::vector<double>(subtree_roots.size()));

                //std::cout << "h: " << h << " size new_weights: " << new_weights.size() << std::endl;
                // 2. update weights

                if (h == 1) {
                    for(iter j(subtree_roots.begin()); j != subtree_roots.end(); ++j) {
                        Tree<unsigned>::const_depth_first_iterator tree_iter = *j;
                        tree_iter.up();

                        if(nod[*tree_iter] == 1) {
                            //		 std::cout << "nod is 1" << std::endl;
                            new_weights[0][0]=1.0;
                        }
                        else {
                            //		 std::cout << "nod is > 1" << std::endl;
                            double alphabar = 1 - alpha;
                            double other = alpha / (subtree_roots.size() - 1);

                            // std::cout << "alpha: " << alpha << std::endl;
                            // std::cout << "alphabar: " << alphabar << std::endl;
                            // std::cout << "other: " << other << std::endl;

                            for (unsigned a = 0; a < subtree_roots.size(); ++a) {
                                for (unsigned b = 0; b < subtree_roots.size(); ++b) {
                                    if(a == b)
                                        new_weights[a][b] = alphabar;
                                    else
                                        new_weights[a][b] = other;
                                }
                            }
                            // for (unsigned a = 0; a < subtree_roots.size(); ++a)
                            //   for (unsigned b = 0; b < subtree_roots.size(); ++b)
                            //	     std::cout << "new_weights[" << a << "][" << b << "]: " << new_weights[a][b] << std::endl;


                        }

                    }
                }
                else
                {
                    // if(subtree_roots.size() == 1)
                    // 	 new_weights[0][0] = 1.0;
                    // else
                    // 	 {

                    std::vector<double> total(subtree_roots.size());

                    for(unsigned s1 = 0 ; s1 < subtree_roots.size(); ++s1)
                    {
                        for(unsigned s2 = 0; s2 < subtree_roots.size(); ++s2)
                        {
                            unsigned s1_mother = reverse_generation_mapping[s1];
                            unsigned s2_mother = reverse_generation_mapping[s2];
                            new_weights[s1][s2] += myweights[s1_mother][s2_mother];
                            total[s1] += myweights[s1_mother][s2_mother];
                        }
                    }
                    for(unsigned s1 = 0 ; s1 < subtree_roots.size(); ++s1)
                    {
                        for(unsigned s2 = 0; s2 < subtree_roots.size(); ++s2)
                        {
                            new_weights[s1][s2] /= total[s1];
                        }
                    }
                }
                // }
                myweights = new_weights;
            }
        }

        generation_smooth(const double& alpha_, const std::vector< Tree<unsigned> >& annot_histories)
            : alpha(alpha_), weights(annot_histories.size())
        {
            // for each symbol
            for (unsigned i = 0; i < annot_histories.size(); ++i) {

                compute_weight(alpha,annot_histories[i],weights[i]);
            }
        }

        template<class T> void operator() (T& rule) const {rule.generation_smooth(weights);}


    };





    struct remove_unlikely
    {
        double threshold;
        remove_unlikely(const double& threshold_) : threshold(threshold_) {}

        template<class T>
            void operator() (T& rule) const {rule.remove_unlikely_annotations(threshold);}
    };


    struct compact { template<class T> void operator()(T& rule) const {rule.compact();}};


    struct out
    {
        std::ostream& os;
        out(std::ostream& os_) : os(os_) {}

        template<class T>
            void operator()(const T& rule) {os << rule << '\n';}
    };


    // we don't want to copy the vectors of rules,
    // but rather copy the rules inside them, pointwise,
    // because we don't want to mess with pointers pointing
    // on the rules contained in these vectors

    struct copy
    {
        template<typename T>
            const T& operator()(const T& rule) const {return rule;}
    };



    template <typename T>
    void perform_on_lexicon(Lexicon * lex, T& action)
    {
        std::vector<LexicalRuleTraining>& lr = lex->get_lexical_rules();
        std::vector<LexicalRuleTraining>& ar = lex->get_additional_rules();
        
        std::for_each(lr.begin(),lr.end(),action);
        std::for_each(ar.begin(),ar.end(),action);
    }
}


    TrainingGrammar::TrainingGrammar(Treebank<PtbPsTree> & tb, Lexicon* lex)
: Grammar<BRuleTraining, URuleTraining, LexicalRuleTraining>(),
    unannotated_node_priors(), lexicon(lex)
{
    //collect basic rule counts and normalise to get probabilities for base grammar
    typedef std::map<Production, double> production_counts_map;

    production_counts_map binary_counts, unary_counts;

    std::map< int, double> LHS_counts;

    tb.collect_internal_counts(binary_counts,unary_counts,LHS_counts);

    //read lexicon from treebank and map unknowns in treebank
    std::clog << "about to read lexicon from treebank trees" <<std::endl;
    lexicon->read_lexicon_from_Treebank(tb.get_trees());


    //normalise counts
    for(production_counts_map::const_iterator it(binary_counts.begin()); it != binary_counts.end(); ++it) {
        const Production& production = it->first;
        double prob = double(it->second)/ double(LHS_counts[production.get_lhs()]);
        binary_rules.push_back(BRuleTraining(production.get_lhs(), production.get_rhs0(), production.get_rhs1(),prob));
    }

    for(production_counts_map::const_iterator it(unary_counts.begin()); it != unary_counts.end(); ++it) {
        const Production& production = it->first;
        double prob = double(it->second) / double(LHS_counts[production.get_lhs()]);
        unary_rules.push_back(URuleTraining(production.get_lhs(), production.get_rhs0(), prob));
    }

    initialise_annotations_map();
    std::clog << "created " << (binary_rules.size() + unary_rules.size()) << " internal rules" << std::endl;


    // initialize annot histories
    // make a method ?
    for (unsigned i = 0; i < label_annotations.get_number_of_unannotated_labels(); ++i)
    {
        Tree<unsigned> tree;
        tree.add_root(0);
        annot_histories.push_back(tree);
    }


    //get priors
    double total_nonterm_count=0;
    for (std::map<int, double>::const_iterator p_itr = LHS_counts.begin(); p_itr != LHS_counts.end(); ++p_itr){
        total_nonterm_count += p_itr->second;
    }
    //std::cout << "total number of non-terminals.." << total_nonterm_count <<std::endl;
    for (std::map<int, double>::const_iterator p_itr = LHS_counts.begin(); p_itr != LHS_counts.end(); ++p_itr){
        unannotated_node_priors[p_itr->first] = (double) p_itr->second/ (double) total_nonterm_count;
        //std::cout << "prior: " << unannotated_node_priors[p_itr->first] << std::endl;
    }

    //  output_counts(binary_counts,unary_counts,LHS_counts);
}


/*
   TrainingGrammar::TrainingGrammar(const std::vector<BRuleTraining>& br,
   const std::vector<URuleTraining>& ur,
   const std::vector<URuleTraining>& lr) :
   binary_rules(br), unary_rules(ur), lexical_rules(lr)
   {
   initialise_annotations_map();
   }*/


std::vector<std::map<int,int> > compute_annotation_reorder(int split_size,
        const AnnotatedLabelsInfo ali,
        const std::map<int,std::set<int> > & full_sets_to_merge_lookup)
{
    std::vector<std::map<int,int> > result(ali.get_number_of_unannotated_labels());

    for(unsigned i = 0; i < ali.get_number_of_unannotated_labels(); ++i) {

        unsigned new_index = 0;
        for(unsigned j = 0 ; j < ali.get_number_of_annotations(i); ++j) {
            if(full_sets_to_merge_lookup.count(i) &&
                    full_sets_to_merge_lookup.find(i)->second.count(j) &&
                    (j % split_size) != 0)
                continue;

            result[i][j] = new_index++;
        }
    }
    return result;
}


void TrainingGrammar::merge_rules(const AnnotatedLabelsInfo& old_ali,
        const Merge_map& annotation_sets_to_merge, int split_size,
        const ProportionsMap& proportions,
        const std::map<int,std::set<int> > & full_sets_to_merge_lookup)
{
    std::vector<std::map<int,int> > annot_reorder = compute_annotation_reorder(split_size,
            old_ali,
            full_sets_to_merge_lookup);

    //modify annot_histories trees
    // TODO make a method
    for(unsigned i = 0; i < annot_reorder.size() ; ++i) {
        Tree<unsigned>& tree = annot_histories[i];

        //std::cout << SymbolTable::instance_nt()->translate(i) << ":" << "\n\t" << tree << std::endl;

        //remove annotations
        for(Tree<unsigned>::depth_first_iterator t = tree.dfbegin(); t != tree.dfend(); ++t) {
            if(t->leaf()) {
                if(!annot_reorder[i].count(*t)) {
                    Tree<unsigned>::depth_first_iterator u = t;
                    u.up();
                    tree.erase(t);
                    t=u;
                }
            }
        }

        //    std::cout << "\t"  << tree << std::endl;

        // rename annotations
        // for some reasons we can't do these 2 operations at the same time or iterators
        // get messed up ?
        for(Tree<unsigned>::leaf_iterator l(tree.lbegin()); l != tree.lend(); ++l) {
            *l = annot_reorder[i].find(*l)->second;
        }

        //    std::cout << "\t"  << tree << std::endl;
    }


    helpers::merge merger(annotation_sets_to_merge, split_size, proportions,
            get_annotations_info(), annot_reorder);

    perform_action_all_internal_rules(merger);
    helpers::perform_on_lexicon(lexicon,merger);
}




void TrainingGrammar::split_all_rules(unsigned split_size, unsigned randomness )
{
    helpers::split splitter(split_size,randomness);
    perform_action_all_internal_rules(splitter);
    helpers::perform_on_lexicon(lexicon,splitter);


    // extend annot_histories
    //TODO make a method
    typedef Tree<unsigned>::leaf_iterator literator;
    for (unsigned i = 0; i < label_annotations.get_number_of_unannotated_labels(); ++i)
    {
        if( i ==  SymbolTable::instance_nt().get(LorgConstants::tree_root_name)) {
            // add one daughter
            Tree<unsigned>& tree = annot_histories[i];
            literator iter(tree.lbegin());
            tree.add_last_daughter(iter,0);
        }
        else {
            // add split_size _daughters
            Tree<unsigned>& tree = annot_histories[i];
            literator iter(tree.lbegin());
            while(iter != tree.lend()) {
                literator current  =  iter;
                literator  next = ++iter;

                for(unsigned j=0; j< split_size;++j)
                    tree.add_last_daughter(current, *current * split_size + j);
            }
            //	std::cout << SymbolTable::instance_nt()->translate(i) << ": " << tree << std::endl;
        }
    }
}



void TrainingGrammar::smooth_all_rules(double alpha_grammar, double alpha_lexicon, SmoothType type)
{

    if(type == GenerationSmooth) {
        if(alpha_lexicon != 0.0) {
            //     std::cout << "before lex" << std::endl;
            helpers::generation_smooth test_lex(alpha_lexicon,annot_histories);
            helpers::perform_on_lexicon(lexicon, test_lex);
        }

        if(alpha_grammar != 0.0) {
            //      std::cout << "before gram" << std::endl;
            helpers::generation_smooth test_gra(alpha_grammar,annot_histories);

            perform_action_all_internal_rules(test_gra);
        }
    }


    if(type == LinearSmooth) {
        helpers::linear_smooth sgram(alpha_grammar);
        helpers::linear_smooth slex(alpha_lexicon);

        perform_action_all_internal_rules(sgram);
        helpers::perform_on_lexicon(lexicon, slex);
    }
    else { // WeighedSmooth
        helpers::weighted_smooth smooth(alpha_grammar, annot_histories);

        perform_action_all_internal_rules(smooth);
        smooth.reset_alpha(alpha_lexicon);
        helpers::perform_on_lexicon(lexicon,smooth);
    }
}


void TrainingGrammar::remove_unlikely_annotations_all_rules(const double& threshold)
{
    helpers::remove_unlikely remover(threshold);
    perform_action_all_internal_rules(remover);
    helpers::perform_on_lexicon(lexicon,remover);
}

void TrainingGrammar::reset_merged_num_annotations(int label,int num_merges,int split_size)
{
    // std::cout << "label " << label
    //  	    << " num_merges " << num_merges
    // 	    << " split size " << split_size
    // 	    <<  " in map before: " << label_annotations.get_number_of_annotations(label) << std::endl;
    if (SymbolTable::instance_nt().is_root_label(label)) return;

    int new_number_annotations = label_annotations.get_number_of_annotations(label) - (num_merges * (split_size -1));
    label_annotations.set_number_of_annotations(label,new_number_annotations);
    assert(label_annotations.get_number_of_annotations(label)>0);
}

void TrainingGrammar::reset_split_num_annotations(int label,int split_size)
{
    if (SymbolTable::instance_nt().is_root_label(label)) return;

    int new_number_annotations =label_annotations.get_number_of_annotations(label) * split_size;
    label_annotations.set_number_of_annotations(label,new_number_annotations);
}

void TrainingGrammar::update_merge_annotations(const Merge_map& sets_to_merge_map, int split_size){
    //reset the number_of_annotations per label map in the grammar
    for(std::map<int,std::vector<int> >::const_iterator i(sets_to_merge_map.begin()); i != sets_to_merge_map.end() ; ++i){
        reset_merged_num_annotations(i->first,i->second.size(),split_size);
    }
}

void TrainingGrammar::update_split_annotations(int split_size)
{
    //update annotations map
    unsigned total_number_of_labels = label_annotations.get_number_of_unannotated_labels();
    for (unsigned i = 0 ; i < total_number_of_labels; ++i){
        reset_split_num_annotations(i,split_size);
    }
}

// prints the prior probabilities
void TrainingGrammar::output_priors(std::ostream* out)
{
    out->precision(22);


    for(boost::unordered_map< int, double >::const_iterator map_iter =  unannotated_node_priors.begin();
            map_iter != unannotated_node_priors.end(); ++map_iter) {

        *out << SymbolTable::instance_nt().translate(map_iter->first)
            << " -> " << map_iter->second << std::endl;
    }
}

void TrainingGrammar::uncompact_all_rules()
{

    for(std::vector<BRuleTraining>::iterator brule_it = binary_rules.begin();
            brule_it != binary_rules.end(); ++brule_it) {

        brule_it->uncompact(label_annotations.get_number_of_annotations(brule_it->get_rhs0()),
                label_annotations.get_number_of_annotations(brule_it->get_rhs1()));
    }

    for(std::vector<URuleTraining>::iterator urule_it = unary_rules.begin();
            urule_it != unary_rules.end(); ++urule_it) {
        urule_it->uncompact(label_annotations.get_number_of_annotations(urule_it->get_rhs0()));
    }

    // lexical rules are not compacted
}

void TrainingGrammar::compact_all_rules()
{
    helpers::compact compact;
    perform_action_all_internal_rules(compact);
    // lexical rules are not compacted
}

std::ostream& operator<<(std::ostream& os, const TrainingGrammar& gram)
{
    os << gram.get_annotations_info() << std::endl;

    for(unsigned i = 0; i < gram.get_annot_histories().size();++i) {
        os << gram.get_annot_histories()[i] << std::endl;
    }

    helpers::out output(os);
    gram.perform_action_all_internal_rules(output);
    helpers::perform_on_lexicon(gram.lexicon,output);

    return os;
}


//inline
TrainingGrammar& TrainingGrammar::operator=(const TrainingGrammar& other)
{
    if(this != &other) {
        label_annotations = other.label_annotations;

        // TODO find a way to compute these priors not in the constructor
        // so the map needs not be an attribute
        // They're useless in the split/merge cycle
        //    unannotated_node_priors = other.unannotated_node_priors;

        helpers::copy copy;
        binary_rules.resize(other.binary_rules.size());
        unary_rules.resize(other.unary_rules.size());
        std::transform(other.binary_rules.begin(), other.binary_rules.end(), binary_rules.begin(), copy);
        std::transform(other.unary_rules.begin(), other.unary_rules.end(), unary_rules.begin(), copy);

        //    delete lexicon.release();
        //    lexicon = other.lexicon->copy();
        assert(other.lexicon);
        other.lexicon->copy(lexicon);

        annot_histories = other.annot_histories;

    }

    return *this;

}


void TrainingGrammar::reset_unannotated_priors()
{
    unannotated_node_count_map().swap(unannotated_node_priors);
}
