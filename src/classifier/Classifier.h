#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include <vector>
#include <utility>

/**
  \class NoTrainingDataAvailable
  \brief Exception class
*/
class NoTrainingDataAvailable{};

/**
  \class UnknownFeature
  \brief Exception class
*/
class UnknownFeature{};

/**
  \class Classifier
  \brief This class functions as an abstract interface from which classifiers can be derived.
  \param FEATURE The type of input data
  \param LABEL The type of class labels
*/
template<class LABEL, class FEATURE>
class Classifier
{
public:
  /**
    \class Datum
    \brief This class encapsulates one datum or feature-value pair.
  */
  class Datum
  {
  public:
    /**
      \brief Constructor, creates a new Datum from given values
      \param ffeature the feature name
      \param vvalue the feature value
    */
    Datum( const FEATURE& ffeature, double vvalue );
    FEATURE feature; ///< the feature name
    double value; ///< the feature value
  };

  /**
    \class TrainingInstance
    \brief This class represents a training instance, i.e. a vector of feature-value pairs and its associated class label.
  */
  class TrainingInstance
  {
  public:
    typedef typename std::vector<typename Classifier<LABEL,FEATURE>::Datum>::iterator iterator;
    typedef typename std::vector<typename Classifier<LABEL,FEATURE>::Datum>::const_iterator const_iterator;

    /**
      \brief Standard Constructor
    */
    TrainingInstance();
    /**
      \brief Constructor, that creates a new TrainingInstance from given data
      \param ffeatures a vector of Datum
      \param llabel a class label
    */
    TrainingInstance( const std::vector<Datum>& ffeatures, const LABEL& llabel );

    /**
      \brief returns the label
      \return the label of the TrainingInstance
    */
    LABEL get_label() const;
    /**
      \brief sets the class label
      \param llabel the new class label
    */
    void set_label( const LABEL& llabel );

    /**
      \brief returns the feature vector
      \return a vector of Datum
    */
    std::vector<Datum> get_features() const;
    /**
      \brief sets the feature data
      \param ffeatures the new feature vector
    */
    void set_features(const std::vector<Datum>& ffeatures);
    /**
      \brief adds a feature to the feature vector
      \param datum the new feature-value pair
    */
    void add_feature(const Datum& datum);

    /**
      \brief returns an iterator pointing to the beginning of the feature vector
      \return an iterator
    */
    iterator begin();
    /**
      \brief returns a constant iterator pointing to the beginning of the feature vector
      \return a constant iterator
      \see begin()
      \note This is an overloaded member function provided for convenience
    */
    const_iterator begin() const;

    /**
      \brief returns an iterator pointing to the end of the feature vector
      \return an iterator
    */
    iterator end() ;
    /**
      \brief returns a constant iterator pointing to the end of the feature vector
      \return a constant iterator
      \see end()
      \note This is an overloaded member function provided for convenience
    */
    const_iterator end() const ;

  private:
    std::vector<Datum> features;   ///< the feature vector
    LABEL label;                                ///< the class label
  };

public:
  /**
    \brief Standard Constructor
  */
  Classifier();
  /**
    \brief Destructor
  */
  virtual ~Classifier();

  /**
    \brief Checks, whether the classifier is trained already
    \return true, if trained, false elsewise
    \note This is an pure virtual function that derived classes have to override
  */
  virtual bool trained() const = 0;
  /**
    \brief returns a pointer to a copy of this
    \return a pointer to a dynamically allocated copy of this
    \note It is bad, but it is also the only way of doing this as far as i know
    \note This is an pure virtual function that derived classes have to override
  */
  virtual Classifier<LABEL,FEATURE>* copy() const = 0;
  /**
    \brief trains the classifier from a vector of training instances
    \param data a vector of TrainingInstance
    \note This is an pure virtual function that derived classes have to override
  */
  virtual void train( const std::vector<std::pair<TrainingInstance,unsigned int> >& data ) = 0;
  /**
    \brief classifies a given instance and returns all labels with a probability higher than a certain threshold
    \param instance a vector of Datum
    \param threshold a probability threshold
    \return a vector of class labels ranked by probability in descending order
    \note This is an pure virtual function that derived classes have to override
  */
  virtual std::vector<LABEL> classify( const std::vector<Datum>& instance, double threshold ) const = 0;
  /**
    \brief classifies a given instance and returns the k-best labels
    \param instance a vector of Datum
    \param k the size of the returned vector
    \return a vector of class labels ranked by probability in descending order
    \note This is an pure virtual function that derived classes have to override
  */
  virtual std::vector<LABEL> classify_nbest( const std::vector<Datum>& instance, unsigned int k ) const = 0;
  /**
    \brief classifies a given instance and returns the best label
    \param instance a vector of Datum
    \return the most probable class label
    \note This is an pure virtual function that derived classes have to override
    \note This is an overloaded function provided for convenience 
    \see classify
  */
  virtual LABEL classify( const std::vector<Datum>& instance ) const = 0;
  /**
    \brief Returns the estimated probability for a class label given a test instance
    \param label the label
    \param instance the test instance
    \return the probability of the class label
    \note This is an pure virtual function that derived classes have to override
  */
  virtual double probability( const LABEL& label, const std::vector<Datum>& instance ) const = 0;
  /**
    \brief Saves the underlying probability model to a specified file
    \param filename the filename
    \note This is an pure virtual function that derived classes have to override
  */
  virtual void save_probability_model( const char* filename ) const = 0;
};

//#include "Classifier.cpp"


template<class LABEL, class FEATURE>
Classifier<LABEL,FEATURE>::Datum::Datum( const FEATURE& ffeature, double vvalue )
: feature(ffeature), 
  value(vvalue) 
{
}

template<class LABEL, class FEATURE>
Classifier<LABEL,FEATURE>::TrainingInstance::TrainingInstance()
{
}

template<class LABEL, class FEATURE>
Classifier<LABEL,FEATURE>::TrainingInstance::TrainingInstance(const std::vector<Datum>& ffeatures, const LABEL& llabel )
: features(ffeatures), 
  label(llabel)
{
}

template<class LABEL, class FEATURE>
Classifier<LABEL,FEATURE>::Classifier()
{
}

template<class LABEL, class FEATURE>
Classifier<LABEL,FEATURE>::~Classifier()
{
}

template<class LABEL, class FEATURE>
inline
LABEL Classifier<LABEL,FEATURE>::TrainingInstance::get_label() const 
{ 
  return label; 
}

template<class LABEL, class FEATURE>
inline
void Classifier<LABEL,FEATURE>::TrainingInstance::set_label( const LABEL& llabel ) 
{ 
  label = llabel; 
}

template<class LABEL, class FEATURE>
inline 
std::vector< typename Classifier<LABEL,FEATURE>::Datum > 
Classifier<LABEL,FEATURE>::TrainingInstance::get_features() const 
{ 
  return features; 
}

template<class LABEL, class FEATURE>
inline
void Classifier<LABEL,FEATURE>::TrainingInstance::set_features(const std::vector<Datum>& ffeatures) 
{ 
  features = ffeatures; 
}

template<class LABEL, class FEATURE>
inline
void Classifier<LABEL,FEATURE>::TrainingInstance::add_feature(const Datum& datum) 
{ 
  features.push_back(datum); 
}

template<class LABEL, class FEATURE>
inline
typename Classifier<LABEL,FEATURE>::TrainingInstance::iterator 
Classifier<LABEL,FEATURE>::TrainingInstance::begin() 
{ 
  return features.begin(); 
}

template<class LABEL, class FEATURE>
inline
typename Classifier<LABEL,FEATURE>::TrainingInstance::const_iterator 
Classifier<LABEL,FEATURE>::TrainingInstance::begin() const 
{ 
  return features.begin(); 
}

template<class LABEL, class FEATURE>
inline
typename Classifier<LABEL,FEATURE>::TrainingInstance::iterator  
Classifier<LABEL,FEATURE>::TrainingInstance::end() 
{ 
  return features.end(); 
}

template<class LABEL, class FEATURE>
inline
typename Classifier<LABEL,FEATURE>::TrainingInstance::const_iterator 
Classifier<LABEL,FEATURE>::TrainingInstance::end() const 
{
  return features.end(); 
}



#endif // CLASSIFIER_H_
