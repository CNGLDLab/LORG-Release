// -*- mode: c++ -*-
#ifndef _PARSERCKYALLFACTORY_H_
#define _PARSERCKYALLFACTORY_H_

#include "utils/ConfigTable.h"
#include "ParserCKYAll.h"

#include "ParserCKYAllFactory.h"

#include "ParserCKYAllViterbi.h"
#include "ParserCKYAllMaxVar.h"
#include "ParserCKYAllMaxVarKB.h"
#include "ParserCKYAllMaxVarMultiple.h"

#include "utils/data_parsers/AnnotHistoriesParser.h"


namespace ParserCKYAllFactory {
    enum Parsing_Algorithm {MaxRule, Viterbi, MaxN, KMaxRule};

    Parsing_Algorithm string_to_pa(const std::string&);

    ParserCKYAll * create_parser(ConfigTable& config);
}


namespace {

    ///////////
    // build the mapping 'gram_idx->nt_symb->annot->annots in gram_idx + 1'
    // needed for c2f pruning
    ///////////

    annot_descendants_type
    create_annot_descendants(const std::vector< Tree<unsigned> >& annot_histories)
    {
        annot_descendants_type result;

        unsigned height = 0;
        for (unsigned i = 0; i < annot_histories.size(); ++i)
        {
            if(annot_histories[i].height() > height)
                height = annot_histories[i].height();
        }
        //    std::clog << height << std::endl;

        result.resize(height);

        for (unsigned nt_idx = 0; nt_idx < annot_histories.size(); ++nt_idx)
        {
            //      std::clog << annot_histories[nt_idx] << std::endl;

            for(unsigned gram_idx = 0; gram_idx < height; ++gram_idx)
            {
                //	  std::clog << gram_idx << " : " << std::endl;
                result[gram_idx].resize(annot_histories.size());

                std::vector<Tree<unsigned>::const_depth_first_iterator> subs = annot_histories[nt_idx].get_at_depth(gram_idx);
                result[gram_idx][nt_idx].resize(subs.size());

                for(std::vector<Tree<unsigned>::const_depth_first_iterator>::const_iterator i(subs.begin());
                    i != subs.end(); ++i) {

                    std::vector<unsigned> descs;
                    Tree<unsigned>::const_depth_first_iterator copy = *i;
                    copy.down_first();
                    while(copy != annot_histories[nt_idx].dfend())
                    {
                        //		std::clog << '\t' << gram_idx << "->" << nt_idx << "->" << **i << "->" << *copy << std::endl;
                        descs.push_back(*copy);
                        copy.right();
                    }
                    result[gram_idx][nt_idx][**i]=descs;

                }
            }
        }

        return result;
    }


    struct smooth_helper
    {
        double threshold;
        smooth_helper(double d) : threshold(d) {};

        template <typename T>
        void operator()(T& rule) const
        {
            rule.linear_smooth(threshold);
        }
    };

    void smooth_grammar(ParserCKYAll::AGrammar& grammar, double internal_threshold, double lexical_threshold)
    {
        std::for_each(grammar.binary_rules.begin(), grammar.binary_rules.end(), smooth_helper(internal_threshold));
        std::for_each(grammar.unary_rules.begin(), grammar.unary_rules.end(), smooth_helper(internal_threshold));
        std::for_each(grammar.lexical_rules.begin(), grammar.lexical_rules.end(), smooth_helper(lexical_threshold));
    }



    std::vector<ParserCKYAll::AGrammar*>
    create_intermediates(ParserCKYAll::AGrammar& grammar, const annot_descendants_type& annot_descendants)
    {
        std::vector<ParserCKYAll::AGrammar*> result;

        //    std::clog << "before transition" << std::endl;
        uomap<int, uomap<unsigned, uomap<int, uomap<unsigned, double > > > >transition_probabilities;
        grammar.compute_transition_probabilities(transition_probabilities);
        //    std::clog << "after transition" << std::endl;

        //    std::clog << "before expected counts" << std::endl;
        std::vector<std::vector<double> > expected_counts;
        calculate_expected_counts(transition_probabilities, grammar.get_annotations_info(), expected_counts);
        //    std::clog << "after expected counts" << std::endl;

        for (unsigned i = 0; i < annot_descendants.size() - 1; ++i) {

            //      std::clog << "before mapping " << i << std::endl;
            std::vector<std::vector<std::vector<unsigned> > > annotation_mapping = compute_mapping(i, annot_descendants.size() - 1, annot_descendants);
            //      std::clog << "after mapping " << i << std::endl;

            //      std::clog << "before create_projection" << std::endl;
            ParserCKYAll::AGrammar * cg = grammar.create_projection(expected_counts, annotation_mapping);
            //      std::clog << "after create_projection" << std::endl;


            //      std::clog << "finishing creation" << std::endl;
            //add unary chains
            //      std::clog << "init" << std::endl;
            cg->init();
            //smooth
            //      std::clog << "smoothing" << std::endl;
            smooth_grammar(*cg,0.01,0.1);
            // remove zeros
            //      std::clog << "removing zeros" << std::endl;
            cg->remove_unlikely_annotations_all_rules(1e-6);
            // done!
            result.push_back(cg);
            //      std::clog << "creation finished" << std::endl;
        }

        return result;
    }
}



ParserCKYAllFactory::Parsing_Algorithm
ParserCKYAllFactory::string_to_pa(const std::string& s)
{
    if (s == "max") return ParserCKYAllFactory::MaxRule;
    if (s == "vit") return ParserCKYAllFactory::Viterbi;

    if (s == "maxn") return ParserCKYAllFactory::MaxN;
    if (s == "kmax") return ParserCKYAllFactory::KMaxRule;

    std::clog << "Unknown parser type: " << s << std::endl;

    throw std::out_of_range(s);
}


ParserCKYAll * create_parser(std::vector<ParserCKYAll::AGrammar*> cgs, ParserCKYAllFactory::Parsing_Algorithm pt,
                             const std::vector<double>& p, double b_t,
                             const std::vector< std::vector<ParserCKYAll::AGrammar*> >& fgs,
                             const std::vector< annot_descendants_type >& all_annot_descendants,
                             bool accurate, unsigned min_beam, int stubborn,
                             unsigned k, unsigned cell_threads)
{
    using namespace ParserCKYAllFactory;
    switch(pt) {
        case Viterbi :
            return new ParserCKYAllViterbi(cgs, p, b_t, all_annot_descendants[0], accurate, min_beam, stubborn, cell_threads);
            break;
        case MaxRule :
            return new ParserCKYAllMaxRule(cgs, p, b_t, all_annot_descendants[0], accurate, min_beam, stubborn, cell_threads);
            break;
        case MaxN :
            return new ParserCKYAllMaxRuleMultiple(cgs, p, b_t, fgs, all_annot_descendants, accurate, min_beam, stubborn, k, cell_threads);
            break;
        case KMaxRule :
            return new ParserCKYAllMaxRuleKB(cgs, p, b_t, all_annot_descendants[0], accurate, min_beam, stubborn, k, cell_threads);
            break;
        default :
            return NULL;
    }

    return NULL;
}


ParserCKYAll * ParserCKYAllFactory::create_parser(ConfigTable& config)
{
    bool verbose = config.exists("verbose");

    std::vector<double> priors;
    std::vector<annot_descendants_type> all_annot_descendants;

    std::vector<ParserCKYAll::AGrammar*> grammars;


    // get grammars
    if(config.exists("grammar")) {
        const std::string& filename = config.get_value< std::string >("grammar");


        // compute expected intermediate grammars

        //get grammar
        if(verbose) std::cerr << "Setting grammar to " << filename << ".\n";
        ParserCKYAll::AGrammar * cg = new ParserCKYAll::AGrammar(filename);
        if(verbose) std::cerr << "Grammar set\n";

        //perform some sanity checks
        if(cg->get_history_trees().empty() ||
           cg->get_annotations_info().get_number_of_unannotated_labels() != cg->get_history_trees().size()) {

            std::cerr << "Problem in the grammar file." << std::endl
            << "Annotation history and number of annotations are inconsistent." << std::endl
            << "Aborting now !" << std::endl;
            delete cg;
            exit(1);
        }


        annot_descendants_type annot_descendants = create_annot_descendants(cg->get_history_trees());
        all_annot_descendants.push_back(annot_descendants);


        if(verbose)
          std::clog << "create intermediate grammars" << std::endl;
        grammars = create_intermediates(*cg, annot_descendants);
        // compute priors for base grammar
        priors = grammars[0]->compute_priors();
        // for(unsigned j = 0; j < priors.size(); ++j) {
        //   std::clog << SymbolTable::instance_nt()->get_label_string(j)
        // 		<< " -> " << priors[j] << std::endl;
        // }


        //      std::clog << "finishing creation" << std::endl;
        //    std::clog << "init" << std::endl;
        cg->init();
        //    std::clog << "smooth" << std::endl;
        smooth_grammar(*cg,0.01,0.1);
        //    std::clog << "clean" << std::endl;
        cg->remove_unlikely_annotations_all_rules(1e-10);
        grammars.push_back(cg);
        //    std::clog << "creation finished" << std::endl;

    }
    else {
        std::cerr << "Grammar wasn't set. Exit program." << std::endl;
        return NULL;
    }


    std::vector< std::vector<ParserCKYAll::AGrammar*> > alt_gs;
    ParserCKYAll::AGrammar* gram_ptr;


    // get fine grammar
    if(config.exists("alternate-grammar")) {

        const std::vector<std::string>& filenames = config.get_value<std::vector<std::string> >("alternate-grammar");

        if(string_to_pa(config.get_value<std::string>("parser-type")) != MaxN && filenames.size() > 0) {
            std::cerr << "Wrong parsing algorithm. Exit program." << std::endl;
            return NULL;
        }

        //    std::vector<std::string> filenames_ahi = config.get_value< std::vector<std::string> >("annot-histories");

        for(unsigned i = 0; i < filenames.size(); ++i) {

            if(verbose) std::cerr << "Setting alternate grammar to " << filenames[i] << ".\n";

            gram_ptr = new ParserCKYAll::AGrammar(filenames[i]);

            annot_descendants_type ad = create_annot_descendants(gram_ptr->get_history_trees());
            all_annot_descendants.push_back(ad);
            alt_gs.push_back(create_intermediates(*gram_ptr, ad));

            gram_ptr->init();
            smooth_grammar(*gram_ptr,0.01,0.1);
            gram_ptr->remove_unlikely_annotations_all_rules(1e-10);
            alt_gs[i].push_back(gram_ptr);

        }
    }

    double beam_threshold = config.get_value<double>("beam-threshold");
    if(verbose)
        std::clog << "using threshold " << beam_threshold << " for chart construction" << std::endl;

    bool accurate = config.exists("accurate");

    if(verbose) {
        if(accurate)
            std::clog << "using accurate c2f thresholds" << std::endl;
        else
            std::clog << "using standard c2f thresholds" << std::endl;
    }


    unsigned min = config.get_value<unsigned>("min-length-beam");

    unsigned nbthreads = config.get_value<unsigned>("nbthreads");
    if(verbose){
        std::clog << "using " << nbthreads << " threads to parse" << std::endl;
    }

    return create_parser(grammars,
                         string_to_pa(config.get_value<std::string>("parser-type")),
                         priors, beam_threshold,
                         alt_gs, all_annot_descendants, accurate, min,
                         config.get_value<int>("stubbornness"),
                         config.get_value<unsigned>("kbest"), nbthreads);

}




#endif /* _PARSERCKYALLFACTORY_H_ */
