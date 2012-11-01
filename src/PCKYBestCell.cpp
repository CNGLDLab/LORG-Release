#include "PCKYBestCell.h"
#include "utils/SymbolTable.h"
#include<cmath>

unsigned PCKYBestCell::max_size = 0;

PCKYBestCell::PCKYBestCell(bool cl):
  closed(cl), word_edge(NULL)
{
  if(closed)
    real_cell = NULL;
  else {
    real_cell =  new Edge * [max_size];
    memset(real_cell, 0, max_size * sizeof(Edge*));
    //for(unsigned i = 0; i < max_size;++i) real_cell[i]=NULL;
  }
}

PCKYBestCell::~PCKYBestCell()
{
  if(!closed) {
    for(unsigned i = 0; i < max_size;++i) delete real_cell[i];
    delete real_cell;
    delete word_edge;
  }
}


std::ostream& operator<<(std::ostream& out, const PCKYBestCell& cell)
{
  if(cell.is_closed())
    out << "cell closed\n";
  else {
    for(unsigned i = 0; i < cell.max_size; ++i) {
      Edge * edge = cell.real_cell[i];
      if(edge) {
	out << SymbolTable::instance_nt().translate(edge->get_lhs()) << ": "
	    << std::exp(edge->get_probability()) << "\n";
      }
    }
  }
  
  return out;
}

