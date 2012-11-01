#include "LorgTrainerApp.h"

int main(int argc, char** argv)
{
  LorgTrainerApp trainer;
  if(!trainer.init(argc,argv)) (exit(0)); // should return a meaningful int
  
  return trainer.run();
}
