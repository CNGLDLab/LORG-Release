// -*- mode: c++ -*-
#ifndef PARSESOLUTION_H
#define PARSESOLUTION_H

#include "utils/PtbPsTree.h"
#include "feature_extract/Extract.h"

struct parse_solution
{
    const std::string& sentence;
    int id_sentence;
    unsigned length;
    std::vector<std::pair<PtbPsTree *, double> > trees;
    double time;
    bool verbose;
    
    std::vector<std::string> comments;
    
    bool extract_features;
    
    static Extract extractor;
    static void init_feature_extractor();
    
    parse_solution(const std::string& s, int id, unsigned l,PtbPsTree* tr, double score, const double& ti, bool ve)
    : sentence(s), id_sentence(id), length(l), trees(1,std::make_pair(tr,score)), time(ti), verbose(ve),
    comments(), extract_features(false)
    {};
    
    parse_solution(const std::string& s, int id, unsigned l, std::vector<std::pair<PtbPsTree*, double> >& trs, const double& ti, bool ve, const std::vector<std::string>& co, bool ef)
    : sentence(s), id_sentence(id), length(l), trees(trs), time(ti), verbose(ve), comments(co), extract_features(ef)
    {};
};


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


inline
std::ostream& operator<<(std::ostream& out, const parse_solution& ps)
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
    
#endif //PARSESOLUTION_H
