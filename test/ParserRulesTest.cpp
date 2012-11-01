#include <boost/test/unit_test.hpp>

#include "rules/Production.h"
#include "rules/LexicalRule.h"
#include "rules/URule.h"
#include "rules/BRule.h"
#include "utils/data_parsers/BURuleInputParser.h"

using namespace std;
typedef vector<double> DoubleScalableVector;
typedef string::const_iterator iterator_type;
typedef burule_parser<iterator_type> parser;

BOOST_AUTO_TEST_SUITE(ProductionSuite)

BOOST_AUTO_TEST_CASE(ProductionTest){
    //Binary production
    vector<int> rhs;
    rhs.push_back(1);
    rhs.push_back(2);
    Production prod(0, rhs, false);

    BOOST_CHECK_EQUAL(prod.get_lhs(), 0);
    BOOST_CHECK_EQUAL(prod.get_rhs0(), 1);
    BOOST_CHECK_EQUAL(prod.get_rhs1(), 2);
    BOOST_CHECK_EQUAL(prod.is_lexical(), false);
    BOOST_CHECK_EQUAL(prod.is_unary(), false);
    BOOST_CHECK_THROW(prod.get_rhs(2), std::out_of_range);

    //Unary production
    rhs.pop_back();
    BOOST_CHECK_EQUAL(rhs.size(), 1);

    Production uprod(0, rhs, false);
    BOOST_CHECK_EQUAL(uprod.get_lhs(), 0);
    BOOST_CHECK_EQUAL(uprod.get_rhs0(), 1);
    BOOST_CHECK_EQUAL(uprod.is_lexical(), false);
    BOOST_CHECK_EQUAL(uprod.is_unary(), true);
    BOOST_CHECK_THROW(uprod.get_rhs(1), std::out_of_range);

    //Lexical production
    Production lprod(0, rhs, true);
    BOOST_CHECK_EQUAL(lprod.get_lhs(), 0);
    BOOST_CHECK_EQUAL(lprod.get_rhs0(), 1);
    BOOST_CHECK_EQUAL(lprod.is_lexical(), true);
    BOOST_CHECK_EQUAL(lprod.is_unary(), true);
    BOOST_CHECK_THROW(lprod.get_rhs(1), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LexicalRulesSuite)

BOOST_AUTO_TEST_CASE(LexicalRuleInstantiationTest){    
    int rhs0 = 12;

    vector<lexical_proba_info> probs;
    probs.push_back(lexical_proba_info(0, 10.5));
    probs.push_back(lexical_proba_info(1, 1.0));

    LexicalRule lex(10, rhs0, probs);

    //Test to see if the probs and probabilities vector
    //inside lex rule are the same
    DoubleScalableVector probs_double;
    probs_double.push_back(10.5);
    probs_double.push_back(1.0);
    DoubleScalableVector v = lex.get_probability();
    BOOST_CHECK_EQUAL_COLLECTIONS(probs_double.begin(), probs_double.end(),
                                  v.begin(), v.end());

    BOOST_CHECK_EQUAL(lex.get_lhs(), 10);
    BOOST_CHECK_EQUAL(lex.get_rhs0(), rhs0);
    BOOST_CHECK_EQUAL(lex.get_word(), rhs0);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(UnaryRuleSuite)

BOOST_AUTO_TEST_CASE(UnaryRuleInstantiationTest){
    int rhs0 = 1;
    vector<unary_proba_info> probs;
    probs.push_back(unary_proba_info(0, 0, 10.5));
    probs.push_back(unary_proba_info(1, 0, 1.0));

    URule u(10, rhs0, probs);

    //Test to see if the probs and probabilities vector
    //inside lex rule are the same
    vector< vector<double> > probs_double;
    probs_double.push_back(vector<double>(1, 10.5));
    probs_double.push_back(vector<double>(1, 1.0));
    vector< vector<double> > v = u.get_probability();

    BOOST_CHECK_EQUAL(probs_double.size(), v.size());

    vector<double> first = probs_double[0], first_urule = v[0];
    vector<double> second = probs_double[1], second_urule = v[1];

    BOOST_CHECK_EQUAL_COLLECTIONS(first.begin(), first.end(),
                                  first_urule.begin(), first_urule.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(second.begin(), second.end(),
                                  second_urule.begin(), second_urule.end());

    BOOST_CHECK_EQUAL(u.get_lhs(), 10);
    BOOST_CHECK_EQUAL(u.get_rhs0(), rhs0);
}

BOOST_AUTO_TEST_CASE(CompactUnaryRuleTest){
    int rhs0 = 1;
    vector<unary_proba_info> probs;
    probs.push_back(unary_proba_info(0, 0, 10.5));
    probs.push_back(unary_proba_info(0, 1, 1.0));
    probs.push_back(unary_proba_info(0, 2, 0.5));

    URule u(10, rhs0, probs);

    vector<vector<double> > before = u.get_probability();
    u.compact();
    vector<vector<double> > after  = u.get_probability();

    BOOST_CHECK_EQUAL(before[0].size(), after[0].size());

    u.set_probability(0, 0, 0.0);
    u.set_probability(0, 1, 0.0);
    u.set_probability(0, 2, 0.0);

    u.compact();
    after = u.get_probability();

    BOOST_CHECK_NE(before[0].size(), after[0].size());
    BOOST_CHECK_EQUAL(after[0].size(), 0); //The vector has been compacted
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(BinaryRuleSuite)

BOOST_AUTO_TEST_CASE(BinaryRuleInstantiationTest){
    int rhs0 = 1;
    int rhs1 = 2;
    vector<binary_proba_info> probs;
    probs.push_back(binary_proba_info(0, 0, 0, 10.5));
    probs.push_back(binary_proba_info(1, 0, 0, 1.0));

    BRule b(10, rhs0, rhs1, probs);

    //Test to see if the probs and probabilities vector
    //inside lex rule are the same
    vector_3d probs_double;
    vector<double> p1; p1.push_back(10.5);
    vector<double> p2; p2.push_back(1.0);
    vector<vector<double> > pp1, pp2;
    pp1.push_back(p1); pp2.push_back(p2);
    probs_double.push_back(pp1);
    probs_double.push_back(pp2);
    vector_3d v = b.get_probability();

    BOOST_CHECK_EQUAL(probs_double.size(), v.size());

    vector<double> first = probs_double[0][0], first_brule = v[0][0];
    vector<double> second = probs_double[1][0], second_brule = v[1][0];

    BOOST_CHECK_EQUAL_COLLECTIONS(first.begin(), first.end(),
                                  first_brule.begin(), first_brule.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(second.begin(), second.end(),
                                  second_brule.begin(), second_brule.end());

    BOOST_CHECK_EQUAL(b.get_lhs(), 10);
    BOOST_CHECK_EQUAL(b.get_rhs0(), rhs0);
    BOOST_CHECK_EQUAL(b.get_rhs1(), rhs1);
}

BOOST_AUTO_TEST_CASE(BinaryRuleCompactTest){
    int rhs0 = 1;
    int rhs1 = 2;
    vector<binary_proba_info> probs;
    probs.push_back(binary_proba_info(0, 0, 0, 10.5));
    probs.push_back(binary_proba_info(0, 0, 1, 1.0));
    probs.push_back(binary_proba_info(0, 1, 0, 1.0));
    probs.push_back(binary_proba_info(0, 1, 1, 1.0));

    BRule b(10, rhs0, rhs1, probs);

    vector_3d before = b.get_probability();
    b.compact();
    vector_3d after  = b.get_probability();

    BOOST_CHECK_EQUAL(before[0].size(), after[0].size());
    BOOST_CHECK_EQUAL(before[0][0].size(), after[0][0].size());

    b.set_probability(0, 0, 0, 0.0);
    b.set_probability(0, 0, 1, 0.0);

    b.compact();
    after = b.get_probability();

    BOOST_CHECK_NE(before[0][0].size(), after[0][0].size());
    BOOST_CHECK_EQUAL(after[0][0].size(), 0); //The vector has been compacted
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(RuleParserSuite)

BOOST_AUTO_TEST_CASE(RuleParserLexicalTest){
    string content = "lex IN of (0,0.02) (1,0.3)";

    string::const_iterator iter = content.begin();
    string::const_iterator end = content.end();
    parser p;

    bool res;
    do {
        AnnotatedRule * r;
        res = phrase_parse(iter, end, p, mychar::space, r);
        BOOST_REQUIRE_EQUAL(res, true);
        BOOST_REQUIRE_EQUAL(r->is_lexical(), true);

        LexicalRule* lexr = static_cast<LexicalRule*>(r);
        unsigned int lhs  = SymbolTable::instance_nt().get_label_id("IN");
        unsigned int word = SymbolTable::instance_word().get_label_id("of");

        BOOST_CHECK_EQUAL(lexr->get_lhs(), (short)lhs);
        BOOST_CHECK_EQUAL(lexr->get_word(), (int)word);
    } while(res && iter != end);
}

BOOST_AUTO_TEST_CASE(RuleParserUnaryTest){
    string content = "int NX CD (0,0,0.002) (1,0,3.8e-05)";
    string::const_iterator iter = content.begin();
    string::const_iterator end = content.end();
    parser p;

    bool res;
    do {
        AnnotatedRule * r;
        res = phrase_parse(iter, end, p, mychar::space, r);
        BOOST_REQUIRE_EQUAL(res, true);
        BOOST_REQUIRE_EQUAL(r->is_unary(), true);

        URule* ur = static_cast<URule*>(r);
        unsigned int lhs = SymbolTable::instance_nt().get_label_id("NX");
        unsigned int rhs = SymbolTable::instance_nt().get_label_id("CD");

        BOOST_CHECK_EQUAL(ur->get_lhs(), (short)lhs);
        BOOST_CHECK_EQUAL(ur->get_rhs0(), (short)rhs);
    } while(res && iter != end);
}

BOOST_AUTO_TEST_CASE(RuleParserBinaryTest){
    string content = "int ADJP IN S (0,1,0,0.00062) (1,1,0,6.65e-06)";
    string::const_iterator iter = content.begin();
    string::const_iterator end = content.end();
    parser p;

    bool res;
    do {
        AnnotatedRule * r;
        res = phrase_parse(iter, end, p, mychar::space, r);
        BOOST_REQUIRE_EQUAL(res, true);
        BOOST_REQUIRE_EQUAL(r->is_lexical() && r->is_unary(), false);

        BRule* br = static_cast<BRule*>(r);
        unsigned int lhs  = SymbolTable::instance_nt().get_label_id("ADJP");
        unsigned int rhs0 = SymbolTable::instance_nt().get_label_id("IN");
        unsigned int rhs1 = SymbolTable::instance_nt().get_label_id("S");

        BOOST_CHECK_EQUAL(br->get_lhs(), (short)lhs);
        BOOST_CHECK_EQUAL(br->get_rhs0(), (short)rhs0);
        BOOST_CHECK_EQUAL(br->get_rhs1(), (short)rhs1);
    } while(res && iter != end);
}

BOOST_AUTO_TEST_SUITE_END()

