// -*- mode: c++ -*- 
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <boost/bimap/bimap.hpp>
#include <string>
#include <exception>
#include <map>
#include "utils/LorgConstants.h"

#include <sstream>


/**
   \class Miss
   \brief used as exception in class \ref SymbolTable
 */
class Miss : public std::exception
{
  std::string msg;

public:
  virtual const char* what() const throw()
  {
    return msg.c_str();
  };

public:
  
  Miss(const std::string& s) 
    : msg()
  {
    msg = "can't find string \"" + s + "\" in symbol table";
  };

  Miss(const int i) 
  : msg()
  {
    std::ostringstream op;
    op << i;
    msg = "can't find int \"" + op.str() + "\" in symbol table";
  };

  ~Miss()throw() {};

private:
  Miss();
  Miss& operator=(const Miss& other);
}; 


/**
    \class SymbolTable
    \brief makes the  string <--> int translation easier

    This class provides 3 methods
    to ease translations between strings and their encoding as integers.
*/
class SymbolTable
{
private:
  typedef boost::bimaps::bimap<std::string, unsigned int> symtab;

  symtab table;  ///< string to integer bijective mapping
  unsigned int cpt;       ///< numbers of strings inserted so far
  

  static SymbolTable* NT_instancePtr;   ///< pointer to the one instance of SymbolTable for non terminals
  static SymbolTable* word_instancePtr; ///< pointer to the one instance of SymbolTable for terminals
    
private:
  /**
    \brief private constructor
  */
  SymbolTable();
  SymbolTable(SymbolTable const& ); //not defined, not copyable
  SymbolTable& operator= (SymbolTable const& ); //not defined, not assignable
  /**
    \brief Destructor
  */
  ~SymbolTable() {}

public:
  int insert(const std::string& str) throw();
  /**
     \brief Depreciated! See get_string_label() translates the given integer into the associated symbol or returns
     the empty string
    \param i integer to translate
    \return the symbol associated with \param i or the empty string
    \todo raise error instead of returning empty strings
  */

  std::string translate(unsigned int i) const throw(Miss);
  
  /**
     \brief translates the given integer into the associated symbol or returns
     the empty string
    \param i integer to translate
    \return the symbol associated with \param i or the empty string
    \todo raise error instead of returning empty strings
  */

  std::string get_label_string(unsigned int i) const throw(Miss);

  
  /**
     \brief returns the integer for a given symbol
     \param str symbol
     \return the assigned integer to the given symbol
   */

  unsigned int get_label_id(const std::string& str) const throw(Miss);
  /**
     \brief Depreciated - see get_label_id returns the integer for a given symbol
     \param str symbol
     \return the assigned integer to the given symbol
   */

  unsigned int get(const std::string& str) const throw(Miss);

  bool token_exists(const std::string&);

  /**
    \brief Global point of access to SymbolTable for terminals.  

    If this is the first call to instance() it will create a SymbolTable object and return a pointer
    to it.  Otherwise it will just return the pointer to the object.
  */
  static SymbolTable& instance_word();

  /**
    \brief Global point of access to SymbolTable for nonterminals.  

    If this is the first call to instance() it will create a SymbolTable object and return a pointer
    to it.  Otherwise it will just return the pointer to the object.
  */
  static SymbolTable& instance_nt();

  /**
     \brief Returns the number of distinct symbols in the table
  */
  unsigned int get_symbol_count() const;
  
  bool is_root_label(int id);
  
  static const std::string unknown_string; ///< the string corresponding to the unknown token
  
};

inline 
unsigned int SymbolTable::get_symbol_count() const {return table.size();}

//TODO what if this is called as word instance?
inline
bool SymbolTable::is_root_label(int id) {return id == (int) get_label_id(LorgConstants::tree_root_name);}

inline
bool SymbolTable::token_exists(const std::string& token){return table.left.find(token) != table.left.end();}

#endif // SYMTAB_H
