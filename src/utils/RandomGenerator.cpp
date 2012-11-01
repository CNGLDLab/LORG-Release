#include "RandomGenerator.h"
#include <ctime> 

unsigned RandomGenerator::global_seed = 29 ; /*std::time(0);*/

RandomGenerator * RandomGenerator::my_instance = NULL;


RandomGenerator::RandomGenerator(double min_value, double max_value, unsigned seed_value) : min(min_value), max(max_value), seed(seed_value)
{}

void RandomGenerator::set_global_seed(unsigned value)
{
  global_seed = value;
}


unsigned RandomGenerator::get_global_seed()
{
  return global_seed;
}


double RandomGenerator::next()
{
  static base_generator_type generator(seed);
  static boost::uniform_real<double> dist(min, max);
  static boost::variate_generator<base_generator_type&, boost::uniform_real<double> > uni(generator, dist);


  return uni();
}


RandomGenerator * RandomGenerator::instance() 
{
  if (my_instance == NULL) {
    my_instance = new RandomGenerator(0,1,global_seed);
  }

  return my_instance;

}
