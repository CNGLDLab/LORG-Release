// -*- mode: c++ -*-
#ifndef _TRAININGNODE_H_
#define _TRAININGNODE_H_

#include "AnnotationInfo.h"
#include "rules/BRuleTraining.h"
#include "rules/URuleTraining.h"
#include "rules/LexicalRuleTraining.h"

typedef boost::unordered_map< std::pair<int, int >, double> DeltaMap;


class TrainingNode;

struct brule_occurrence
{
  const TrainingNode* left;
  const TrainingNode* right;
  const TrainingNode* up;
  
  const TrainingNode* root;


  brule_occurrence(const TrainingNode* l, const TrainingNode* r, const  TrainingNode* u, 
		   const TrainingNode* ro) : left(l), right(r), up(u), root(ro) 
  {}

};

struct urule_occurrence
{
  const TrainingNode* left;
  const TrainingNode* up;
  
  const TrainingNode* root;

  urule_occurrence(const TrainingNode* l, const  TrainingNode* u, 
		   const TrainingNode* ro) : left(l), up(u), root(ro) 
  {}
  
};


struct lrule_occurrence
{
  const TrainingNode* up;

  const TrainingNode* root;
  
  lrule_occurrence(const TrainingNode*u, const TrainingNode* r) : up(u), root(r)
  {}
};





class LexicalTrainingNode;

class TrainingNode
{
  friend class BinaryTrainingTree;
public:
  TrainingNode() : lhs(), annotations() {};
  virtual ~TrainingNode() {};

  virtual AnnotatedRule * get_rule() = 0;
  virtual const AnnotatedRule * get_rule() const = 0;

  int get_lhs() const {return lhs;}
  const AnnotationInfo& get_annotations() const {return annotations;}
  AnnotationInfo& get_annotations() {return annotations;}

  virtual void get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer) = 0;
  virtual void resize_annotations(unsigned size);

  virtual void resize_annotations(const AnnotatedLabelsInfo& lookup);
  virtual void reset_inside_probabilities(double value);
  
  virtual void reset_outside_probabilities(double value);

  virtual void reset_probabilities(double value);

  virtual void compute_inside_probability() = 0;
  virtual void compute_outside_probability() = 0;
  virtual void update_rule_frequencies(const scaled_array& root_insides) = 0;
  virtual void update_delta_scores(DeltaMap &  delta_scores_map,
				   int split_number, 
				   const ProportionsMap & proportions, 
				   const scaled_array& root_insides) const;

  void calculate_delta_score(DeltaMap &  delta_scores_map, int split_size, 
			     const ProportionsMap & proportions,
			     const scaled_array& root_inside) const;

  virtual bool contains_empty_rules() const = 0;
  
  virtual void add_occurrences(boost::unordered_map<BRuleTraining*,std::vector<brule_occurrence> >& bmap, 
			       boost::unordered_map<URuleTraining*,std::vector<urule_occurrence> >& umap,
                               boost::unordered_map<LexicalRuleTraining*,std::vector<lrule_occurrence> >& lmap,
			       const TrainingNode* root) const = 0;

protected:
  int lhs;
  AnnotationInfo annotations; 
};

class LexicalTrainingNode : public TrainingNode
{
  friend class BinaryTrainingTree;
public:
  LexicalTrainingNode() : rule(NULL) {};
  virtual ~LexicalTrainingNode() {};
  
  virtual LexicalRuleTraining *   get_rule() {return rule;}
  virtual const LexicalRuleTraining * get_rule() const {return rule;}

  virtual void resize_annotations(unsigned size);
  virtual void resize_annotations(const AnnotatedLabelsInfo& lookup);
  virtual void reset_inside_probabilities(double value);
  virtual void reset_outside_probabilities(double value);
  virtual void reset_probabilities(double value);
  virtual void compute_inside_probability();
  virtual void compute_outside_probability();
  virtual void update_rule_frequencies(const scaled_array& /*root_insides*/);
  virtual bool contains_empty_rules() const;
  virtual void get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer);

  virtual void add_occurrences(boost::unordered_map<BRuleTraining*, std::vector<brule_occurrence> >& bmap,
                               boost::unordered_map<URuleTraining*, std::vector<urule_occurrence> >& umap,
                               boost::unordered_map<LexicalRuleTraining*, std::vector<lrule_occurrence> >& lmap,
                               const TrainingNode* root)  const ;

protected: 
  LexicalRuleTraining * rule;
  //  double word_annotation_inside; // words are not annotated // not needed anymore
  //  double word_annotation_outside; // words are not annotated// not needed anymore
private:
  LexicalTrainingNode(const LexicalTrainingNode&);
  LexicalTrainingNode& operator= (const LexicalTrainingNode& ); //not defined, not assignable
};


class BinaryTrainingNode : public TrainingNode
{
  friend class BinaryTrainingTree;
public:
  BinaryTrainingNode() : left(NULL), right(NULL), rule(NULL) {};
  virtual ~BinaryTrainingNode() {};
  
  virtual BRuleTraining * get_rule() {return rule;}
  virtual const BRuleTraining * get_rule() const {return rule;}

  virtual void resize_annotations(unsigned size);
  virtual void resize_annotations(const AnnotatedLabelsInfo& lookup);
  virtual void reset_probabilities(double value);
  virtual void reset_inside_probabilities(double value);
  virtual void reset_outside_probabilities(double value);
  virtual void compute_inside_probability();
  virtual void compute_outside_probability();
  virtual void update_rule_frequencies(const scaled_array& root_insides);
  virtual void update_delta_scores(DeltaMap &  delta_scores_map,
				   int split_number,
				   const ProportionsMap & proportions,
				   const scaled_array& root_inside) const ;
  virtual bool contains_empty_rules() const;
  virtual void get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer) ;

  virtual void add_occurrences(boost::unordered_map<BRuleTraining*, std::vector<brule_occurrence> >& bmap,
                               boost::unordered_map<URuleTraining*, std::vector<urule_occurrence> >& umap,
                               boost::unordered_map<LexicalRuleTraining*, std::vector<lrule_occurrence> >& lmap,
                               const TrainingNode* root)  const;


protected:
  TrainingNode * left;
  TrainingNode * right;
  BRuleTraining * rule;
private:
  BinaryTrainingNode(const BinaryTrainingNode&);
  BinaryTrainingNode& operator= (const BinaryTrainingNode& ); //not defined, not assignable
};


class UnaryTrainingNode : public TrainingNode
{
  friend class BinaryTrainingTree;
public:
  UnaryTrainingNode() : left(NULL), rule(NULL) {};
  virtual ~UnaryTrainingNode() {};

  virtual URuleTraining * get_rule() {return rule;}
  virtual const URuleTraining * get_rule() const {return rule;}
  
  virtual void resize_annotations(unsigned size);
  virtual void resize_annotations(const AnnotatedLabelsInfo& lookup);
  virtual void reset_probabilities(double value);
  virtual void reset_inside_probabilities(double value);
  virtual void reset_outside_probabilities(double value);
  virtual void compute_inside_probability();
  virtual void compute_outside_probability();
  virtual void update_rule_frequencies(const scaled_array& root_insides);
  virtual void update_delta_scores(DeltaMap &  delta_scores_map,
				   int split_number,
				   const ProportionsMap & proportions,
				   const scaled_array& root_inside) const ;
  virtual bool contains_empty_rules() const;
  virtual void get_lexical_nodes(std::vector<LexicalTrainingNode*>& buffer);
  
  virtual void add_occurrences(boost::unordered_map<BRuleTraining*, std::vector<brule_occurrence> >& bmap,
                               boost::unordered_map<URuleTraining*, std::vector<urule_occurrence> >& umap,
                               boost::unordered_map<LexicalRuleTraining*, std::vector<lrule_occurrence> >& lmap,
                               const TrainingNode* root)  const;


protected:
  TrainingNode * left;
  URuleTraining * rule;
private:
  UnaryTrainingNode(const UnaryTrainingNode&);
  UnaryTrainingNode& operator= (const UnaryTrainingNode& ); //not defined, not assignable
};


#endif /* _TRAININGNODE_H_ */
