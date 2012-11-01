#ifndef TOKENISERSPEC_H
#define TOKENISERSPEC_H

#include "Word.h"
#include <vector>
#include <string>

class TokeniserSpec
{
public:
  TokeniserSpec()
  {}

  virtual ~TokeniserSpec()
  {}

  virtual bool process( const std::string& sentence ) = 0;
  virtual std::vector< Word > sentence() const = 0;
  virtual void clear() = 0;

};

#endif // TOKENISERSPEC_H
