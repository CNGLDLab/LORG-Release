// -*- mode: c++ -*-

#ifndef _ANNOTATIONINFO_H_
#define _ANNOTATIONINFO_H_

#include <vector>
#include <cmath>

#include <iostream>
#include <algorithm>

#define LOGSCALE 100
#define SCALE std::exp(LOGSCALE)

struct scaled_array
{
  std::vector<double> array;
  int scale;

  scaled_array();

  scaled_array(unsigned i, double d);

  scaled_array(const scaled_array& other);

  void scale_array(int previous_scale);


  double get_scaled_value(unsigned i) const;

  double get_scaled_logvalue(unsigned i) const;


  void reset(double value);

  void resize(unsigned new_size);

  static double calculate_scalingfactor(int previous);

  static double calculate_logscalingfactor(int previous);

  bool operator==(const scaled_array& other) const
  {
    return array == other.array && scale == other.scale;
  }

};


/**
   \struct AnnotationInfo
   \brief stores probabilities for annotated nodes
*/

struct AnnotationInfo
{
  scaled_array inside_probabilities;
  scaled_array outside_probabilities;

  scaled_array inside_probabilities_unary_temp;
  scaled_array outside_probabilities_unary_temp;


  AnnotationInfo();

  AnnotationInfo(unsigned i, double d);

  AnnotationInfo(const AnnotationInfo& other);

  double get_inside(int i) const;

  double get_outside(int i) const;

  int get_inside_scale() const;

  int get_outside_scale() const;

  void reset_inside_probabilities(double value = 0.0);

  void reset_outside_probabilities(double value = 0.0);

  void reset_probabilities( double value = 0.0);

  unsigned get_size() const;

  void resize(unsigned new_size);

  bool valid_prob_at(unsigned i, double invalid) const;

  bool operator==(const AnnotationInfo &other) const
  {
    return inside_probabilities == other.inside_probabilities
      && outside_probabilities == other.outside_probabilities;
  }

};


//not clean
// should not add into std namespace
namespace std {
  template<>
  inline
  void swap<scaled_array>(scaled_array& a, scaled_array&b)
  {
    std::swap(a.scale, b.scale);
    std::swap(a.array, b.array);
  }


template<>
  inline
  void swap<AnnotationInfo>(AnnotationInfo& a, AnnotationInfo&b)
  {
    //    std::cout << "called1" << std::endl;
    std::swap(a.inside_probabilities, b.inside_probabilities);
    std::swap(a.outside_probabilities, b.outside_probabilities);
  }
}


#endif /* _ANNOTATIONINFO_H_ */
