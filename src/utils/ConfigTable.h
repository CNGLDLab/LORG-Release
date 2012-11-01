#ifndef CONFIGTABLE_H_
#define CONFIGTABLE_H_

#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include "LorgOptions.h"

namespace po = boost::program_options;

class NotInitialised{};
class NotSpecified{};

class ConfigTable
{
public:
  ~ConfigTable();
  ConfigTable( int argc, char** argv, const LorgOptions& opts );

  template<class TYPE>
    const TYPE& get_value( const std::string& option ) const;
  bool exists( const std::string& option ) const;

  void parse_config_file(const std::string& config_file );
  void print_help() const;

private:
  ConfigTable();

  // not implemented to prevent copying:
  ConfigTable( const ConfigTable& rhs );
  ConfigTable& operator=( const ConfigTable& rhs );

private:
  LorgOptions options;
};

template<class TYPE>
inline
const TYPE& ConfigTable::get_value( const std::string& option ) const
{
  if(!options.variables.count(option)) {
    std::cerr << option << " is not a valid option" << std::endl;
    throw NotSpecified();
  }
  return options.variables[option].as<TYPE>();
}

inline
bool ConfigTable::exists( const std::string& option ) const { return options.variables.count(option); }

inline
void ConfigTable::print_help() const { std::cerr << options.options << std::endl; }

#endif // CONFIGTABLE_H_
