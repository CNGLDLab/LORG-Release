// -*- mode: c++ -*-
#ifndef TAGGUESSER_H_
#define TAGGUESSER_H_

#include "rules/Rule.h"
#include "classifier/Classifier.h"
#include "classifier/ProbModel.h"
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <boost/bimap.hpp>
#include <cstring>




/**
  \enum ClassifierType
  \brief Holds the different type of classifiers, only one for the time being
*/
typedef enum{ 
    NaiveBayes
}  ClassifierType;

typedef boost::bimap<std::string,unsigned int> fbimap;
typedef Classifier<unsigned int,unsigned int>::TrainingInstance TrainingInstance;
typedef Classifier<unsigned int,unsigned int>::Datum Datum;

/**
  \class TagGuesser
  \brief A guesser to guess class labels according to a classifier
*/
class TagGuesser
{
public:
  /**
    \brief Constructor, takes a classifier type and a smoothing method
    \param cctype the type of the classifier to be used
    \param stype the type of the smoothing method
  */
  TagGuesser( ClassifierType cctype = NaiveBayes, SmoothingType stype = Lidstone );
  /**
    \brief Constructor, creates a new guesser from a given file
    \param filename a file containing the classification model
    \param cctype the type of the classifier to be used
    \param stype the type of the smoothing method
  */
  TagGuesser( const std::string& filename, ClassifierType cctype = NaiveBayes, SmoothingType stype = Lidstone );
  /**
    \brief Copy constructor
  */
  TagGuesser( const TagGuesser& rhs );
  /**
    \brief Destructor
  */
  ~TagGuesser();

  /**
    \brief Assignment operator
  */
  TagGuesser& operator=( const TagGuesser& rhs );
  
  /**
    \brief Checks whether the model is trained or not
    \return true if the model is trained, false otherwise
  */
  bool trained() const;

//   /**
//     \brief Trains the underlying classifier on a given treebank grammar
//     \param tbg a treebank grammar
//   */
//   void train(const TreebankGrammar& tbg);
  
  void train(const std::vector<Rule>& lexical_rules);

  /**
    \brief Trains the underlying classifier on a given grammar file
    \param grammar_file
  */
  void train(const char* grammar_file);

  /**
    \brief Returns the class labels according to the classifier for a given word that have a probability higher than threshold
    \param word the word to classify
    \param threshold a probability threshold
    \return a vector of class labels
  */
  std::vector<std::string> guess( const std::string& word, double threshold ) const;
  /**
    \brief Returns the k best class labels according to the classifier for a given word
    \param word the word to classify
    \param k the number of class labels desiredconst
    \return a vector of class labels
  */
  std::vector<std::string> guess_nbest( const std::string& word, unsigned int k ) const;
  /**
    \brief Returns the best class label according to the classifier
    \param word the word to classify
    \return a class label
  */
  std::string guess( const std::string& word ) const;
  /**
    \brief Save the current state of the TagGuesser
    \param filename the file to save to
    \note This function will create two files, filename and filename + ".featmap"
  */
  void save_to_file( const std::string& filename ) const;

  /**
    \brief Trains the classifier from a set of rules
    \param lexical_rules a list of lexical rules
  */
  void train_classifier( const std::vector<Rule>& lexical_rules );

private:  
  /**
    \brief Constructs a feature vector from a given word form and stores new features
    \param word_form the word form
    \return a vector of Datum containing the features of the word form
  */
  std::vector<Datum> construct_feature_vector( const std::string& word_form);
  /**
    \brief Constructs a feature vector from a given word form. Ignores unknown features
    \param word_form the word form
    \return a vector of Datum containing the features of the word form
  */
  std::vector<Datum> construct_feature_vector( const std::string& word_form) const;
  /**
    \brief Reads a feature integer mapping and stores it in the feature map
    \param filename the name of the file
  */
  void fill_feature_map( const std::string& filename );
  /**
    \brief Saves the feature mapping to a given file
    \param filename the name of the file
  */
  void save_feature_map( const std::string& filename ) const;
  /**
    \brief Counts a number of features from a given word form
    \param word_form the word form to count features on
    \return a vector of feature-frequency-pairs
  */
  std::vector<std::pair<std::string,unsigned int> > count_features( const std::string& word_form ) const;

  ClassifierType ctype; ///< the type of the classifier
  fbimap feature_map; ///< mapping from features to integers
  fbimap label_map; ///< mapping from labels to integers
  unsigned int feature_cnt; ///< the number of recorded features
  unsigned int label_cnt; ///< the number of recorded labels
  std::auto_ptr<Classifier<unsigned int,unsigned int> > classifier; ///< the classifier
};

// inline functions
inline
bool TagGuesser::trained() const { return classifier->trained(); }

inline
void TagGuesser::train(const std::vector<Rule>& lexical_rules)
{
  train_classifier(lexical_rules);
}

inline
std::string TagGuesser::guess( const std::string& word ) const { return guess_nbest(word,1)[0]; }

inline
void TagGuesser::save_to_file( const std::string& filename ) const
{ 
  save_feature_map(filename + ".featmap");
  classifier->save_probability_model(filename.c_str());
}

#endif // TAGGUESSER_H_
