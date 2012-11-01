// -*- mode: c++ -*-
#ifndef PARSERCKYALL_H
#define PARSERCKYALL_H

#include "grammars/GrammarAnnotated.h"
#include "ParserCKY.h"


#include "ChartCKY.h"
#include "edges/PackedEdge.h"

#include "PCKYAllCell.h"

#include "rules/BRule.h"
#include "rules/URule.h"

#include "utils/RuleVect2Map.h"

#include "utils/data_parsers/BURuleInputParser.h"

#include <algorithm>
#include <numeric>

#define USE_THREADS 1

#ifdef USE_THREADS
#include <tbb/parallel_for.h>
#include <tbb/tick_count.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
using namespace tbb;
#endif 


typedef std::vector< std::vector<std::vector< std::vector<unsigned> > > > annot_descendants_type;


class ParserCKYAll : public ParserCKY< GrammarAnnotated<BRuleC2f,URuleC2f, LexicalRuleC2f> >
{
public:
    typedef GrammarAnnotated<BRuleC2f,URuleC2f, LexicalRuleC2f> AGrammar;
    typedef ParserCKY<AGrammar> Parser;
    /**
     \brief ParserCKYAll destructor
     */
    virtual ~ParserCKYAll() {};
    
    /**
     \brief ParserCKYAll constructor
     \param cgs the grammars from the coarsest to the finest
     \param prior_map, the prior values for each NT
     \param beam_threshold, the threshold for prior-based pruning
     \param annot_descendants the data structures for C2F mappings
     \param accurate, triggers accurate mode
     \param min_beam, minimum length for sentences to apply pruning
     \param stubborn, number of retries
     */
    ParserCKYAll(std::vector<AGrammar*>& cgs, const std::vector<double>& prior_map, double beam_threshold,
                 const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants,
                 bool accurate, unsigned min_beam, int stubborn, unsigned nbCellThreads);
    
    /**
     \brief parses the sentence using the grammar
     \param start_symbol the axiom symbol of the grammar
     */
    virtual void parse(int start_symbol) const = 0;
    
    
    /**
     \brief Performs the PCFG-LA coarse-to-fine beam
     */
    virtual void beam_c2f(int start_symbol) = 0;
    
    
    /**
     \brief computes the best solution (depends on the algorithm)
     */
    virtual void extract_solution()=0;
    
    virtual   void initialise_chart(const std::vector< Word >& s,
                                    const std::vector<bracketing>& bs) = 0;
    
    virtual bool is_chart_valid(int start_symbol) = 0;
    
    virtual void get_parses(int start_symbol, unsigned kbest, bool always_output_forms, bool output_annotations,
                            std::vector<std::pair<PtbPsTree *,double> >& best_trees) = 0;
    
    virtual void clean() = 0;
    
    
protected: // attributes
    std::vector<AGrammar *> grammars; ///< the grammars to beam
    const std::vector<double> priors;  ///< used for prior-based beam
    
    double prior_beam_threshold; ///< the default threshold value for prior-based pruning
    
    
    std::vector< std::vector<std::vector< std::vector<unsigned> > > > annot_descendants; // fast access to annotation histories
    
    std::vector<double> io_beam_thresholds; ///< thresholds used for c2f IO beam (one per c2f level)
    
    bool accurate;  ///< triggers accurate/slow parsing thresholds
    
    unsigned min_length_beam; ///< minimum sentence length to apply beam on
    
    int stubbornness; ///< number of tries with incremental prior-based pruning
    
    unsigned num_cell_threads; ///< number of threads for cell parallel processing
};


/**
 \class ParserCKYAll
 \brief represents a parsing device for probabilistic cfgs using the cky algorithm
 */
template<class Probability>
class ParserCKYAll_Impl : public ParserCKYAll
{
public:
    typedef PCKYAllCell<PackedEdge<Probability> > Cell;
    typedef ChartCKY<Cell, Word> Chart;
    
    
    /**
     \brief ParserCKYAll_Impl destructor
     */
    virtual ~ParserCKYAll_Impl() ;
    
    /**
     \brief ParserCKYAll_Impl constructor
     \param cgs the grammars from the coarsest to the finest
     \param prior_map, the prior values for each NT
     \param beam_threshold, the threshold for prior-based pruning
     \param annot_descendants the data structures for C2F mappings
     \param accurate, triggers accurate mode
     \param min_beam, minimum length for sentences to apply pruning
     \param stubborn, number of retries
     */
    ParserCKYAll_Impl(std::vector<AGrammar*>& cgs, const std::vector<double>& prior_map, double beam_threshold,
                      const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants,
                      bool accurate, unsigned min_beam, int stubborn, unsigned nbCellThreads);
    
    /**
     \brief parses the sentence using the grammar
     \param start_symbol the axiom symbol of the grammar
     */
    void parse(int start_symbol) const;
    
    /**
     \brief computes the best solution (depends on the algorithm)
     */
    virtual void extract_solution() =0;
    
    /**
     \brief gets the idx^th grammar of the c2f-parser
     \param idx the rank of the grammar
     \return the idx^th grammar in the c2f-chain
     */
    AGrammar& get_grammar(unsigned idx);
    
    
    /**
     \brief gets the idx^th grammar of the c2f-parser
     \param idx the rank of the grammar
     \return the idx^th grammar in the c2f-chain
     */
    const AGrammar& get_grammar(unsigned idx) const;
    
    /**
     \brief Performs the PCFG-LA coarse-to-fine beam
     */
    void beam_c2f(int start_symbol);
    
    
    /**
     \brief returns the sentence score (depends on the parsing algorithm)
     \return a score
     */
    double get_sentence_probability() const;
    
    
    void initialise_chart(const std::vector< Word >& sentence, const std::vector<bracketing>& brackets)
    { chart = new Chart(sentence, get_nonterm_count(), brackets);}
    
    
    void clean() { delete chart; chart = NULL;}
    
    
private:
    /** \brief Add unary rules at this position in the chart
     (only consider non-terminals created from binary rules)
     \param cell the cell to fill
     \param isroot true if cell is root
     */
    void add_unary_internal(Cell& cell, bool isroot) const;
    
    /** \brief Add unary rules at this position in the chart
     (only consider non-terminals created from pos)
     \param cell the cell to fill
     \param isroot true if cell is root
     */
    void add_unary_init(Cell& cell, bool isroot) const;
    
    /**
     \brief processes the internal rules, adding edges to the chart where appropriate
     \param beam_threshold the prior beam threshold to prune the base packed forest
     */
    void process_internal_rules(double beam_threshold) const;
    void process_cell(Cell& cell, double beam_threshold, unsigned begin, unsigned end, bool isroot) const;
    
    /**
     \brief fill the result cell with the most probable edges for each lhs,
     created from edges contained in left_cell and right_cell
     \param left_cell the  leftmost cell to combine
     \param right_cell the rightmost cell to combine
     \param result_cell the cell to store new edges
     */
    void get_candidates(Cell& left_cell,
                        Cell& right_cell,
                        Cell& result_cell) const;
    
    /**
     \brief process all unary rules for this edge - unary chains are not followed
     \param cell the cell to fill
     \param lhs the entry in the cell
     \param isroot true if cell is the rootcell
     
     */
    void process_unary(Cell& cell, int lhs, bool isroot) const;
    
    
    /**
     \brief create an efficient data structure for c2f parsing
     from a vector of trees denoting annotation histories
     \param annot_histories the trees
     */
    void create_annot_descendants(const std::vector< Tree<unsigned> >& annot_histories);
    
protected:
    /**
     \brief computes the inside probability for all nodes in chart
     */
    void compute_inside_probabilities() const;
    
    /**
     \brief computes the outside probability for all nodes in chart
     */
    void compute_outside_probabilities() const;
    
    
    /**
     \brief computes the inside & outside scores in the chart
     */
    void compute_inside_outside_probabilities() const;
    
    
    /**
     \brief prunes the chart according to I/O scores of nodes
     if a node I/0 is lower by k% than the best node in the cell, remove it
     */
    void beam_chart_io_relative() const;
    
    
    /**
     \brief beam the chart
     \param sent_prob, the score of the current sentence (with the base grammar)
     \param threshold, threshold for pruning
     \param huang, triggers the use of huang/charniak pruning with the base grammar
     */
    void beam_chart(double sent_prob, double threshold, bool huang) const;
    
    
    /**
     \brief creates a c2f grammar from a vector of grammar
     \param cgs, grammars from coarsest to finest
     */
    void create_coarse_to_fine_mapping(std::vector<AGrammar*>& cgs);
    
    
    /**
     \brief Performs the PCFG-LA coarse-to-fine beaming
     \para current_grammars the grammars from coarsest to finest
     \para current_annot_descendants the histories of annotations
     */
    void beam_c2f(const std::vector<AGrammar*>& current_grammars,
                  const annot_descendants_type& current_annot_descendants) const;
    
    void change_rules_resize(unsigned step,
                             const std::vector<AGrammar*>& current_grammars) const;
    
    
    bool is_chart_valid(int start_symbol) {return chart->is_valid(start_symbol);}
    
    
    void get_parses(int start_symbol, unsigned kbest, bool always_output_forms, bool output_annotations,
                    std::vector<std::pair<PtbPsTree *,double> >& best_trees);
    
    
protected: // attributes
    Chart * chart; // the chart
    
    
#ifdef USE_THREADS    
    template<typename P>
    struct ProcessCellBody {
        
        const ParserCKYAll_Impl<P>& parser;
        double beam_threshold;
        bool isroot;
        unsigned span;
        
        ProcessCellBody(const ParserCKYAll_Impl<P>& p,
                        double beam_threshold_,
                        bool isroot_,
                        unsigned span_)
        :parser(p), beam_threshold(beam_threshold_),
        isroot(isroot_), span(span_)
        {}
        
        void operator()(const blocked_range<unsigned>& r) const
        {
            unsigned real_begin = r.begin();
            if(real_begin > 0) ++real_begin;
            
            for(unsigned begin = real_begin; begin <= r.end(); ++begin)
            {
                unsigned end = begin + span - 1;
                Cell& result_cell = parser.chart->access(begin, end);
                if(!result_cell.is_closed()) {
                    parser.process_cell(result_cell, beam_threshold, begin, end, isroot);
                }
            }
        }
    };
    friend struct ProcessCellBody<Probability>;
#endif
    
    
};


template<>
GrammarAnnotated<BRuleC2f, URuleC2f, LexicalRuleC2f>::GrammarAnnotated(const std::string& filename)
:
Grammar<BRuleC2f, URuleC2f, LexicalRuleC2f>::Grammar(),
AnnotatedContents(),
viterbi_decoding_paths()
{
    std::map<short ,unsigned short> map;
    
    std::vector<BRule> bin;
    std::vector<URule> un;
    std::vector<LexicalRule> lex;
    
    
    BURuleInputParser::read_rulefile(filename, lex, un, bin, map, history_trees);
    
    label_annotations.set_num_annotations_map(map);
    
    lexical_rules.insert(lexical_rules.end(),lex.begin(),lex.end());
    unary_rules.insert(unary_rules.end(),un.begin(),un.end());
    binary_rules.insert(binary_rules.end(),bin.begin(),bin.end());
    
    
    // copied from void TrainingGrammar::uncompact_all_rules()
    for(std::vector<BRuleC2f>::iterator brule_it = binary_rules.begin();
        brule_it != binary_rules.end(); ++brule_it) {
        
        brule_it->uncompact(label_annotations.get_number_of_annotations(brule_it->get_rhs0()),
                            label_annotations.get_number_of_annotations(brule_it->get_rhs1()));
    }
    
    for(std::vector<URuleC2f>::iterator urule_it = unary_rules.begin();
        urule_it != unary_rules.end(); ++urule_it) {
        urule_it->uncompact(label_annotations.get_number_of_annotations(urule_it->get_rhs0()));
    }
    
    // lexical rules are not compacted
    // initialisation in  parserckyallfactory
}


// namespace boost {
//   namespace serialization {

//     template<class Archive>
//     void serialize(Archive & ar, ParserCKYAll & p, const unsigned int version)
//     {

//       ar & p.grammar;
//       ar & p.brules;
//       ar & p.unary_rhs_2_rules;
//       ar & p.unary_rhs_from_binary;
//       ar & p.unary_rhs_from_pos;
//       ar & p.words_to_rules;
//       ar & p.n_nonterminals;
//     }

//   } // namespace serialization
// } // namespace boost



ParserCKYAll::ParserCKYAll(std::vector<AGrammar*>& cgs,
                           const std::vector<double>& p,
                           double prior_threshold,
                           const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_,
                           bool accurate_,
                           unsigned min_beam, int stubborn, unsigned nbCellThreads)
: Parser(cgs[0]),
grammars(cgs),
priors(p), prior_beam_threshold(prior_threshold),
annot_descendants(annot_descendants_),
accurate(accurate_),
min_length_beam(min_beam),
stubbornness(stubborn),
num_cell_threads(nbCellThreads)
{
    // these thresholds look familiar ;)
    if(accurate) {
        //extra accurate
        //double t_acc[] = {-1000, -1000, -1000, -1000, -1000, -1000,-1000};
        double t_acc[] = {-8, -12, -12, -11, -12, -12, -14, -17};
        io_beam_thresholds = std::vector<double> (t_acc,t_acc+8);
    }
    else {
        double t[] ={-8, -9.75, -10, -9.6, -9.66, -8.01, -7.4, -10, -12};
        io_beam_thresholds = std::vector<double> (t,t+8);
    }
    
    
    // // create and open a character archive for output
    // std::ofstream ofs("filename");
    
    // // save data to archive
    // {
    //   boost::archive::text_oarchive oa(ofs);
    //   // write class instance to archive
    //   oa << *this;
    //   // archive and stream closed when destructors are called
    // }
}






template <typename P>
ParserCKYAll_Impl<P>::ParserCKYAll_Impl(std::vector<AGrammar*>& cgs,
                                        const std::vector<double>& p,
                                        double prior_threshold,
                                        const std::vector< std::vector<std::vector< std::vector<unsigned> > > >& annot_descendants_,
                                        bool accurate_,
                                        unsigned min_beam, int stubborn, unsigned nbCellThreads) :
ParserCKYAll(cgs, p, prior_threshold, annot_descendants_, accurate_, min_beam, stubborn, nbCellThreads),
chart(NULL)
{};



template <typename P>
ParserCKYAll_Impl<P>::~ParserCKYAll_Impl()
{
    for (std::vector<AGrammar*>::iterator i(grammars.begin()); i != grammars.end(); ++i)
        if(i != grammars.begin()) // the first grammar is deleted by super class
        {
            delete *i;
            *i = NULL;
        }
}

template <typename P>
void ParserCKYAll_Impl<P>::parse(int start_symbol) const
{
    int ntries = stubbornness;
    double beam_threshold = prior_beam_threshold;
    
    do {
        
        //clear only when first try was a failure
        if(ntries != stubbornness) {
            chart->prepare_retry();
        }
        
        // last resort
        if(ntries == 0)
            beam_threshold = 0;
        
        //    std::clog << "ParserCKY::parse ntries = " << ntries << " threshold : " << beam_threshold << std::endl;
        
        
        //init
        bool isroot = chart->get_size() == 1;
        for(unsigned i = 0; i < chart->get_size(); ++i) {
            for (unsigned j = i; j < chart->get_size(); ++j) {
                
                Cell& cell = chart->access(i,j);
                
                if(!cell.is_empty()) {
                    add_unary_init(cell,isroot);
                    cell.adjust_inside_probability();
                    
                    // prevent short sentences from being skipped ...
                    if(chart->get_size() >= min_length_beam)
                        cell.beam(priors, beam_threshold);
                    
                    // if(cell.is_closed())
                    //   std::cout << "(" << i << "," <<j << ") is closed" << std::endl;
                }
                
            }
        }
        
        //actual cky is here
        process_internal_rules(beam_threshold);
        
        if(ntries == 0)
            break;
        
        --ntries;
        beam_threshold /= 10;
    }
    while (stubbornness >=0 &&
           beam_threshold > 0 &&
           (chart->get_root().is_closed() || !chart->get_root().exists_edge(start_symbol)));
    
}

template <typename P>
inline
void ParserCKYAll_Impl<P>::get_candidates(Cell& left_cell,
                                          Cell& right_cell,
                                          Cell& result_cell) const
{
    static std::vector<vector_rhs0>::const_iterator brules_begin(brules->_begin);
    static std::vector<vector_rhs0>::const_iterator brules_end(brules->_end);
    
    //iterating through all the rules P -> L R, indexed by L
    for(std::vector<vector_rhs0>::const_iterator same_rhs0_itr(brules_begin);
        same_rhs0_itr != brules_end; ++same_rhs0_itr) {
        
        // is L present in the left_cell ?
        if(left_cell.exists_edge(same_rhs0_itr->rhs0)) {
            
            double LR1 = left_cell[same_rhs0_itr->rhs0]->get_annotations().inside_probabilities.array[0];
            //iterating through all the rules P -> L R, indexed by R, L fixed
            for (std::vector<vector_rhs1>::const_iterator same_rhs1_itr(same_rhs0_itr->_begin);
                 same_rhs1_itr != same_rhs0_itr->_end; ++same_rhs1_itr) {
                
                // is R present in the left_cell ?
                if(right_cell.exists_edge(same_rhs1_itr->rhs1)) {
                    
                    
                    double LR = LR1 * right_cell[same_rhs1_itr->rhs1]->get_annotations().inside_probabilities.array[0];
                    
                    //iterating through all the rules P -> L R, indexed by P, R and L fixed
                    std::vector< const BRuleC2f* >::const_iterator bitr(same_rhs1_itr->_begin);
                    for(; bitr != same_rhs1_itr->_end; ++bitr) {
                        result_cell.process_candidate(&left_cell,&right_cell,*bitr, LR);
                    }
                }
            }
        }
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::process_internal_rules(double beam_threshold) const
{
#ifdef USE_THREADS
    task_scheduler_init init(num_cell_threads);
#endif
    unsigned sent_size=chart->get_size();
    for (unsigned span = 2; span <= sent_size; ++span) {
        bool isroot =  span == sent_size;
        unsigned end_of_begin = sent_size-span;
        
#ifdef USE_THREADS
        parallel_for(blocked_range<unsigned>(0, end_of_begin),
                     ProcessCellBody<P>(*this, beam_threshold, isroot, span));
        
        if(span == sent_size)
        {
            unsigned begin = 0, end = span-1;
            Cell& result_cell = chart->access(begin,end);
            if(!result_cell.is_closed()) {
                process_cell(result_cell, beam_threshold, begin, end, isroot);
            }
        }
#else
        for (unsigned begin = 0; begin <= end_of_begin; ++begin) {
            unsigned end = begin + span -1;
            Cell& result_cell = chart->access(begin,end);
            if(!result_cell.is_closed()) {                
                process_cell(result_cell, beam_threshold, begin, end, isroot);
            }
        }
#endif
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::process_cell(Cell& cell, double beam_threshold, unsigned begin, unsigned end, bool isroot) const
{
    // look for all possible new edges
    
    //application of binary rules
    for (unsigned m = begin; m < end; ++m) {
        // m is the mid-point
        Cell& left_cell = chart->access(begin,m);
        if(!left_cell.is_closed()) {
            Cell& right_cell = chart->access(m+1,end);
            if( !right_cell.is_closed())
                get_candidates(left_cell,right_cell,cell);
        }
    }
    //	std::cout << result_cell << std::endl;
    
    //unary rules
    add_unary_internal(cell, isroot);
    cell.adjust_inside_probability();
    
    // pruning
    if(chart->get_size() >= min_length_beam)
        cell.beam(priors, beam_threshold);
    
    // if(cell.is_closed())
    //   std::cout << "(" << begin << "," << end << ") is closed" << std::endl;
}


template <typename P>
inline
void ParserCKYAll_Impl<P>::add_unary_init(Cell& cell, bool isroot) const
{
    //for each unary rule set in the grammar [sets made up of all unary rules with a particular rhs]
    static std::vector<short>::const_iterator unary_rhs_itr_begin = unary_rhs_from_pos.begin();
    static std::vector<short>::const_iterator unary_rhs_itr_end = unary_rhs_from_pos.end();
    
    for(std::vector<short>::const_iterator unary_rhs_itr(unary_rhs_itr_begin); unary_rhs_itr != unary_rhs_itr_end; ++unary_rhs_itr) {
        
        if (cell.exists_edge(*unary_rhs_itr))
            process_unary(cell,*unary_rhs_itr, isroot);
    }
}

template <typename P>
inline
void ParserCKYAll_Impl<P>::add_unary_internal(Cell& cell, bool isroot) const
{
    
    //for each unary rule set in the grammar [sets made up of all unary rules with a particular rhs being a lhs of a binary rule]
    std::vector<short>::const_iterator unary_rhs_itr_end = unary_rhs_from_binary.end();
    for(std::vector<short>::const_iterator unary_rhs_itr = unary_rhs_from_binary.begin();unary_rhs_itr!=unary_rhs_itr_end;++unary_rhs_itr) {
        
        if (cell.exists_edge(*unary_rhs_itr))
            process_unary(cell,*unary_rhs_itr,isroot);
    }
}




template <typename C>
struct processunary
{
    C& cell;
    double L_inside;
    processunary(C& c, double L) : cell(c), L_inside(L) {};
    void operator()(const URuleC2f* r) const
    {
        cell.process_candidate(r,L_inside);
    }
};


template <typename C>
struct processunary_wotop
{
    int start_symbol;
    C& cell;
    double L_inside;
    processunary_wotop(int ss, C& c, double L) : start_symbol(ss), cell(c), L_inside(L) {};
    void operator()(const URuleC2f* r) const
    {
        if(r->get_lhs() != start_symbol)
            cell.process_candidate(r,L_inside);
            }
};


template <typename P>
void ParserCKYAll_Impl<P>::process_unary(Cell& cell, int lhs, bool isroot) const
{
    static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    const std::vector<const URuleC2f*>& rules = unary_rhs_2_rules[lhs];
    
    double L_inside = cell.at(lhs).get_annotations().inside_probabilities.array[0];
    
    if(isroot)
        std::for_each(rules.begin(),rules.end(),processunary<Cell>(cell, L_inside));
    else
        std::for_each(rules.begin(),rules.end(),processunary_wotop<Cell>(start_symbol,cell, L_inside));
}

template <typename P>
void ParserCKYAll_Impl<P>::compute_outside_probabilities() const
{
    unsigned sent_size=chart->get_size();
    for (unsigned span = sent_size; span > 0; --span) {
        unsigned end_of_begin=sent_size-span;
        
        for (unsigned begin=0; begin < end_of_begin; ++begin) {
            unsigned end = begin + span ;
            //std::cout << '(' << begin << ',' << end << ')' << std::endl;
            
            Cell& cell = chart->access(begin,end);
            if(!cell.is_closed())
                cell.compute_outside_probabilities();
        }
    }
    for(unsigned i = 0; i < sent_size; ++i) {
        //    std::cout << i << std::endl;
        // assert(!chart->access(i,i).is_closed());
        if(!chart->access(i,i).is_closed())
            chart->access(i,i).compute_outside_probabilities();
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::compute_inside_probabilities() const
{    
    unsigned sent_size=chart->get_size();
    
    for (unsigned span = 0; span < sent_size; ++span) {
        unsigned end_of_begin=sent_size-span;                     
        for (unsigned begin=0; begin < end_of_begin; ++begin) {
            unsigned end = begin + span ;
            
            Cell& cell = chart->access(begin,end);
            if(!cell.is_closed()) {                               
                cell.compute_inside_probabilities();                
            }
            
        }        
    }
}


template <typename P>
double ParserCKYAll_Impl<P>::get_sentence_probability() const
{
    static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    
    if(chart->get_root().exists_edge(start_symbol))
        return chart->get_root()[start_symbol]->get_annotations().get_inside(0);
    else
        return LorgConstants::NullProba;
}

// relative beam
template <typename P>
void ParserCKYAll_Impl<P>::beam_chart_io_relative() const
{
    static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    
    chart->get_root()[start_symbol]->get_annotations().reset_outside_probabilities(1.0);
    compute_outside_probabilities(chart);
    
    unsigned sent_size=chart->get_size();
    for (unsigned span = 1; span <= sent_size; ++span) {
        unsigned end_of_begin=sent_size-span;
        for (unsigned begin=0; begin <= end_of_begin; ++begin) {
            unsigned end = begin + span -1;
            
            Cell& cell = chart->access(begin,end);
            
            if(!cell.is_closed()) {
                cell.beam(io_beam_thresholds[0]);
            }
        }
    }
}

//absolute beam
template <typename P>
void ParserCKYAll_Impl<P>::beam_chart(double log_sent_prob, double log_threshold, bool huang) const
{
    static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    
    chart->get_root()[start_symbol]->get_annotations().reset_outside_probabilities(1.0);
    compute_outside_probabilities();
    
    unsigned sent_size=chart->get_size();
    for (unsigned span = 1; span <= sent_size; ++span) {
        unsigned end_of_begin=sent_size-span;
                    
        for (unsigned begin=0; begin <= end_of_begin; ++begin) {
            unsigned end = begin + span -1;
            
            Cell& cell = chart->access(begin,end);
            
            if(!cell.is_closed()) {               
                cell.clean_binary_daughters();
                cell.beam(log_threshold, log_sent_prob);
                cell.clean();
                
                if(!cell.is_closed() && huang) {
                    cell.clean_binary_daughters();
                    cell.beam_huang(std::log(0.0001), log_sent_prob);
                    cell.clean();
                }                
            }
        }        
    }
}




/////////////////////////////
//// mapping c2f ////////////
/////////////////////////////
// should be moved somewhere else


//  calculates c2f mapping
// returns rules that don't belong to the mapping
template <typename Key, typename MyRule>
std::vector<MyRule*> calculate_mapping(typename rulevect2mapvect<Key,MyRule>::map_type& map, unsigned size)
{
    std::vector<MyRule*> r2remove;
    for(typename rulevect2mapvect<Key,MyRule>::map_type::const_iterator i(map.begin()); i != map.end(); ++i)
    {
        if(i->second.size() != size
           || (std::find_if(i->second.begin(), i->second.end(), std::mem_fun(&MyRule::is_empty)) != i->second.end())
           )
            r2remove.push_back(i->second[0]);
        else
            for(unsigned g = 0 ; g < size - 1; ++g)
                i->second[g]->add_finer(i->second[g+1]);
    }
    return r2remove;
}



// calls previous function
// and removes useless rules
template <typename Key, typename MyRule>
void process_internal(typename rulevect2mapvect<Key,MyRule>::map_type& map, std::vector<MyRule>& grammar_coarse_rules, unsigned size)
{
    std::vector<MyRule*> r2remove = calculate_mapping<Key,MyRule>(map,size);
    typename std::vector<MyRule>::iterator end = grammar_coarse_rules.end();
    for(typename std::vector<MyRule*>::iterator i(r2remove.begin()); i != r2remove.end(); ++i) {
        // ++r
        // if(r % 1000 == 0)
        //   std::cout << "removing " << r << " of " << coarse_rules_wo_finers.size() << std::endl;
        end = std::remove(grammar_coarse_rules.begin(), end,**i);
    }
    grammar_coarse_rules.erase(end,grammar_coarse_rules.end());
}


#define MAP boost::unordered_map
//#define MAP std::map

template <typename P>
void ParserCKYAll_Impl<P>::create_coarse_to_fine_mapping(std::vector<AGrammar*>& cgs)
{
    //  std::clog << "before mapping" << std::endl;
    
    typedef std::pair<int, std::pair<int,int> > bkey;
    typedef std::pair<int,int> ukey;
    
    MAP< bkey, std::vector<BRuleC2f*> > bmap;
    MAP< ukey, std::vector<URuleC2f*> > umap;
    MAP< ukey, std::vector<LexicalRuleC2f*> > lmap;
    
    rulevect2mapvect<bkey,BRuleC2f> bc2f(bmap);
    rulevect2mapvect<ukey, URuleC2f> uc2f(umap);
    rulevect2mapvect<ukey, LexicalRuleC2f> lc2f(lmap);
    
    for(std::vector<AGrammar*>::const_iterator g(cgs.begin()); g != cgs.end(); ++g) {
        bc2f.add_all((*g)->binary_rules);
        uc2f.add_all((*g)->unary_rules);
        lc2f.add_all((*g)->lexical_rules);
    }
    
    process_internal<bkey,BRuleC2f>(bmap, cgs[0]->binary_rules, cgs.size());
    process_internal<ukey,URuleC2f>(umap, cgs[0]->unary_rules, cgs.size());
    
    std::vector<LexicalRuleC2f*> l2remove = calculate_mapping<ukey,LexicalRuleC2f>(lmap, cgs.size());
    for(std::vector<LexicalRuleC2f*>::iterator i(l2remove.begin()); i != l2remove.end(); ++i) {
        remove_lex_rule(*i);
    }
    
    //  std::clog << "after mapping" << std::endl;
    
}

////////////////////////////////
/////////////// C2f ///////////
///////////////////////////////
template <typename P>
void ParserCKYAll_Impl<P>::beam_c2f(int start_symbol)
{
    if(!chart->get_root().is_closed() && chart->get_root().exists_edge(start_symbol)) {
        beam_c2f(grammars, annot_descendants);
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::beam_c2f(const std::vector<AGrammar*>& current_grammars,
                                    const annot_descendants_type& /*current_annot_descendants*/) const
{
    static int top_idx = SymbolTable::instance_nt().get_label_id(LorgConstants::tree_root_name);
    
    //  std::cout << "beam_c2f" << std::endl;
    
    for(unsigned i = 0; i < current_grammars.size() - 1; ++i) {
        
        double beam_threshold = io_beam_thresholds[i + 1];
        
        // std::cout << std::log(get_sentence_probability()) << std::endl;
        //     std::cout << "beaming with grammar: " << i << std::endl;
        
        
        // FIX: This test messes with product grammar parsing
        // TODO: Do this test only with the first grammar
        //    if(i != 0) {// inside_probs already computed when bulding the chart
        //      std::cout << "before inside" << std::endl;
        compute_inside_probabilities();
        //    }
        
        
        
        // if(chart->get_root().is_closed())
        //   std::cout << "root cell is closed" << std::endl;
        // else if(!chart->get_root().exists_edge(top_idx))
        //   std::cout << "top is not in root cell" << std::endl;
        
        if(chart->get_root().is_closed() || !chart->get_root().exists_edge(top_idx)) {
            //      std::cerr << "grammar " << i << " spoiled the fun :(" << std::endl;
            break;
        }
        //    std::cout << "after inside" << std::endl;
        //    std::cout << "before beam" << std::endl;
        double sp = std::log(get_sentence_probability());
        //    std::cout << "sentence probability: " << sp << std::endl;
        
        // huang beam seems to affect only the first pass
        //bool huang = i == 0;
        bool huang = false;
        if(chart->get_size() >= min_length_beam) // TODO if sentence is short skip everything but correct resizing
            beam_chart(sp, beam_threshold, huang);
        //    std::cout << "after beam" << std::endl;
        
        // PCKYAllCell& root = chart->get_root();
        // if (!root.exists_edge(SymbolTable::instance_nt()->get_label_id(LorgConstants::tree_root_name)))
        //   std::cout << "no axiom at root" << std::endl;
        
        
        //    std::cout << "before change" << std::endl;
        
        // TODO this function should take current_annot_descendants as an argument
        // instead annot_descendants is changed in ParserCKYAllMaxVarMultiple::extract_solution
        // which is a bit .. hackish
        change_rules_resize(i, current_grammars);
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::change_rules_resize(unsigned step,
                                               const std::vector<AGrammar*>& current_grammars) const
{
    const AnnotatedLabelsInfo& next_annotations = current_grammars[step+1]->get_annotations_info();
    const std::vector<std::vector<std::vector<unsigned> > >& annot_descendants_current =  annot_descendants[step];
    
    unsigned sent_size=chart->get_size();
    for (unsigned span = 1; span <= sent_size; ++span) {
        unsigned end_of_begin=sent_size-span;
        for (unsigned begin=0; begin <= end_of_begin; ++begin) {
            unsigned end = begin + span -1;
            
            Cell& cell = chart->access(begin,end);
            
            if(!cell.is_closed()) {
                cell.change_rules_resize(next_annotations, annot_descendants_current);
            }
        }
    }
}

template <typename P>
void ParserCKYAll_Impl<P>::get_parses(int start_symbol, unsigned kbest,
                                      bool always_output_forms, bool output_annotations,
                                      std::vector<std::pair<PtbPsTree *,double> >& best_trees)
{
    for(unsigned i = 0; i < kbest; ++i) {
        // get results
        if(!chart->has_solution(start_symbol, i)) {
            break;
        }
        PtbPsTree * t = chart->get_best_tree(start_symbol, i, always_output_forms, output_annotations);
        best_trees.push_back(std::make_pair(t, chart->get_score(start_symbol, i)));
    }
    
}

template<class Probability>
inline
typename ParserCKYAll_Impl<Probability>::AGrammar& ParserCKYAll_Impl<Probability>::get_grammar(unsigned idx)
{
    return *(grammars[idx]);
}

template<class Probability>
inline
const typename ParserCKYAll_Impl<Probability>::AGrammar& ParserCKYAll_Impl<Probability>::get_grammar(unsigned idx) const
{
    return *(grammars[idx]);
}


template<class Probability>
void ParserCKYAll_Impl<Probability>::compute_inside_outside_probabilities() const
{
    compute_inside_probabilities();
    static int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name);
    chart->get_root()[start_symbol]->get_annotations().reset_outside_probabilities(1.0);
    compute_outside_probabilities();
}






#endif /*PARSERCKYALL_H*/
