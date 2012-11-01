// -*- mode: c++ -*-
#ifndef _METAPRODUCTION_H_
#define _METAPRODUCTION_H_

/**
   \class MetaProduction : common ancestor for all
   classes dealing with rules
   The only thing common to all these classes is the lhs !
 */
class MetaProduction {

public:
  MetaProduction() : lhs(-1) {};
  MetaProduction(short l) : lhs(l) {};
  virtual ~MetaProduction() {};

  /**
     \brief Gets the current left hand side symbol of the production
     \return Left hand side symbol
  */
  short get_lhs() const {return lhs;}

  /**
     \brief Sets left hand side symbol of the production
     \param[in] l New left hand side symbol
  */
  void set_lhs(short l) {lhs = l;};

protected:
  short lhs;                              ///< left hand side symbol

};

#endif /* _METAPRODUCTION_H_ */
