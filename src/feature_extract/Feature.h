// -*- mode: c++ -*-

#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <functional>
#include <string>
#include <map>

#include <iostream>
#include <sstream>


#include "utils/PtbPsTree.h"


struct function_feature : std::binary_function<PtbPsTree::const_depth_first_iterator,
                                               PtbPsTree,
                                              std::string>
{
  virtual std::string operator()(PtbPsTree::const_depth_first_iterator&, const PtbPsTree&) const =0;
  // { std::cout << "HERE" << std::endl; return "";};
};


class Feature
{
public:
  Feature(const std::string& id_, const function_feature* ff_) :
    id(id_), ff(ff_) {};
  virtual ~Feature() {};

  std::string get_id() const;
  std::string extract(PtbPsTree::const_depth_first_iterator& i, const PtbPsTree& t) const
  {
    return (*ff)(i,t);
  }


  std::string extract_with_name(PtbPsTree::const_depth_first_iterator& i, const PtbPsTree& t) const
  {
    //    std::cout << "1" << std::endl;
    std::string s = (*ff)(i,t);
    //    std::cout << "2" << std::endl;
    if(s != "") {
      std::stringstream ss(std::stringstream::in |std::stringstream::out);
      //      std::cout << "3" << std::endl;
      ss << id << "=" << s;
      return  ss.str();
    }
    else
      return "";
  }


private:
  std::string id;
  const function_feature* ff;

  static std::map<std::string, Feature> id2feature_map;

};


class RuleFeature : public Feature
{
public:
  RuleFeature();
  virtual ~RuleFeature();
};

class RuleParentFeature : public Feature
{
public:
  RuleParentFeature();
  virtual ~RuleParentFeature();
};

class RuleGrandParentFeature : public Feature
{
public:
  RuleGrandParentFeature();
  virtual ~RuleGrandParentFeature();
};


class BiGramNodeFeature : public Feature
{
public:
  BiGramNodeFeature();
  virtual ~BiGramNodeFeature();
};


class HeavyFeature : public Feature
{
public:
  HeavyFeature();
  virtual ~HeavyFeature();
};


class NeighboursFeature : public Feature
{
public:
  NeighboursFeature();
  virtual ~NeighboursFeature();
};

class NeighboursExtFeature : public Feature
{
public:
  NeighboursExtFeature();
  virtual ~NeighboursExtFeature();
};


class WordFeature2 : public Feature
{
public:
  WordFeature2();
  virtual ~WordFeature2();
};

class WordFeature3 : public Feature
{
public:
  WordFeature3();
  virtual ~WordFeature3();
};

class WordFeatureGen2 : public Feature
{
public:
  WordFeatureGen2();
  virtual ~WordFeatureGen2();
};

class WordFeatureGen3 : public Feature
{
public:
  WordFeatureGen3();
  virtual ~WordFeatureGen3();
};


// class BiGramTreeFeature : public Feature
// {
// public:
//   BiGramFeature();
//   virtual ~BiGramFeature();
// };


// class TriGramTreeFeature : public Feature
// {
// public:
//   TriGramFeature();
//   virtual ~TriGramFeature();
// };





#endif /* _FEATURE_H_ */
