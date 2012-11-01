#include "LorgApp.h"


LorgApp::LorgApp() : verbose(false), in(NULL), out(NULL) {}

LorgApp::~LorgApp()
{
  if (in != &std::cin) delete in;
  if (out != &std::cout) delete out;
}

bool LorgApp::init(int argc, char **argv)
{

  ConfigTable configuration(argc,argv,get_options());

  bool res = read_config(configuration);
  return res;
}


ConfigTable *  LorgApp::parse_config(int argc, char **argv)
{
  ConfigTable * configuration = new ConfigTable(argc,argv,get_options());

  if (!configuration) {
    std::cerr << "Unable to read options\n";
  }

  return configuration;
}


bool LorgApp::read_config(ConfigTable& configuration)
{
  verbose = configuration.exists("verbose");
  // parse config file if provided
  if(configuration.exists("config-file"))
    {
      if(verbose) std::clog << "Parsing configuration file." << std::endl;
      configuration.parse_config_file(configuration.get_value<std::string>("config-file"));
    }


  if (configuration.exists("help")) {
    configuration.print_help();
    return false;
  }

  return true;
}
