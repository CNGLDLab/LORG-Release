// -*- mode: c++ -*-
#ifndef PARSESOLUTION_H
#define PARSESOLUTION_H

#include "utils/PtbPsTree.h"
#include "feature_extract/Extract.h"


/////////////////////////////////////
template <typename IdentifierType,
          class IgnoredAbstractProduct>
class DefaultFactoryError
{
public:
  class Exception : std::exception
  {
  public:
    Exception(const IdentifierType& id) : unknown_id(id) {};

    const IdentifierType& get_id() {return unknown_id;};

    virtual const char * what() { return "Unknown Object Type passed to Factory";};

    ~Exception() throw() {};


  private:
    IdentifierType unknown_id;
  };

protected:
  IgnoredAbstractProduct * on_unknown_type_id(const IdentifierType&id)
  {
    throw Exception(id);
  };
};


template <class AbstractProduct,
          typename IdentifierType,
          typename DataIn,
          typename ProductCreator = AbstractProduct* (*)(const DataIn&),
          template <typename, class> class FactoryErrorPolicy = DefaultFactoryError
          >
class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
{
public:
  bool register_type(const IdentifierType& id, ProductCreator creator)
   {
    return assoc.insert(typename AssocMap::value_type(id,creator)).second;
   };

  bool unregister_type(const IdentifierType& id)
  {
    return assoc.erase(id) == 1;
  };

  AbstractProduct* create_object(const IdentifierType& id, const DataIn& data)
  {
    typename AssocMap::const_iterator i = assoc.find(id);
    if(i != assoc.end())
      {
        return (i->second)(data);
      }
    else
      {
        return this->on_unknown_type_id(id);
      }
  };

 private:
   typedef std::map<IdentifierType, ProductCreator> AssocMap;
   AssocMap assoc;
 };

////////////////////////////////////

class ParseSolutionFactory;

struct parse_solution
{
  enum parse_solution_format { UNIX, JSON};


  const std::string& sentence;
  int id_sentence;
  unsigned length;
  std::vector<std::pair<PtbPsTree *, double> >& trees;
  double time;
  bool verbose;

  const std::vector<std::string>& comments;
  bool extract_features;


  static parse_solution_format format_from_string(const std::string&);

  static Extract extractor;
  static void init_feature_extractor();
  static ParseSolutionFactory factory;

  virtual ~parse_solution() {};


  virtual std::ostream& print(std::ostream& out) const {return out;};


  parse_solution(const std::string& s, int id, unsigned l, std::vector<std::pair<PtbPsTree*, double> >& trs, const double& ti, bool ve, const std::vector<std::string>& co, bool ef)
    : sentence(s), id_sentence(id), length(l), trees(trs), time(ti), verbose(ve), comments(co), extract_features(ef)
  {};

  parse_solution(const parse_solution& other)
    : sentence(other.sentence),
      id_sentence(other.id_sentence),
      length(other.length),
      trees(other.trees),
      time(other.time),
      verbose(other.verbose),
      comments(other.comments),
      extract_features(other.extract_features)
  {};

};


  parse_solution::parse_solution_format
  parse_solution::format_from_string(const std::string& s)

  {
    if (s == "unix")
      return UNIX;
    if (s == "json")
      return JSON;

    throw std::out_of_range(s);
  }


Extract parse_solution::extractor;

void parse_solution::init_feature_extractor()
{
    std::vector<Feature> v;
    v.push_back(RuleFeature());
    v.push_back(RuleParentFeature());
    v.push_back(RuleGrandParentFeature());
    v.push_back(BiGramNodeFeature());
    v.push_back(HeavyFeature());
    v.push_back(NeighboursFeature());
    v.push_back(NeighboursExtFeature());
    v.push_back(WordFeature2());
    v.push_back(WordFeature3());
    v.push_back(WordFeatureGen2());
    v.push_back(WordFeatureGen3());

    extractor = Extract(v);
}


  class ParseSolutionFactory : public Factory<parse_solution,
                                              int,
                                              parse_solution>
{

};


ParseSolutionFactory parse_solution::factory = ParseSolutionFactory();


struct unix_parse_solution : parse_solution
{
  unix_parse_solution(const parse_solution& p) : parse_solution(p) {};


  static parse_solution * create(const parse_solution&p)
  {
    return new unix_parse_solution(p);
  }

  static bool init()
  {
    return parse_solution::factory.register_type(UNIX, create);
  }

  std::ostream& print(std::ostream& out) const;
};

inline
std::ostream& operator<<(std::ostream& out, const unix_parse_solution& ps)
{
  out << "### ID: "       << ps.id_sentence << '\n';

  if(ps.verbose)
    out << "### length: "   << ps.length << '\n'
	<< "### time: "     << ps.time << '\n'
        << "### solutions: " << ps.trees.size() << '\n';
  out << "### sentence: " << ps.sentence << '\n';

    for(unsigned i = 0; i < ps.comments.size(); ++i) {
        out << "### comment: " << ps.comments[i] << '\n';
    }

    if(ps.sentence.size() == 0){ out << "empty sentence\n";return out;}
    if(ps.trees.empty())  out << "no parse found\n";



    for(unsigned i = 0; i < ps.trees.size(); ++i) {
        if (ps.trees[i].first == NULL ) {
            out << "no parse found\n";
            if(ps.extract_features)
                out << "### features:\n";
        }
        else {
            ps.trees[i].first->unbinarise();
            if(ps.verbose)
                out << ps.trees[i].second << " : " ;
            out << *(ps.trees[i].first) << '\n' ;
            if(ps.extract_features) {
                std::string s;
                parse_solution::extractor.extract(*(ps.trees[i].first),s);
                out << "### features: " << s << '\n' ;
            }
        }
    }

    return out;
    }


std::ostream& unix_parse_solution::print(std::ostream& out) const
{
  return out << *this;
}



struct json_parse_solution : parse_solution
{
  json_parse_solution(const parse_solution& p) : parse_solution(p) {};

  static parse_solution * create(const parse_solution&p)
  {
    return new json_parse_solution(p);
  }

  static bool init()
  {
    return parse_solution::factory.register_type(JSON, create);
  }

  std::ostream& print(std::ostream& out) const;

};

#include <sstream>

std::string encode_for_json( const std::string &src )
{
    std::ostringstream sret;

    for(std::string::const_iterator iter = src.begin(); iter!=src.end(); ++iter)
    {
         unsigned char c = (unsigned char)*iter;

         switch( c )
         {
// other cases ?
             case '"': sret << "\\\""; break;
             default: sret << c;
         }
    }

    return sret.str();
}


inline
std::ostream& operator<<(std::ostream& out, const json_parse_solution& ps)
{
  out << "{\n"
      << "  \"id\": "        << ps.id_sentence << ",\n"
      << "  \"length\": "    << ps.length << ",\n"
      << "  \"time\": "      << ps.time << ",\n"
      << "  \"solutions\": " << ps.trees.size() << ",\n"
      << "  \"sentence\": \""  << encode_for_json(ps.sentence) << "\",\n";

  out << "  \"comments:\" [" << "\n";
  for(unsigned i = 0; i < ps.comments.size(); ++i) {
     out << "    {\n";
     out << "      \"comment\": " << encode_for_json(ps.comments[i]) << '\n';
     out << "    }\n";
    if (i != ps.comments.size() - 1)
      out << "    ,\n";
  }
  out << "  ]" << "\n";

  out << "  \"parses:\" [" << "\n";
  for (unsigned i = 0; i < ps.trees.size(); ++i) {

    out << "    \"parse\": {" << "\n";
    if (ps.trees[i].first == NULL ) {
        out << "      \"tree\": \"\"\n"
            << "      \"score\": \"-infinity\"\n"
            << "      \"features\": \"\"\n";
    }
    else {
       std::ostringstream stream;
       ps.trees[i].first->unbinarise();
       stream << *(ps.trees[i].first);

       std::string s = "";
       if(ps.extract_features)
         parse_solution::extractor.extract(*(ps.trees[i].first),s);

       out << "      \"tree\": \"" << encode_for_json(stream.str()) << "\"\n"
           << "      \"score\": " << ps.trees[i].second << "\n"
           << "      \"features\": \"" << encode_for_json(s) << "\"\n";
    }
    out << "    }" << "\n";

    if (i != ps.trees.size() - 1)
      out << "    ,\n";
  }
  out << "  ]" << "\n";

  out << "}\n";
  return out;
}


std::ostream& json_parse_solution::print(std::ostream& out) const
{
  return out << *this;
}

#endif //PARSESOLUTION_H
