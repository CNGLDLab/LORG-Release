#ifndef PARSEERROR_H
#define PARSEERROR_H

#include <string>

  class ParseError : public std::exception
  {
    std::string file;
    virtual const char* what() const throw()
    {
      if(file == "")
	return "can't parse grammar string" ;
      else {
	std::string s = "can't parse grammar " + file;
	return (s.c_str());
      }
    }
      
  public:
    ParseError(std::string f = "") : std::exception(), file(f){};
    ~ParseError()throw() {} ;
  }; 

#endif //PARSEERROR_H
