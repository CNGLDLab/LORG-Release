// -*- mode: c++ -*-
#ifndef _EXTRACTAPP_H_
#define _EXTRACTAPP_H_

#include "LorgApp.h"
#include "Extract.h"



class ExtractApp : public LorgApp
{
public:
  int run();
  LorgOptions get_options() const;
  ExtractApp();
private:
  Extract extractor;
};


#endif /* _EXTRACTAPP_H_ */
