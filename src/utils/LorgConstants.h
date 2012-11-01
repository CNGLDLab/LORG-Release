#ifndef LORGCONSTANTS_H
#define LORGCONSTANTS_H

#include <string>

#define MAP boost::unordered_map
//#define MAP std::map

namespace LorgConstants
{
  static const std::string tree_root_name = "TOP";
  static const std::string token_number   = "|NUMBER|";
  static const std::string token_unknown  = "UNK";

  // Do we need this  ?
  static const double NullProba = 2.0;
}


#endif  //LORGCONSTANTS_H
