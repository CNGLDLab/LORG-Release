// -*- mode: c++ -*-

#ifndef _EXTRACT_H_
#define _EXTRACT_H_

#include <functional>
#include <vector>
#include <string>

#include "utils/PtbPsTree.h"
#include "Feature.h"



class Extract
{
public:
  Extract(std::vector<Feature>& features_) : features(features_) {};
  Extract() {};
  virtual ~Extract() {};

  //  void extract(const PtbPsTree& tree, std::vector<std::pair>& result);
  void extract(const PtbPsTree& tree, std::string& result);

private:
  std::vector<Feature> features;
};



#endif /* _EXTRACT_H_ */
