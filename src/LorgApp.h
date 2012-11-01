// -*- mode: c++ -*-
#ifndef LORGAPP_H
#define LORGAPP_H

#include "utils/ConfigTable.h"

class LorgApp
{
public:
  LorgApp();

  bool init(int argc, char **argv);
  virtual int run() = 0;

  virtual ~LorgApp();

private:
  ConfigTable *  parse_config(int argc, char **argv);

protected:
  virtual bool read_config(ConfigTable& configuration);
  virtual LorgOptions get_options()const =0;

protected:
  bool verbose; // maybe use an integer to have several verbosity levels ?

  std::istream* in;
  std::ostream* out;
};


#endif //LORGAPP_H
