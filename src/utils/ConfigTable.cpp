#include <fstream>
#include "ConfigTable.h"

ConfigTable::~ConfigTable() {}

void ConfigTable::parse_config_file(const std::string& config_file)
{
  std::ifstream config_stream(config_file.c_str());
  if(!config_stream)
  {
    std::clog << "ConfigTable: Error: Could not open file " << config_file << "." << std::endl;
    return;
  }
  po::store(po::parse_config_file(config_stream, options.options), options.variables);
  notify(options.variables);
}

ConfigTable::ConfigTable()
{
}

ConfigTable::ConfigTable( int argc, char** argv, const LorgOptions& opts )
: options(opts)
{
  try
  {
    options.initialise(argc,argv);
  }
  catch(po::unknown_option& e)
  {
    std::cerr << "Error when parsing command line: " <<  e.what() << "\n\nExit program. See help (--help,-h) for more information.\n";
    //print_help();
    exit(1);
  }
}
