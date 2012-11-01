#ifndef PROBMODEL_H_
#define PROBMODEL_H_

#include "FreqDist.h"
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

/**
  \enum ProbType
  \brief The types of probability representation
*/
// typedef enum
// {
//   Normal, ///< normal probabilities
//   Logarithmic ///< logarithmic probabilities
// } ProbType;

/**
  \enum SmoothingType
  \brief Different smoothing methods
*/
typedef enum
{
  None, ///< no smoothing
  Lidstone ///< Lidstone aka. Add-One smoothing
} SmoothingType;

/**
  \class ProbModel
  \brief Represents a probability distribution
  
  This class will take an object of FreqDist and generate from its counts a probability distribution.
  The counts will be lost unless you keep the FreqDist object.
  The probabilities can optionally be represented as logarithms.
  The distribution can optionally be smoothed according to Lidstone smoothing.

  \todo find a way to store the model binary and get rid of the text format and the freqdist member
*/
template<class LABEL,class FEATURE>
class ProbModel
{
public:
  /**
    \brief This constructor takes a frequency distribution and creates a probability distribution
    \param ffreqdist The frequency distribution to generate from
    \param ptype The representation type of probabilities
    \param stype The smoothing method
  */
  ProbModel( const FreqDist<LABEL,FEATURE>& ffreqdist, SmoothingType stype = Lidstone );

  /**
    \brief equality operator
  */
  bool operator==( const ProbModel<LABEL,FEATURE>& rhs ) const;

  /**
    \brief Returns the probability of a certain class label
    \param label the class label
    \return a probability
  */
  double get_probability( const LABEL& label ) const;
  /**  
    \brief Returns the probability of a certain feature value given its class label
    \param label the class label
    \param feature the feature
    \param value the value
    \return a probability
  */
  double get_probability( const LABEL& label, const FEATURE& feature, double value ) const;
  /**
    \brief Returns a vector containing all class labels the model knows of
    \return a vector of class labels
  */
  std::vector<LABEL> get_labels() const;
  /**
    \brief Returns a set of all features the model knows of
    \return a set of features
  */
  boost::unordered_set<FEATURE> get_features() const;
  /**
    \brief Checks whether a certain label is known to the model
    \param label the class label
    \return true, if the label is known, false otherwise
  */
  bool has_label( const LABEL& label ) const;
  /**
    \brief Checks whether a certain feature is known to the model
    \param feature the feature name
    \return true, if the feature is known, false otherwise
  */
  bool has_feature( const FEATURE& feature ) const;
  /**
    \brief Checks whether the model is empty
    \return true, if the model is empty, false otherwise
  */
  bool empty() const ;

  /**
    \brief Saves the underlying frequency distribution to a specified file
    \param filename the filename
  */
  void save_frequency_distribution( const char* filename ) const;

private:
  ProbModel();

  /**
    \brief Computes the unconditional probabilities from a given frequency distribution according to representation type and smoothing method
    \param ptype the representation type
    \param stype the smoothing method
  */
  void populate_uncond_pdist( SmoothingType stype );
  /**
    \brief Computes the conditional probabilities from a given frequency distribution according to representation type and smoothing method
    \param ptype the representation type
    \param stype the smoothing method
  */
  void populate_cond_pdist( SmoothingType stype );

  double zero_prob; ///< zero probability according to representation type
  boost::unordered_set<FEATURE> feature_set; ///< the feature set
  boost::unordered_map<LABEL, double> uncond_pdist; ///< unconditional probabilities
  boost::unordered_map<LABEL, boost::unordered_map<FEATURE, boost::unordered_map<int, double> > > cond_pdist; ///< conditional probabilities
  FreqDist<LABEL,FEATURE> freqdist; ///< the underlying frequency distribution
};

//#include "ProbModel.cpp"


#include <boost/foreach.hpp>
#include <cmath>
#include <limits>

template<class LABEL,class FEATURE>
ProbModel<LABEL,FEATURE>::ProbModel()
{
  // empty and private, nobody is supposed to use it
}

template<class LABEL,class FEATURE>
ProbModel<LABEL,FEATURE>::ProbModel( const FreqDist<LABEL,FEATURE>& ffreqdist, SmoothingType stype)
: freqdist(ffreqdist)
{
  zero_prob = -std::numeric_limits<double>::infinity();
  populate_uncond_pdist(stype);
  populate_cond_pdist(stype);
}

template<class LABEL,class FEATURE>
inline
bool ProbModel<LABEL,FEATURE>::operator==( const ProbModel<LABEL,FEATURE>& rhs ) const
{
  return zero_prob == rhs.zero_prob &&
               feature_set == rhs.feature_set &&
               uncond_pdist == rhs.uncond_pdist &&
               cond_pdist == rhs.cond_pdist &&
               freqdist == rhs.freqdist;
}

template<class LABEL,class FEATURE>
double ProbModel<LABEL,FEATURE>::get_probability( const LABEL& label ) const
{
  try
  {
    return uncond_pdist.at(label);
  }
  catch(std::out_of_range& e)
  {
    return zero_prob;
  }
}

template<class LABEL,class FEATURE>
double ProbModel<LABEL,FEATURE>::get_probability( const LABEL& label, const FEATURE& feature, double value ) const
{
  // TODO: add check for unseen feature value here
  try
  {
    return cond_pdist.at(label).at(feature).at(value);
  }
  catch(std::out_of_range& e)
  {
    try
    {
      return cond_pdist.at(label).at(feature).at(-1);
    }
    catch(std::out_of_range& e)
    {
      return zero_prob;
    }
  }
}

template<class LABEL,class FEATURE>
std::vector<LABEL> ProbModel<LABEL,FEATURE>::get_labels() const
{  
  std::vector<LABEL> labels;
  std::pair<LABEL,double> item;
  BOOST_FOREACH( item, uncond_pdist )
    labels.push_back(item.first);
  return labels;
}

template<class LABEL,class FEATURE>
inline
boost::unordered_set<FEATURE> ProbModel<LABEL,FEATURE>::get_features() const 
{ 
  return feature_set; 
}

template<class LABEL,class FEATURE>
inline
bool ProbModel<LABEL,FEATURE>::has_label( const LABEL& label ) const 
{ 
  return uncond_pdist.count(label); 
}

template<class LABEL,class FEATURE>
inline
bool ProbModel<LABEL,FEATURE>::has_feature( const FEATURE& feature ) const 
{ 
  return feature_set.count(feature); 
}

template<class LABEL,class FEATURE>
inline
bool ProbModel<LABEL,FEATURE>::empty() const 
{ 
  return feature_set.empty() && uncond_pdist.empty() && cond_pdist.empty(); 
}

template<class LABEL, class FEATURE>
inline
void ProbModel<LABEL,FEATURE>::save_frequency_distribution( const char* filename ) const
{
  freqdist.save_distribution(filename);
}

template<class LABEL,class FEATURE>
void ProbModel<LABEL,FEATURE>::populate_uncond_pdist( SmoothingType stype )
{
// std::cerr << "populate_uncond_pdist" << std::endl;
  if(!freqdist.N)
    return;
  for(typename boost::unordered_map<LABEL,unsigned int>::const_iterator item = freqdist.uncond_fdist.begin();
        item != freqdist.uncond_fdist.end(); ++item)
  {
    double count = double(item->second);
    switch(stype)
    {
      case Lidstone: // number of occurrence with this class label + 1 / total number of instances + total number of class labels
        count += 1.0;
        count *= double(freqdist.N)/double(freqdist.N + freqdist.uncond_fdist.size());
        break;
      case None:
      default: 
        break;
    }
    uncond_pdist.insert(std::make_pair(item->first,log(count/double(freqdist.N))));
  }
}

template<class LABEL,class FEATURE>
void ProbModel<LABEL,FEATURE>::populate_cond_pdist( SmoothingType stype )
{
// std::cerr << "populate_cond_pdist" << std::endl;
  for(typename boost::unordered_map<LABEL,boost::unordered_map<FEATURE,freqmap > >::const_iterator label_item = freqdist.cond_fdist.begin();
         label_item != freqdist.cond_fdist.end(); ++label_item)
  { 
    // for every feature in the feature set of the fdist ...
    for(typename boost::unordered_map<FEATURE,boost::unordered_set<VALUE> >::const_iterator feature_item = freqdist.value_set.begin();
           feature_item != freqdist.value_set.end(); ++feature_item)
    { 
      // store it in the feature set
      feature_set.insert(feature_item->first);
      // and create an entry for every of its possible values in the model
      for(boost::unordered_set<int>::const_iterator value = feature_item->second.begin(); value != feature_item->second.end(); ++value)
      {
        double total = freqdist.get_frequency(label_item->first);
        double count = double(freqdist.get_frequency(label_item->first, feature_item->first, *value));
        switch(stype)
        {
          case Lidstone: // number of occurrence + 1 / total number of instances with this class + number of different values
            count += 1.0;
            count *= total/double(total + feature_item->second.size());
            break;
          case None:
          default: 
            break;
        }
        cond_pdist[label_item->first][feature_item->first].insert(std::make_pair(*value,log(count/total)));
      }
    }
  }
// std::cerr << "populate_cond_pdist done" << std::endl;
}



#endif // PROBMODEL_H_
