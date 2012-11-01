#include "AnnotatedLabelsInfo.h"

#include "utils/SymbolTable.h"

#include <algorithm>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

namespace phx = boost::phoenix;

AnnotatedLabelsInfo::AnnotatedLabelsInfo() : num_annotations_map() {}

AnnotatedLabelsInfo::AnnotatedLabelsInfo(const std::vector< BRule >& brules, 
					 const std::vector< URule >& urules, 
					 const std::vector< LexicalRule >& lrules)
  : num_annotations_map()
{
  initialise_from_rules(brules, urules, lrules);
}


AnnotatedLabelsInfo::~AnnotatedLabelsInfo() {}

void AnnotatedLabelsInfo::initialise(){
  //initialise number of annotations per label to 1
  num_annotations_map = std::vector<unsigned short>(SymbolTable::instance_nt().get_symbol_count(),1);
}



// using boost phoenix to build
// a pair <label, num of annotations>
// maybe an overkill ...
struct annot_impl
{
  template <typename MyRule>
  struct result
  {
    typedef std::pair<int,unsigned> type;
  };
  
  template <typename MyRule>
  std::pair<int,unsigned> operator()(const MyRule& rule) const
  {
    return std::make_pair<int,int>(rule.get_lhs(), rule.get_probability().size());
  }
};

// making a functor from the structure to be used with for_each
phx::function<annot_impl> annot;

void AnnotatedLabelsInfo::initialise_from_rules(const std::vector< BRule >& brules, 
						const std::vector< URule >& urules, 
						const std::vector< LexicalRule >& lrules)
{
  std::map<short ,unsigned short> map;

  std::for_each(brules.begin(),brules.end(),
		phx::insert(phx::ref(map), annot(phx::arg_names::_1))
		);

  std::for_each(urules.begin(),urules.end(),
		phx::insert(phx::ref(map), annot(phx::arg_names::_1))
		);

  std::for_each(lrules.begin(),lrules.end(),
		phx::insert(phx::ref(map), annot(phx::arg_names::_1))
		);

  set_num_annotations_map(map);  
}

std::ostream& operator<<(std::ostream& out, const AnnotatedLabelsInfo& ali)
{
  for(unsigned i = 0; i < ali.get_number_of_unannotated_labels(); ++i)
    out << "ainfos " << SymbolTable::instance_nt().translate(i) 
	<< " " << ali.get_number_of_annotations(i) << std::endl;
  return out;
}



AnnotatedContents::AnnotatedContents() {}

AnnotatedContents::~AnnotatedContents() {}

void AnnotatedContents::initialise_annotations_map()
{
  label_annotations.initialise();
}
