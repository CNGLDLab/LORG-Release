#include <sstream>
//#include <cfloat>
#include "Edge.h"

#include "utils/SymbolTable.h"

Edge::~Edge()
{}

Edge::Edge(): lhs(), left(NULL), right(NULL), probability(), lex(false){}

Edge::Edge(int l,const Edge * r, const double& p):
  lhs(l), left(r), right(NULL), probability(p), lex(false)
{}

Edge::Edge(int l,const Edge * r1,const Edge * r2, const double& p):
  lhs(l), left(r1), right(r2), probability(p), lex(false)
{}

Edge::Edge(int lh, const double& p, bool le):
  lhs(lh), left(NULL), right(NULL), probability(p), lex(le)
{}


Edge::Edge(const Edge& e) :
  lhs(e.lhs), left(e.left), right(e.right), probability(e.probability),lex(e.lex)
{}


PtbPsTree* Edge::to_ptbpstree(int /*not read*/, unsigned /*not read*/, bool /*not read*/, bool /*not read*/) const
{
  PtbPsTree * tree = new PtbPsTree(SymbolTable::instance_nt().translate(get_lhs()));
  PtbPsTree::depth_first_iterator pos = tree->dfbegin();

  if(left)
    left->to_ptbpstree(*tree, pos);
  if(right)
    right->to_ptbpstree(*tree, pos);
  return tree;
}

void Edge::to_ptbpstree(PtbPsTree& tree, PtbPsTree::depth_first_iterator& pos) const
{
  if(lex){
	     //std::cout << "lex: " << get_lhs() << std::endl;
	     //std::cout << SymbolTable::instance_word()->translate(get_lhs()) << std::endl;
    pos = tree.add_last_daughter(pos, SymbolTable::instance_word().translate(get_lhs()));


    // // dont bother, words will be rewritten anyway
    // pos = tree.add_last_daughter(pos, "");
  }
  else{
    //	 std::cout << SymbolTable::instance_nt()->translate(get_lhs()) << std::endl;
    //std::cout << "int: " << get_lhs() << std::endl;
    //std::cout << "probability: " << get_probability() << std::endl;
    pos = tree.add_last_daughter(pos, SymbolTable::instance_nt().translate(get_lhs()));
   }
  if(!is_terminal()) {
    if(left)
      left->to_ptbpstree(tree, pos);
    if(right)
      right->to_ptbpstree(tree, pos);
  }
  // go up because you processed the last daughter
  pos.up();
}


std::ostream& operator<<(std::ostream& out, const Edge& edge)
{
  if(!edge.lex) {
    out << SymbolTable::instance_nt().translate(edge.lhs) << "[" << edge.probability << "] -> ( (";

    if(edge.left)
      out << *edge.left << ") ";
    if(edge.right)
      out << *edge.right << ") ";
  out << ")";
  }
  else
    out << "Edge: " << SymbolTable::instance_word().translate(edge.lhs) <<  "[" << edge.probability << "]";
  return out;
}


Edge::Edge(PtbPsTree& tree) : lhs(), left(NULL), right(NULL), probability(), lex(false)
{
  PtbPsTree::depth_first_iterator r_iter = tree.dfbegin();
  *this = Edge(tree,r_iter);
}


// assume the ptbpstree is binarized !!!!!!!
// beware if the tree contains an unknown word, it will be added to the symboltable !!!
// (Maybe we should use symboletable::get instead of insert ?)
Edge::Edge(PtbPsTree& tree, PtbPsTree::depth_first_iterator current)
  : lhs(), left(NULL), right(NULL), probability(), lex(false)
{
  if (current->leaf()){
    std::cout << *current << std::endl;
    *this = Edge(SymbolTable::instance_word().insert(*current),0,true);
  }
  else {
    PtbPsTree::depth_first_iterator daughter_iter = current;
    daughter_iter.down_first();
    Edge * left = new Edge (tree, daughter_iter);
    daughter_iter.right();
    if (daughter_iter == tree.dfend())
      *this = Edge(SymbolTable::instance_nt().insert(*current),left,0);
    else {
      Edge * right = new Edge(tree, daughter_iter);
      *this = Edge(SymbolTable::instance_nt().insert(*current),left,right,0);
    }
  }
}
