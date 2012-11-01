// -*- mode: c++ -*-

#ifndef _PACKEDEDGEPROBABILITY_H_
#define _PACKEDEDGEPROBABILITY_H_

#include <limits>

#include "AnnotationInfo.h"
#include "rules/LexicalRuleC2f.h"

class PackedEdgeDaughters;

struct packed_edge_probability
{
  double probability;	   ///< the best probabilities for all each edge in this packed edge
  const PackedEdgeDaughters* dtrs;        ///< the best rhs for each edge in this packed edge
  unsigned left_index;	   ///< the indices to the best edge for the left dtrs in best_dtrs;
  unsigned right_index;	   ///< the indices to the best edge for the right dtrs in best_dtrs;

  packed_edge_probability() :
    probability(-std::numeric_limits<double>::infinity()),
    dtrs(NULL) , left_index(0), right_index(0)
  {};

  bool operator<(const packed_edge_probability& other) const
  {
    return probability < other.probability;
  }

  bool operator>(const packed_edge_probability& other) const
  {
    return probability > other.probability;
  }

};

#endif /* _PACKEDEDGEPROBABILITY_H_ */
