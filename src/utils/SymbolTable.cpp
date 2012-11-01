#include "SymbolTable.h"

SymbolTable * SymbolTable::NT_instancePtr = 0;//initialize singlton pointer
SymbolTable * SymbolTable::word_instancePtr = 0;//initialize singlton pointer
std::string const SymbolTable::unknown_string  = "UNK"; // TODO: give a more generic unknown string

SymbolTable& SymbolTable::instance_word()
{ 
  if (word_instancePtr == 0)  // is it the first call?
    word_instancePtr = new SymbolTable(); // create sole instance
  return *word_instancePtr; // address of sole instance
}

SymbolTable& SymbolTable::instance_nt()
{ 
  if (NT_instancePtr == 0)  // is it the first call?
    NT_instancePtr = new SymbolTable(); // create sole instance
  return *NT_instancePtr; // address of sole instance
}

SymbolTable::SymbolTable() : table(), cpt(0) {}

unsigned int SymbolTable::get(const std::string& str) const throw(Miss)
{
  return get_label_id(str);
}

unsigned int SymbolTable::get_label_id(const std::string& str) const throw(Miss)
{
  symtab::left_const_iterator find_str = table.left.find(str);
  if (find_str==table.left.end()) 
    throw(Miss(str)); 
  else
    return find_str->second;
}

int SymbolTable::insert(const std::string& str) throw()
{
  try {
    return get_label_id(str);
  }
  catch (Miss) {
    table.insert(symtab::value_type(str,cpt));
    return cpt++;
  }
}

std::string SymbolTable::translate(unsigned int i) const throw(Miss)
{
  return get_label_string(i);
}

std::string SymbolTable::get_label_string(unsigned int i) const throw(Miss)
{
  symtab::right_const_iterator find_i(table.right.find(i));
  if (find_i==table.right.end())
    throw(Miss(i));
  else
    return find_i->second;
}

