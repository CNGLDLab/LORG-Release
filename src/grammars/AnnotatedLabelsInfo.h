// -*- mode: c++ -*-
#ifndef ANNOTATEDLABELSINFO_H_
#define ANNOTATEDLABELSINFO_H_

#include "rules/BRule.h"
#include "rules/URule.h"
#include "rules/LexicalRule.h"

#include <vector>

class AnnotatedLabelsInfo
{
private:
  std::vector<unsigned short> num_annotations_map;///< maps a non-terminal label to its number of annotations
public:
  AnnotatedLabelsInfo();
  AnnotatedLabelsInfo(const std::vector< BRule>& brules,
		      const std::vector< URule>& urules,
		      const std::vector< LexicalRule>& lrules);

  ~AnnotatedLabelsInfo();

  /**
     \brief initialise the annotations map by
     \setting every unannotated non terminal label to have just one annotation.
     \  This function also sets the number_of_unannotated labels. The non terminal labels are integers from zero to
     \ the number of labels in the grammar -1.  The total number of non-terminal labels
     \ is taken from the symbol table.  So the symbol table must be initialised before this function is called.
  */
  void initialise();

  /**
     \brief returns the number of annotations associated with this non-terminal label.
     \param the integer id of a non-terminal label.
     \return the number of annotations that this non-terminal label has.
  */
  unsigned short get_number_of_annotations(short label) const;

  /**
    \brief set the symbol-annotation# map
     \param map the map
   */
  void set_num_annotations_map(const std::map<short,unsigned short>& map);

  unsigned short& operator[](short label);




  /**
     \brief sets the number of annotations to be associated with this non-terminal label.
     \if the label is not already in the map, it will create a new entry for the label.
     \param the integer id of a non-terminal label.
     \param the number of annotations for this label.
  */
  void set_number_of_annotations(short label, unsigned short number_of_annotations);
  void set_num_annotations_map(const std::vector<unsigned short>& la);

  /**
     \brief returns the total number of non-terminal labels in the grammar.
     \return the total number of non-terminal labels in the grammar.
  */
  unsigned short get_number_of_unannotated_labels() const;


  void initialise_from_rules(const std::vector< BRule>& brules,
			     const std::vector< URule>& urules,
			     const std::vector< LexicalRule >& lrules);

    /**
     \brief Output operator
     \param out the ostream to write on
     \param ali the object to write
     \return the used ostream
  */
  friend std::ostream& operator<<(std::ostream& out, const AnnotatedLabelsInfo& ali);

};


inline
unsigned short AnnotatedLabelsInfo::get_number_of_annotations(short label) const
{
  assert(label >= 0);
  assert(label < (int) num_annotations_map.size());

  return num_annotations_map[label];
}

inline
unsigned short AnnotatedLabelsInfo::get_number_of_unannotated_labels() const
{return num_annotations_map.size();}

inline
unsigned short& AnnotatedLabelsInfo::operator[](short label)
{
  return num_annotations_map[label];
}

inline
void AnnotatedLabelsInfo::set_num_annotations_map(const std::map<short,unsigned short>& map)
{
  for(std::map<short, unsigned short>::const_iterator it(map.begin());
      it != map.end(); ++it) {

    if(it->first >= (short) num_annotations_map.size())
      num_annotations_map.resize(it->first + 1 ,0);

    num_annotations_map[it->first] = it->second;
  }
}

inline
void AnnotatedLabelsInfo::set_num_annotations_map(const std::vector<unsigned short>& la)
{
  num_annotations_map = la;
}



inline
void AnnotatedLabelsInfo::set_number_of_annotations(short label, unsigned short number_of_annotations){

  num_annotations_map[label]=number_of_annotations;}

class AnnotatedContents
{
protected:
  AnnotatedLabelsInfo label_annotations;///< information on the number of annotations per non-terminal label
  /**
     \brief calls initialise function for label_annotations, which initialises the annotations map by
     \ setting every unannotated non terminal label to have just one annoation.
     \ This function also sets the number_of_unannotated labels. The non terminal labels are integers from zero to
     \ the number of labels in the grammar -1.  The total number of non-terminal labels
     \ is taken from the symbol table.  So the symbol table must be initialised before this function is called.
  */
  void initialise_annotations_map();

public:
  AnnotatedContents();
  virtual ~AnnotatedContents();

  /**
     \brief functions which returns the annotation information object for this grammar.
     \return the annotation information object for this grammar
  */
  const AnnotatedLabelsInfo& get_annotations_info() const;
};

inline
const AnnotatedLabelsInfo&  AnnotatedContents::get_annotations_info()const {return label_annotations;}


#endif /*ANNOTATEDLABELSINFO_H_*/
