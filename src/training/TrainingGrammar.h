// -*- mode: c++ -*-
#ifndef TRAININGGRAMMAR_H
#define TRAININGGRAMMAR_H

#include "rules/BRuleTraining.h"
#include "rules/URuleTraining.h"
#include "rules/LexicalRuleTraining.h"
#include "grammars/Grammar.h"

#include <boost/unordered_map.hpp>
#include <vector>

#include "lexicon/Lexicon.h"
#include "Treebank.h"

typedef boost::unordered_map< std::pair<int, int >, double> DeltaMap;
typedef std::map<int,std::vector<int> > Merge_map;
typedef boost::unordered_map<int,double> unannotated_node_count_map;

class TrainingGrammar: public Grammar<BRuleTraining,URuleTraining,LexicalRuleTraining>, public AnnotatedContents
{

    private:
        unannotated_node_count_map unannotated_node_priors;

        Lexicon* lexicon;

        std::vector<Tree<unsigned> > annot_histories;


        //Debug methods
        void output_counts(std::map<std::pair<int, std::pair<int,int> >, int> & binary_counts,
                std::map<std::pair<int,int> , int> & unary_counts,
                std::map< int, int> & LHS_counts);

        //private and not implemented -> forbidden
        // if you want to implement that, have a look at operator= first
        TrainingGrammar(const TrainingGrammar&);

    public:

        enum SmoothType {LinearSmooth, WeightedSmooth, GenerationSmooth}; // GenerationSmooth is not working

        //reads grammar from treebank
        TrainingGrammar(Treebank<PtbPsTree> & tb, Lexicon * lex);
        TrainingGrammar() : lexicon(NULL) {};
        ~TrainingGrammar() {delete lexicon;}
        void update_lexical_counts(std::vector<BinaryTrainingTree> & trees, bool last_iteration, std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
                int nbthreads);
        void maximise_lexical_rules();

        void split_all_rules(unsigned n, unsigned randomness = 1 );
        void smooth_all_rules(double alpha_grammar, double alpha_lexicon, SmoothType type);

        void remove_unlikely_annotations_all_rules(const double& threshold);
        /**
          \brief for each binary, unary and lexical rule in the grammar merges the rule (with a call to rule.merge_rule()
          \param  annotation_sets_to_merge data structure created in em trainer which contains a map with key=unannotatedLabel and value= vector of
          \ start indices.   map: label -> [vector of start indices of annotation sets that will be merged]
          \the start index of an annotation set is the first annotataion of the annotation set.
          \eg an annotation with start index 3 and split size 4, will contain annotations: {3,4,5,6}
          \param split_size a label is split into split_size new annotated labels
          \param proportions data structure containing the proportions calculation (calculated originally in EMTrainer) for each annotated label
          \param full_sets_to_merge_lookup map: label -> [set of all indices of annotation sets that will be merged]
          */
        void merge_rules(const AnnotatedLabelsInfo& old_ali,
                const Merge_map &, int,
                const ProportionsMap&,
                const std::map<int,std::set<int> > & );

        const std::vector<BRuleTraining>& get_binary_rules() const;
        const std::vector<URuleTraining>& get_unary_rules()  const;
        const std::vector<LexicalRuleTraining>& get_lexical_rules() const;

        std::vector<BRuleTraining>& get_binary_rules() ;
        std::vector<URuleTraining>& get_unary_rules() ;
        std::vector<LexicalRuleTraining>& get_lexical_rules() ;

        /**
          \brief Output operator
          \param os the ostream to write on
          \param gram the grammar object to write
          \return the used ostream
          */
        friend std::ostream& operator<<(std::ostream& os, const TrainingGrammar& gram);

        /**
          \brief calculates the new number of annotations a label should have, when the merge is completed.
          \ The label_annotations class member data structure is updated with this information
          \param label the non-terminal label
          \param num_merges the number of merges that will be made on this label's annotation sets/ ie. the number of annotation sets that will be merged for this label
          \param split_size the size of the original split
          */
        void reset_merged_num_annotations(int label,int num_merges,int split_size);

        /**
          \brief calculates the new number of annotations a label should have, when the split is completed.
          \param label the label that is being split
          \param split_size the size of the split (e.g. split size = 4 then each annotation a label has, will be split into four)
          */
        void reset_split_num_annotations(int label,int split_size);

        /**
          \brief  resets the number_of_annotations per label map in the grammar; calls reset_merged_num_annotations for each label that will be merged
          \param sets_to_merge_map, data structure containing the set of labels and annotation sets that are to be merged
          \param split_size size of the split
          */
        void update_merge_annotations(const Merge_map& sets_to_merge_map, int split_size);

        /**
          \brief  resets the number_of_annotations per label map in the grammar; calls reset_split_num_annotations for each label that will be split
          \param split_size size of the split
          */
        void update_split_annotations(int split_size);

        void reset_lexical_counts();
        void resize_lexical_datastructs();

        void output_priors(std::ostream* out);

        void uncompact_all_rules();
        void compact_all_rules();

        TrainingGrammar& operator=(const TrainingGrammar& other);

        void add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& NodeCountMap);


        void reset_unannotated_priors();


        void lexical_smoothing();

        void create_additional_rules();

        //debugging..
        void output_rules();


        const std::vector< Tree<unsigned> >& get_annot_histories() const {return annot_histories;}

        Lexicon* get_lexicon() {return lexicon;}

        template <class T>
            void perform_action_all_internal_rules(T& action);

        template <class T>
            void perform_action_all_internal_rules(T& action) const;

};

inline void TrainingGrammar::add_lexicon_annotated_node_counts(std::vector< std::vector<double> >& NodeCountMap)
{
    assert(lexicon != NULL);
    lexicon->add_lexicon_annotated_node_counts(NodeCountMap);
}

inline const std::vector<BRuleTraining>& TrainingGrammar::get_binary_rules() const { return binary_rules;}

inline const std::vector<URuleTraining>& TrainingGrammar::get_unary_rules() const {return unary_rules;}

inline const std::vector<LexicalRuleTraining>& TrainingGrammar::get_lexical_rules() const
{
    assert(lexicon != NULL);
    return lexicon->get_lexical_rules();
}

inline void TrainingGrammar::update_lexical_counts(std::vector<BinaryTrainingTree> & trees, bool last_iteration,
        std::vector< std::pair<LexicalRuleTraining*, std::vector<lrule_occurrence> > >& lex_occurrences,
        int nbthreads)
{
    assert(lexicon != NULL);
    lexicon->update_annotated_counts_from_trees(trees, last_iteration, lex_occurrences, nbthreads);
}

    inline
void TrainingGrammar::reset_lexical_counts()
{
    assert(lexicon != NULL);
    lexicon->reset_counts();
}

    inline
void TrainingGrammar::resize_lexical_datastructs()
{
    assert(lexicon != NULL);
    lexicon->resize_structures(label_annotations);
}


    inline
void TrainingGrammar::maximise_lexical_rules()
{
    assert(lexicon != NULL);
    lexicon->maximisation();
}

    inline
void TrainingGrammar::lexical_smoothing()
{
    assert(lexicon != NULL);
    lexicon->lexical_smoothing();
}

    inline
void TrainingGrammar::create_additional_rules()
{
    assert(lexicon != NULL);
    lexicon->create_additional_rules();
}



inline std::vector<BRuleTraining>& TrainingGrammar::get_binary_rules() { return binary_rules;}

inline std::vector<URuleTraining>& TrainingGrammar::get_unary_rules() {return unary_rules;}

inline std::vector<LexicalRuleTraining>& TrainingGrammar::get_lexical_rules() {return lexicon->get_lexical_rules();}

template <class T> inline void TrainingGrammar::perform_action_all_internal_rules(T& action)
{        
    std::for_each(binary_rules.begin(),binary_rules.end(), action);
    std::for_each(unary_rules.begin(),unary_rules.end(), action);
}

template <class T> inline void TrainingGrammar::perform_action_all_internal_rules(T& action) const
{    
    std::for_each(binary_rules.begin(),binary_rules.end(), action);
    std::for_each(unary_rules.begin(),unary_rules.end(), action);
}


#endif // TRAININGGRAMMAR_H


