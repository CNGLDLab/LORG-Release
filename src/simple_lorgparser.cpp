#include "SimpleLorgParseApp.h"


int main(int argc, char** argv)
{
  SimpleLorgParseApp app;
  
  if (!app.init(argc,argv)) return -1;
  
  app.run();

  return 0;
}
