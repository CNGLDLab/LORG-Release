// -*- mode: c++ -*-
#ifndef TWOSTAGELORGPARSEAPP_H
#define TWOSTAGELORGPARSEAPP_H

#include "LorgParseApp.h"

#include "parsers/ParserCKYAll.h"
#include "utils/Tagger.h"

#include "feature_extract/Extract.h"


class TwoStageLorgParseApp : public LorgParseApp
{
public:
    TwoStageLorgParseApp();
    ~TwoStageLorgParseApp();
    int run();
    
private:
    bool read_config(ConfigTable& configuration);
    LorgOptions get_options() const;
    
    ParserCKYAll * parser;
    Tagger tagger;
    bool output_annotations;
    unsigned kbest;
    
    bool extract_features;
};

#include "utils/PtbPsTree.h"
#include "ParseSolution.h"
#include "utils/LorgConstants.h"

#include "parsers/ParserCKYAllFactory.h"

#include <tbb/tick_count.h>
using namespace tbb;

TwoStageLorgParseApp::TwoStageLorgParseApp() : LorgParseApp(), parser(NULL) {}

TwoStageLorgParseApp::~TwoStageLorgParseApp()
{
    if(parser) delete parser;
}

int TwoStageLorgParseApp::run()
{
    parse_solution::init_feature_extractor();
    
    
    if(verbose)  std::clog << "Start parsing process.\n";
    
    std::string raw_sentence; // the sentence string (only for pretty print solutions)
    std::vector<Word> sentence; // the vector of words used by the parser to initialise the chart
    std::vector< bracketing > brackets; // pre-bracketting for guided parsing
    std::vector<std::string> comments; // strings to store comments associated with a sentence
    int count = 0; // sentence count (debug & pretty-print)
    
    int start_symbol = SymbolTable::instance_nt().get(LorgConstants::tree_root_name); // axiom of the grammar
    tick_count parse_start = tick_count::now();
    
    std::vector<std::pair<PtbPsTree *,double> > best_trees; // vector of (tree,score)
        
    //read input and fill raw_sentence, sentence and brackets
    while(tokeniser->tokenise(*in, raw_sentence, sentence, brackets, comments)) {
        
        tick_count sent_start = tick_count::now();
        
        if(sentence.size() <=  max_length && sentence.size() > 0) {
            
            /*Extra verbose
            if(verbose) {
                std::clog << "Tokens: ";
                for(std::vector<Word>::const_iterator i(sentence.begin()); i != sentence.end(); ++i)
                    std::clog << "<" << i->get_form() << ">";
                std::clog << "\n";
            }*/
            
            //tag sentence
            tagger.tag(sentence);
            
            // create and initialise chart
            parser->initialise_chart(sentence, brackets);
            
            // parse, aka create the coarse forest
            parser->parse(start_symbol);
            
            //use intermediate grammars to prune the chart
            parser->beam_c2f(start_symbol);
            
            // extract best solution with the finest grammar
            if(parser->is_chart_valid(start_symbol))
                parser->extract_solution();
            if(parser->is_chart_valid(start_symbol))
                parser->get_parses(start_symbol, kbest, always_output_forms, output_annotations, best_trees);
        }
        
        *out << parse_solution(raw_sentence, ++count, sentence.size(), best_trees,
                               (verbose) ? (tick_count::now() - sent_start).seconds() : 0,
                               verbose, comments, extract_features);
        
        ///*
        if(verbose && count % 50 == 0)
            std::cout << count << " parsed sentences in " << (tick_count::now() - parse_start).seconds() << " sec" << std::endl;
        //*/
        
        //sanity
        for(unsigned i = 0; i < best_trees.size(); ++i) { // delete solutions
            delete best_trees[i].first;
        }
        best_trees.clear();
        sentence.clear();
        brackets.clear();
        comments.clear();
        
        parser->clean();
        
    }
    
    *out << std::flush;
    
    if(verbose) std::clog << "overall time: " << (tick_count::now() - parse_start).seconds() << "s" << std::endl;    
    return 0; //everything's fine
}

LorgOptions TwoStageLorgParseApp::get_options() const
{
    LorgOptions options(LorgParseApp::get_options());
    options.add_2sparser_options();
    return options;
}



bool TwoStageLorgParseApp::read_config(ConfigTable& configuration)
{
    if(LorgParseApp::read_config(configuration) == false) return false;
    
    output_annotations = configuration.get_value<bool>("output-annotations");
    
    if(verbose) { std::clog << "creating the parser... ";}
    if((parser = ParserCKYAllFactory::create_parser(configuration)) == NULL) return false;
    if(verbose) {std::clog << "ok" << std::endl;}
    
    kbest = configuration.get_value<unsigned>("kbest");
    
    //creating tagger
    tagger.set_word_rules(&(parser->get_words_to_rules()));
    
    
    extract_features = configuration.get_value<bool>("extract-features");
    
    
    return true;
}



#endif // TWOSTAGELORGPARSEAPP_H
