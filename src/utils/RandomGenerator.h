#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random.hpp>


class RandomGenerator
{
private:
  double min;
  double max;

  // do we still need this attribute ?
  unsigned seed;


  static unsigned global_seed;
  static RandomGenerator * my_instance;


  // This is a typedef for a random number generator.
  // Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
  //typedef boost::mt19937 base_generator_type;
  typedef boost::minstd_rand base_generator_type;
  boost::variate_generator<base_generator_type&, boost::uniform_real<double> > *uni;

  RandomGenerator(double min_value, double max_value, unsigned seed = global_seed);
 public:

  static RandomGenerator * instance();
  double next();

  static void set_global_seed(unsigned value);
  static unsigned get_global_seed();

};

#endif //RANDOMGENERATOR_H
