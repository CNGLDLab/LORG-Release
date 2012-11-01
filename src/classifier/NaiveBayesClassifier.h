#ifndef NAIVEBAYESCLASSIFIER_H_
#define NAIVEBAYESCLASSIFIER_H_

#include "Classifier.h"
#include "ProbModel.h"
#include <boost/unordered_set.hpp>
#include <vector>
#include <utility>
#include <memory>

/**
  \class NaiveBayesClassifier
  \brief Represents a classifer that uses the Naive Bayes Assumption to classify data
  \param LABEL the type of class labels
  \param FEATURE the type of data

  This classifier uses the formula argmax_v P(v) Pi P(a_i|v) , where v is a class label and a_i the ith feature.
*/
template<class LABEL = unsigned int, class FEATURE = unsigned int>
class NaiveBayesClassifier : public Classifier<LABEL,FEATURE>
{
public:
  typedef typename Classifier<LABEL,FEATURE>::TrainingInstance TrainingInstance;
  typedef typename Classifier<LABEL,FEATURE>::Datum Datum;

  /**
    \brief Standard Constructor
    \param sstype the smoothing method
  */
  NaiveBayesClassifier( SmoothingType sstype = None );
  /**
    \brief Constructor which will train the classifier from a given frequency distribution specification file
    \param filename the specification file
    \param sstype the smoothing method
  */
  NaiveBayesClassifier( const char* filename, SmoothingType sstype = Lidstone );
  /**
    \brief Constructor which will train the classifier from a given frequency distribution
    \param freqdist the frequency distribution
    \param sstype the smoothing method
    \note The LABEL and FEATURE classes are required to define the >> and << operator
    in order to read and write correctly to the file.
  */
  NaiveBayesClassifier( const FreqDist<LABEL,FEATURE>& freqdist, SmoothingType sstype = Lidstone );
  /**
    \brief Constructor which will train the classifier from the given data
    \param data the data to train from
    \param sstype the smoothing method
  */
  NaiveBayesClassifier( const std::vector<std::pair<TrainingInstance,unsigned int> >& data, SmoothingType sstype = Lidstone );
  /**
    \brief Copy Constructor
    \param rhs the instance to copy from
  */
  NaiveBayesClassifier( const NaiveBayesClassifier<LABEL,FEATURE>& rhs );
  /**
    \brief Destructor
  */
  virtual ~NaiveBayesClassifier();

  /**
    \brief Assignment operator
  */
  NaiveBayesClassifier<LABEL,FEATURE>& operator=( const NaiveBayesClassifier<LABEL,FEATURE>& rhs );

  /**
    \brief returns a reference to a dynamically allocated copy of this
    \return a pointer to a copy of this
    \note this is bad, but the only way
  */
  NaiveBayesClassifier<LABEL,FEATURE>* copy() const;

  /**
    \brief Returns the smoothing type of the internal model
    \return the smoothing type
  */
  SmoothingType get_smoothing_type() const;
  /**
    \brief Checks, whether the classifier is trained already
    \return true, if trained, false elsewise
  */
  bool trained() const;
  /**
    \brief trains the classifier from a vector of training instances
    \param data a vector of TrainingInstance
  */
  void train( const std::vector<std::pair<TrainingInstance,unsigned int> >& data );  
  /**
    \brief classifies a given instance and returns all labels with a probability higher than a certain threshold
    \param instance a vector of Datum
    \param threshold a probability threshold
    \return a vector of class labels ranked by probability in descending order
  */
  std::vector<LABEL> classify( const std::vector<Datum>& instance, double threshold ) const;
  /**
    \brief classifies a given instance and returns the k-best labels
    \param instance a vector of Datum
    \param k the size of the returned vector
    \return a vector of class labels ranked by probability in descending order
  */
  std::vector<LABEL> classify_nbest( const std::vector<Datum>& instance, unsigned int k ) const;
  /**
    \brief classifies a given instance and returns the best label
    \param instance a vector of Datum
    \return the most probable class label
    \note This is an overloaded function provided for convenience 
    \see classify
  */
  LABEL classify( const std::vector<Datum>& instance ) const;
  /**
    \brief Returns the estimated probability for a class label given a test instance
    \param label the label
    \param instance the test instance
    \return the probability of the class label
  */
  double probability( const LABEL& label, const std::vector<Datum>& instance ) const;
  
  /**
    \brief Saves the underlying probability model to a specified file
    \param filename the filename
  */
  void save_probability_model( const char* filename ) const;
  
private:
  /**
    \brief scans training data and stores feature set
    \param data the training data
  */
  boost::unordered_set<FEATURE> scan_data( const std::vector<std::pair<TrainingInstance,unsigned int> >& data );

  SmoothingType stype;  ///< the used smoothing method
  std::auto_ptr<ProbModel<LABEL,FEATURE> > model; ///< the probability model
};

//#include "NaiveBayesClassifier.cpp"


#include <iostream>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <list>
#include "FreqDist.h"

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::NaiveBayesClassifier( SmoothingType sstype )
: stype(sstype)
{
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::NaiveBayesClassifier( const char* filename, SmoothingType sstype)
: stype(sstype),
  model(new ProbModel<LABEL,FEATURE>(FreqDist<LABEL,FEATURE>(filename),sstype))
{
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::NaiveBayesClassifier( const FreqDist<LABEL,FEATURE>& freqdist, SmoothingType sstype )
: stype(sstype),
  model(new ProbModel<LABEL,FEATURE>(freqdist,sstype))
{
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::NaiveBayesClassifier( const std::vector<std::pair<TrainingInstance,unsigned int> >& data, SmoothingType sstype )
: stype(sstype)
{
  train(data);
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::NaiveBayesClassifier( const NaiveBayesClassifier<LABEL,FEATURE>& rhs )
: stype(rhs.stype),
  model(new ProbModel<LABEL,FEATURE>(*(rhs.model)))
{
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>::~NaiveBayesClassifier()
{
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>& NaiveBayesClassifier<LABEL,FEATURE>::operator=( const NaiveBayesClassifier<LABEL,FEATURE>& rhs )
{
  if( this != &rhs )
  {
    stype = rhs.stype;
    model.reset(new ProbModel<LABEL,FEATURE>(*(rhs.model)));
  }
  return *this;
}

template<class LABEL, class FEATURE>
NaiveBayesClassifier<LABEL,FEATURE>* NaiveBayesClassifier<LABEL,FEATURE>::copy() const
{
  return new NaiveBayesClassifier<LABEL,FEATURE>(*this);
}

template<class LABEL, class FEATURE>
inline
SmoothingType NaiveBayesClassifier<LABEL,FEATURE>::get_smoothing_type() const 
{ 
  return stype; 
}

template<class LABEL, class FEATURE>
inline
bool NaiveBayesClassifier<LABEL,FEATURE>::trained() const 
{ 
  return model.get() && !model->empty();
}

template<class LABEL, class FEATURE>
void NaiveBayesClassifier<LABEL,FEATURE>::train( const std::vector<std::pair<TrainingInstance,unsigned int> >& data )
{
  int i = 0;
  boost::unordered_set<FEATURE> feature_set =  scan_data( data );
  FreqDist<LABEL,FEATURE> fd;
  for(typename std::vector<std::pair<TrainingInstance,unsigned int> >::const_iterator ti = data.begin(); ti != data.end(); ++ti)
  {
    std::cerr << "NBC: training with instance " << ++i << " of " << data.size() << "\r";
    LABEL current_label = ti->first.get_label();
    fd.increment_N(ti->second);
    fd.add_unconditional_count(current_label,ti->second);
    boost::unordered_set<FEATURE> no_zero_features;

    for(typename TrainingInstance::const_iterator d = ti->first.begin(); d != ti->first.end(); ++d)
    {
      no_zero_features.insert(d->feature);
      fd.add_conditional_count(current_label,d->feature,d->value,ti->second);
    }

    for(typename boost::unordered_set<FEATURE>::const_iterator feature = feature_set.begin(); feature != feature_set.end(); ++feature)
    {
      if( !no_zero_features.count(*feature) )
        fd.add_conditional_count(current_label,*feature,-1,ti->second);
    }
  }
  std::cerr << std::endl;
  model.reset(new ProbModel<LABEL,FEATURE>(fd,stype));
}

template<class LABEL, class FEATURE>
std::vector<LABEL> NaiveBayesClassifier<LABEL,FEATURE>::classify( const std::vector<Datum>& instance, double threshold ) const
{
  if(!trained())
    throw(NoTrainingDataAvailable());

  std::list<std::pair<LABEL,double> > ranking;
  BOOST_FOREACH( LABEL label, model->get_labels() )
  {
    double product = probability(label,instance);
    typename std::list<std::pair<LABEL,double> >::iterator l = ranking.begin();
    for(; l != ranking.end(); ++l)
    {
      if( product > l->second )
        break;
    }
    ranking.insert(l, std::make_pair(label,product));
  }
  // get the labels from the list that exceed a certain threshold
  std::vector<LABEL> labels;
  std::pair<LABEL,double> item;
  BOOST_FOREACH( item, ranking)
  {
    if(item.second < threshold || ranking.empty())
      break;
    labels.push_back(ranking.front().first);
    ranking.pop_front();
  }  
  return labels;
}
// #include <iostream>
template<class LABEL, class FEATURE>
std::vector<LABEL> NaiveBayesClassifier<LABEL,FEATURE>::classify_nbest( const std::vector<Datum>& instance, unsigned int k ) const
{
  if(!trained())
    throw(NoTrainingDataAvailable());
  if(!k) k = 1;

  std::list<std::pair<LABEL,double> > ranking;
  BOOST_FOREACH( LABEL label, model->get_labels() )
  {
    double product = probability(label,instance);
// std::cerr << label << " " << product << "\n";
    typename std::list<std::pair<LABEL,double> >::iterator l = ranking.begin();
    for(; l != ranking.end(); ++l)
    {
      if( product > l->second )
        break;
    }
    ranking.insert(l, std::make_pair(label,product));
  }
  // get the first k labels from the list
  std::vector<LABEL> labels;
  for(unsigned int n = 0; n < k; ++n)
  {
    labels.push_back(ranking.front().first);
    ranking.pop_front();
    if(ranking.empty())
      break;
  }  
  return labels;
}

template<class LABEL, class FEATURE>
LABEL NaiveBayesClassifier<LABEL,FEATURE>::classify( const std::vector<Datum>& instance ) const
{
  return classify_nbest(instance,1)[0];
}

template<class LABEL, class FEATURE>
double NaiveBayesClassifier<LABEL,FEATURE>::probability( const LABEL& label, const std::vector<Datum>& instance ) const
{
  double product = model->get_probability(label);
  boost::unordered_set<FEATURE> no_zero_features;
  BOOST_FOREACH( Datum d, instance )
  {
    if( model->has_feature(d.feature) )
    {
      no_zero_features.insert(d.feature);
      product += model->get_probability(label, d.feature, d.value);
    }
  }
  BOOST_FOREACH( FEATURE feature, model->get_features() )
  {
    if( !no_zero_features.count(feature) )
      product += model->get_probability(label,feature,-1);
  }
  return product;
}

template<class LABEL, class FEATURE>
inline
void NaiveBayesClassifier<LABEL,FEATURE>::save_probability_model( const char* filename ) const
{
  model->save_frequency_distribution(filename);
}

template<class LABEL, class FEATURE>
boost::unordered_set<FEATURE> NaiveBayesClassifier<LABEL,FEATURE>::scan_data( const std::vector<std::pair<TrainingInstance,unsigned int> >& data )
{
  boost::unordered_set<FEATURE> feature_set;
  std::pair<TrainingInstance,unsigned int> ti;
  BOOST_FOREACH( ti, data )
    BOOST_FOREACH( Datum d, ti.first )
      feature_set.insert(d.feature);
  return feature_set;
}



#endif //NAIVEBAYESCLASSIFIER_H_
