// -*- mode: c++ -*-

#ifndef BRACKETING_H
#define BRACKETING_H

struct bracketing{

  unsigned begin;
  bool hard_begin;
  unsigned end;
  bool hard_end;


  ~bracketing(){};

  bracketing(unsigned b, unsigned e)
  : begin(b),hard_begin(true),end(e),hard_end(true)
  {};

  bracketing(unsigned b, bool hb, unsigned e, bool he)
    : begin(b),hard_begin(hb),end(e),hard_end(he)
  {};


  bool overlap(const bracketing& b2) const;


};


// suppose this.begin and this.end are "hard"
// is this rule correct ?
inline
bool bracketing::overlap(const bracketing& b2) const
{
  bool res =
    ( b2.hard_begin && (begin <= b2.begin && b2.begin <= end && end < b2.end))
    ||
    ( b2.hard_end   && (b2.begin < begin && begin <= b2.end && b2.end < end));

  //     if(res)
  //       std::cout << "(" << b1.first << "," << b1.second << ") and "
  // 		<< "(" << b2.first << "," << b2.second << ") overlap" << std::endl;
  //     else
  //       std::cout << "(" << b1.first << "," << b1.second << ") and "
  // 		<< "(" << b2.first << "," << b2.second << ") don't overlap" << std::endl;

  return res;
}

#endif // BRACKETING_H
