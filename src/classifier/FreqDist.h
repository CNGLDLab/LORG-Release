#ifndef FREQDIST_H_
#define FREQDIST_H_

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>

template<class LABEL, class FEATURE>
class ProbModel;

template<class LABEL, class FEATURE>
class FreqDist;

template<class LABEL, class FEATURE>
std::ostream& operator<<(std::ostream& out, const FreqDist<LABEL,FEATURE>& freqdist);

typedef int VALUE;
typedef unsigned COUNT;
typedef boost::unordered_map<VALUE, COUNT> freqmap;

/**
  \class FreqDist
  \brief Represents a frequency distribution
*/
template<class LABEL,class FEATURE>
class FreqDist
{
public:
  friend class ProbModel<LABEL,FEATURE>;
  friend std::ostream& operator<< <LABEL,FEATURE>(std::ostream& out, const FreqDist<LABEL,FEATURE>& freqdist);

public:
  /**
    \brief Standard constructor
  */
  FreqDist();
  /**
    \brief Constructor, constructs a frequency distribution from a given file specification
    \param filename the specification file
    \note The LABEL and FEATURE classes are required to define the >> and << operator
    in order to read and write correctly to the file.
  */
  FreqDist( const char* filename );

  /**
    \brief equality operator
  */
  bool operator==( const FreqDist& rhs ) const;

  /**
    \brief Returns the total number of samples
    \return total number of samples
  */
  unsigned int get_N() const;
  /**
    \brief Returns the frequency of a certain class label
    \param label the class label
    \return a frequency
  */
  unsigned int get_frequency( const LABEL& label ) const;
  /**
    \brief Returns the frequency of a certain feature value given its class label
    \param label the class label
    \param feature the feature
    \param value the value
    \return a frequency
  */
  unsigned int get_frequency( const LABEL& label, const FEATURE& feature, VALUE value ) const;

  /**
    \brief Increments the number of training instances by a given number (default 1)
    \param count the number of training instances
  */
  void increment_N( COUNT count = 1);
  /**
    \brief Returns the status of the model
    \return True, if the model is empty, false otherwise
  */ 
  bool empty() const;
  /**
    \brief Increments the count of a certain class label by a certain number (default 1)
    \param label the class label
    \param count the number of training instances
  */
  void add_unconditional_count( const LABEL& label, unsigned int count = 1);
  /**
    \brief Increments the count of a certain feature value given its class label by a certain number (default 1)
    \param label the class label
    \param feature the feature
    \param value the value
    \param count the number of training instances
  */
  void add_conditional_count( const LABEL& label, const FEATURE& feature, VALUE value,  unsigned int count = 1);

  /**
    \brief Saves the current distribution to the file system.
    \param filename the filename to save to
    \param text text format true/false
    \note The LABEL and FEATURE classes are required to define the >> and << operator
    in order to read and write correctly to the file.
  */
  void save_distribution( const char* filename, bool text = true ) const;

private:
  /**
    \brief writes a text representation of the distribution to a specified stream
    \param out an output stream
    \return a reference to the used stream
  */
  std::ostream& write_text_representation( std::ostream& out ) const;
  /**
    \brief reads a text representation from a specified stream
    \param inp an input stream
    \return a reference to the used stream
  */
  std::istream& read_text_representation( std::istream& inp );

  unsigned int N; ///< the total number of observed training instances
  boost::unordered_map<FEATURE,boost::unordered_set<VALUE> > value_set; ///< stores the values associated with a feature
  boost::unordered_map<LABEL, COUNT> uncond_fdist; ///< unconditional frequency counts
  boost::unordered_map<LABEL, boost::unordered_map<FEATURE, freqmap > > cond_fdist; ///< conditional frequency counts
};

//#include "FreqDist.cpp"


#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <sstream>

template< class LABEL, class FEATURE >
FreqDist<LABEL,FEATURE>::FreqDist()
: N(0)
{
}

template< class LABEL, class FEATURE >
FreqDist<LABEL,FEATURE>::FreqDist( const char* filename )
: N(0)
{
  std::ifstream inp(filename);
  if(!inp)
  {
    std::cerr << "FreqDist: Error: Could not read file " << filename << "." << std::endl;
    exit(1);
  }
  read_text_representation( inp );
  inp.close();
}

template< class LABEL, class FEATURE >
inline
bool FreqDist<LABEL,FEATURE>::operator==( const FreqDist& rhs ) const
{
  return N == rhs.N &&
               value_set == rhs.value_set && 
               uncond_fdist == rhs.uncond_fdist &&
               cond_fdist == rhs.cond_fdist;
}

template< class LABEL, class FEATURE >
inline
unsigned int FreqDist<LABEL,FEATURE>::get_N() const 
{ 
  return N; 
}

template< class LABEL, class FEATURE >
COUNT FreqDist<LABEL,FEATURE>::get_frequency( const LABEL& label ) const
{
  try
  {
    return uncond_fdist.at(label);
  }
  catch(std::out_of_range& e)
  {
    return 0;
  }
}

template< class LABEL, class FEATURE >
COUNT FreqDist<LABEL,FEATURE>::get_frequency( const LABEL& label, const FEATURE& feature, VALUE value ) const
{
  try
  {
    return cond_fdist.at(label).at(feature).at(value);
  }
  catch(std::out_of_range& e)
  {
    return 0;
  }
}

template< class LABEL, class FEATURE >
inline 
void FreqDist<LABEL,FEATURE>::increment_N( unsigned int count )
{ 
  N += count; 
}

template< class LABEL, class FEATURE >
inline 
bool FreqDist<LABEL,FEATURE>::empty() const 
{ 
  return !N && value_set.empty() && uncond_fdist.empty() && cond_fdist.empty(); 
}

template<class LABEL, class FEATURE >
void FreqDist<LABEL,FEATURE>::add_unconditional_count( const LABEL& label, unsigned int count )
{
  try
  {
    uncond_fdist.at(label) += count;
  }
  catch(std::out_of_range& e)
  {
    uncond_fdist.insert(std::make_pair(label,count));
  }
}

template< class LABEL, class FEATURE >
void FreqDist<LABEL,FEATURE>::add_conditional_count( const LABEL& label, const FEATURE& feature, VALUE value, COUNT count )
{
  try // increment value count
  {
    cond_fdist.at(label).at(feature).at(value) += count;
  }
  catch(std::out_of_range& e)
  {
    cond_fdist[label][feature].insert(std::make_pair(value,count));
  }
  try // add feature's value to value set
  {
    value_set.at(feature).insert(value);
  }
  catch(std::out_of_range& e)
  {
    boost::unordered_set<VALUE> new_set;
    new_set.insert(-1);
    new_set.insert(value);
    value_set.insert(std::make_pair(feature,new_set));
  }
}

template< class LABEL, class FEATURE >
void FreqDist<LABEL,FEATURE>::save_distribution( const char* filename, bool text) const
{
  std::ofstream out(filename);
  if(!out)
  {
    std::cerr << "FreqDist: Error: Could not write to file " << filename << "." << std::endl;
    return;
  }
  if(!text)
    std::cerr << "FreqDist: Error: No binary format available yet." << std::endl;
  write_text_representation(out);
  out.close();
}

template< class LABEL, class FEATURE >
std::ostream& FreqDist<LABEL,FEATURE>::write_text_representation( std::ostream& out ) const
{
  std::cerr << "write model ..." << std::endl;
  // output N
  out << N << "\n";
  out << "----\n";
  // output value set
  for(typename boost::unordered_map<FEATURE,boost::unordered_set<VALUE> >::const_iterator value_item = value_set.begin();
        value_item != value_set.end(); ++value_item)
  {
    out << value_item->first << "\n";
    for(typename boost::unordered_set<VALUE>::const_iterator value = value_item->second.begin();
           value != value_item->second.end(); ++value)
      out << boost::lexical_cast<std::string>(*value) << " ";
    out << "\n";
  }
  out << "----\n";
  // output unconditional frequencies
  for(typename boost::unordered_map<LABEL,COUNT>::const_iterator label_item = uncond_fdist.begin();  
         label_item != uncond_fdist.end(); ++label_item)
    out << label_item->second << " " << label_item->first << "\n";
  out << "----\n";
  // output conditional frequencies
  for(typename boost::unordered_map<LABEL,boost::unordered_map<FEATURE, freqmap > >::const_iterator label_item2 = cond_fdist.begin();
         label_item2 != cond_fdist.end(); ++label_item2)
  {
    out << label_item2->first << "\n";
    for(typename boost::unordered_map<FEATURE, freqmap >::const_iterator feature_item = label_item2->second.begin();
           feature_item != label_item2->second.end(); ++feature_item)
    {
      out << feature_item->first << "\n";
      for(typename freqmap::const_iterator value_item2 = feature_item->second.begin();
             value_item2 != feature_item->second.end(); ++value_item2)
        out << value_item2->second << " " << boost::lexical_cast<std::string>(value_item2->first) << "\n";
      out << "--\n";
    }
    out << "---\n";
  }
  return out;
}

template< class LABEL, class FEATURE >
std::istream& FreqDist<LABEL,FEATURE>::read_text_representation( std::istream& inp )
{
  unsigned int mode = 0;
  bool vsmode = true;
  unsigned int cfmode = 0;
  std::string line;
  LABEL label;
  FEATURE feature;
  freqmap value_cnts;  
  while(std::getline(inp,line))
  {
    if( line == "----" ) 
    {
      ++mode;
      continue;
    }
    std::istringstream iss(line);
    switch(mode)
    {
      case 0: // N
        iss >> N;
        break;
      case 1: // value set
        if(vsmode) iss >> feature;
        else
        {
          boost::unordered_set<VALUE> values;
          VALUE value;
          while( iss >> value )
            values.insert(value);
          value_set.insert(std::make_pair(feature,values));
        }
        vsmode = !vsmode;
        break;
      case 2: // unconditional frequencies
        unsigned int freq;
        iss >> freq;
        iss >> label;
        uncond_fdist.insert(std::make_pair(label,freq));
        break;
      case 3: // conditional frequencies
        switch(cfmode)
        {
          case 0: // label
            iss >> label;
            ++cfmode;
            break;
          case 1: // feature
            if(line == "---") --cfmode;
            else 
            {
              iss >> feature;
              ++cfmode;
            }
            break;
          case 2: // freq-value pairs
            if(line == "--")
            {
              cond_fdist[label][feature] = value_cnts;
              value_cnts.clear();
              --cfmode;
            }
            else
            {
              COUNT frequency;
              VALUE value;
              iss >> frequency;
              iss >> value;
              value_cnts.insert(std::make_pair(value,frequency));
            }
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  return inp;
}

template<class LABEL,class FEATURE>
std::ostream& operator<<(std::ostream& out, const FreqDist<LABEL,FEATURE>& freqdist)
{
  return freqdist.write_text_representation( out );
}



#endif // FREQDIST_H_
