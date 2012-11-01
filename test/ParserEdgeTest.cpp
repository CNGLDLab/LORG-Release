#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "edges/Edge.h"
#include "edges/PackedEdge.h"
#include "edges/PackedEdgeDaughters.h"
#include "edges/PackedEdgeProbability.h"
#include "edges/MaxRuleProbabilityKB.h"
#include "edges/MaxRuleProbabilityMultiple.h"
#include "utils/PtbPsTree.h"

#include "rules/BRuleC2f.h"
#include "rules/URuleC2f.h"
#include "rules/LexicalRuleC2f.h"

#include "rules/LexicalRule.h"
#include "rules/BRule.h"
#include "rules/URule.h"

#include "Word.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(EdgeTestSuite)

BOOST_AUTO_TEST_CASE(EdgeConstructorsTest){

    //Terminal edges
    Edge *e0 = new Edge(0, 10.0);
    Edge *e1 = new Edge(3, 30.0);

    BOOST_CHECK_EQUAL(e0->get_lhs(), 0);
    BOOST_CHECK_EQUAL(e0->is_terminal(), true);
    BOOST_CHECK_EQUAL(e0->get_probability(), 10.0);

    //Unary edge
    Edge *e2 = new Edge(1, e0, 11.0);
    BOOST_CHECK_EQUAL(e2->get_lhs(), 1);
    BOOST_CHECK_EQUAL(e2->get_left_child(), e0);
    BOOST_CHECK_EQUAL(e2->get_probability(), 11.0);
    BOOST_CHECK_EQUAL(e2->is_terminal(), false);

    //Binary edge
    Edge *e3 = new Edge(2, e0, e1, 28.65);
    BOOST_CHECK_EQUAL(e3->get_lhs(), 2);
    BOOST_CHECK_EQUAL(e3->get_left_child(), e0);
    BOOST_CHECK_EQUAL(e3->get_right_child(), e1);
    BOOST_CHECK_EQUAL(e3->get_probability(), 28.65);
    BOOST_CHECK_EQUAL(e3->is_terminal(), false);

    //Copy constructor
    Edge *e4 = new Edge(*e1);
    BOOST_CHECK_EQUAL(e4->get_probability(), e1->get_probability());
    BOOST_CHECK_EQUAL(e4->get_lhs(), e1->get_lhs());
    BOOST_CHECK_EQUAL(e4->is_terminal(), e1->is_terminal());

    e4->set_lhs(72);
    BOOST_CHECK_EQUAL(e4->get_lhs(), 72);
    BOOST_CHECK_EQUAL(e1->get_lhs(), 3);
    BOOST_CHECK_NE(e1->get_lhs(), e4->get_lhs());
}

BOOST_AUTO_TEST_CASE(EdgeSettersTest){
    Edge *e0 = new Edge(0, new Edge(1, 10.0), new Edge(2, 15.0), 25.00);

    e0->set_lhs(1);
    BOOST_CHECK_EQUAL(e0->get_lhs(), 1);
    e0->set_probability(350.00);
    BOOST_CHECK_EQUAL(e0->get_probability(), 350.00);

    Edge *e1 = new Edge(3, 22.5);
    e0->set_left_child(e1);
    BOOST_CHECK_EQUAL(e0->get_left_child(), e1);

    Edge *e2 = new Edge(5, 22.6);
    e0->set_right_child(e2);
    BOOST_CHECK_EQUAL(e0->get_right_child(), e2);

    //Need to have the same lhs...
    e0->set_lhs(5);
    e0->replace(*e2);
    BOOST_CHECK_EQUAL(e0->get_lhs(), e2->get_lhs());
    BOOST_CHECK_EQUAL(e0->get_probability(), e2->get_probability());
    BOOST_CHECK_EQUAL(e0->get_left_child(), e2->get_left_child());
    BOOST_CHECK_EQUAL(e0->get_right_child(), e2->get_right_child());
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(PackedEdgeDaughtersSuite)

BOOST_AUTO_TEST_CASE(RuleHolderTest){
    BRule br(0, 1, 2, vector<binary_proba_info>());
    BRule br2(3, 4, 5, vector<binary_proba_info>());

    const BRuleC2f *br_c2f = new BRuleC2f(br);
    const BRuleC2f *br2_c2f = new BRuleC2f(br2);

    RuleHolder<BRuleC2f> rh(br_c2f);
    BOOST_CHECK_EQUAL(rh.get_rule(), br_c2f);
    rh.set_rule(br2_c2f);
    BOOST_CHECK_EQUAL(rh.get_rule(), br2_c2f);
}

BOOST_AUTO_TEST_CASE(BinaryEdgeDaughtersTest){
    //For the sake of the test, we use dummy cell
    // ie : std::string

    const BRuleC2f *br_c2f = new BRuleC2f(BRule(0, 1, 2, vector<binary_proba_info>()));
    const URuleC2f *ur_c2f = new URuleC2f(URule(0, 1, vector<unary_proba_info>()));
    const LexicalRuleC2f *lr_c2f = new LexicalRuleC2f(LexicalRule(0, 1, vector<lexical_proba_info>()));
    string *left = new string;
    string *right = new string;

    BinaryPackedEdgeDaughters<string> *bped = new BinaryPackedEdgeDaughters<string>(left, right, br_c2f);

    BOOST_CHECK_EQUAL(bped->is_binary(), true);
    BOOST_CHECK_EQUAL(bped->is_lexical(), false);
    BOOST_CHECK_EQUAL(bped->left_daughter(), left);
    BOOST_CHECK_EQUAL(bped->right_daughter(), right);

    UnaryPackedEdgeDaughters<string> *uped = new UnaryPackedEdgeDaughters<string>(left, ur_c2f);

    BOOST_CHECK_EQUAL(uped->is_binary(), false);
    BOOST_CHECK_EQUAL(uped->is_lexical(), false);
    BOOST_CHECK_EQUAL(uped->left_daughter(), left);

    SymbolTable& st_word  = SymbolTable::instance_word();
    st_word.insert("cat");

    const Word *w = new Word("cat", 1, 2, vector<string>());

    LexicalPackedEdgeDaughters* lped = new LexicalPackedEdgeDaughters(lr_c2f, w);

    BOOST_CHECK_EQUAL(lped->is_binary(), false);
    BOOST_CHECK_EQUAL(lped->is_lexical(), true);
    BOOST_CHECK_EQUAL(lped->get_word(), w);
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(AnnotationInfoSuite)

BOOST_AUTO_TEST_CASE(ScaledArrayConstructorsTest){
    scaled_array test;
    BOOST_CHECK_EQUAL(test.array.size(), 0);
    BOOST_CHECK_EQUAL(test.scale, 0);

    scaled_array test2(2, 5);
    vector<double> comparison(2, 5);
    BOOST_CHECK_EQUAL(test2.array.size(), 2);
    BOOST_CHECK_EQUAL_COLLECTIONS(test2.array.begin(), test2.array.end(),
                                  comparison.begin(), comparison.end());

    scaled_array test3(test2);
    BOOST_CHECK_EQUAL(test3.scale, test2.scale);
    BOOST_CHECK_EQUAL_COLLECTIONS(test2.array.begin(), test2.array.end(),
                                  test3.array.begin(), test3.array.end());

}

BOOST_AUTO_TEST_CASE(ScaledArrayGettersTest){
    scaled_array test;
    scaled_array test2(2, 5);

    BOOST_CHECK_EQUAL(test2.get_scaled_value(0), test2.array.at(0));
    BOOST_CHECK_EQUAL(test2.get_scaled_logvalue(0), log(test2.array.at(0)));

    BOOST_CHECK_THROW(test.get_scaled_logvalue(0), std::out_of_range);
    BOOST_CHECK_THROW(test.get_scaled_value(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(ScaledArraySettersTest){
    scaled_array test(2, 5.0); //first is size and second is value

    test.reset(3.0);
    vector<double> comparison(2, 3.0);

    BOOST_CHECK_EQUAL_COLLECTIONS(test.array.begin(), test.array.end(),
                                  comparison.begin(), comparison.end());

    test.resize(15);
    BOOST_CHECK_EQUAL(test.array.size(), 15);

    BOOST_CHECK_EQUAL(test.calculate_logscalingfactor(1), LOGSCALE);
    BOOST_CHECK_EQUAL(test.calculate_scalingfactor(0), 1.0);
    BOOST_CHECK_EQUAL(test.calculate_scalingfactor(1), SCALE);
    BOOST_CHECK_EQUAL(test.calculate_scalingfactor(2), SCALE*SCALE);
    BOOST_CHECK_EQUAL(test.calculate_scalingfactor(3), SCALE*SCALE*SCALE);

    //TODO test the scale_array method
}

BOOST_AUTO_TEST_CASE(AnnotationInfoConstructorsTest){
    AnnotationInfo info;
    BOOST_CHECK_THROW(info.get_inside(0), std::out_of_range);
    BOOST_CHECK_THROW(info.get_outside(0), std::out_of_range);
    BOOST_CHECK_EQUAL(info.get_inside_scale(), 0);
    BOOST_CHECK_EQUAL(info.get_outside_scale(), 0);

    AnnotationInfo info2(2, 5.0); // first is size of scaled array and second values

    BOOST_CHECK_EQUAL(info2.get_inside(0), 5.0);
    BOOST_CHECK_EQUAL(info2.get_outside(0), 5.0);
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(PackedEdgeSuite)

BOOST_AUTO_TEST_CASE(PackedEdgeProbabilityTest){
    packed_edge_probability p, p2;
    p.probability = 10.0;
    p2.probability = 11.0;

    BOOST_CHECK(p < p2);
    BOOST_CHECK(p2 > p);
}

BOOST_AUTO_TEST_CASE(PackedEdgeConstructorsTest){
    //For the sake of the test with use a dummy
    //PackedEdge Probability ie string (uh ?)
    typedef BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<string> > > BinaryDaughters;
    typedef UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<string> > > UnaryDaughters;

    PackedEdge<string> *ped = new PackedEdge<string>();
    BOOST_CHECK_EQUAL(ped->get_annotations().get_size(), 1);

    BinaryDaughters bped(NULL, NULL, NULL);
    PackedEdge<string> *ped2 = new PackedEdge<string>(bped);
    BOOST_CHECK_EQUAL(ped2->get_binary_daughters().size(), 1);

    UnaryDaughters uped(NULL, NULL);
    PackedEdge<string> *ped3 = new PackedEdge<string>(uped);
    BOOST_CHECK_EQUAL(ped3->get_unary_daughters().size(), 1);

    LexicalPackedEdgeDaughters lped(NULL, NULL);
    PackedEdge<string> *ped4 = new PackedEdge<string>(lped);
    BOOST_CHECK_EQUAL(ped4->get_lexical_daughters().size(), 1);
}

BOOST_AUTO_TEST_CASE(PackedEdgeAccessorsTest){
    typedef BinaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<string> > > BinaryDaughters;
    typedef UnaryPackedEdgeDaughters<PCKYAllCell<PackedEdge<string> > > UnaryDaughters;

    BinaryDaughters bped(NULL, NULL, NULL);
    PackedEdge<string> *ped = new PackedEdge<string>(bped);

    UnaryDaughters uped(NULL, NULL);
    PackedEdge<string> *ped2 = new PackedEdge<string>(uped);

    LexicalPackedEdgeDaughters lped(NULL, NULL);
    PackedEdge<string> *ped3 = new PackedEdge<string>(lped);

    BOOST_CHECK_EQUAL(ped->get_binary_daughters().size(), 1);
    BOOST_CHECK_THROW(ped->get_binary_daughter(1), out_of_range);
    BOOST_CHECK_EQUAL(ped->get_unary_daughters().size(), 0);

    BOOST_CHECK_EQUAL(ped2->get_unary_daughters().size(), 1);
    BOOST_CHECK_THROW(ped2->get_unary_daughter(1), out_of_range);

    BOOST_CHECK_EQUAL(ped3->get_lexical_daughters().size(), 1);
    BOOST_CHECK_THROW(ped3->get_lexical_daughter(1), out_of_range);

    AnnotationInfo& annot = ped->get_annotations();
    BOOST_CHECK_EQUAL(annot.get_size(), 1);

    //Backup is only in use for MaxRuleProbabilityMultiple
    PackedEdge<MaxRuleProbabilityMultiple> *ped_maxrulem = new PackedEdge<MaxRuleProbabilityMultiple>();
    std::vector<AnnotationInfo>& backup = ped_maxrulem->get_annotations_backup();
    BOOST_CHECK_EQUAL(backup.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
