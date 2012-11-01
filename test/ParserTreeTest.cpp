#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "PtbPsTree.h"

using namespace std;

typedef Tree<string> StrTree;

BOOST_AUTO_TEST_SUITE(BasicTreeSuite)

BOOST_AUTO_TEST_CASE(NodeTreeTest){
    Node<string>* root = new Node<string>("TOP");
    Node<string>* n = new Node<string>("TOP", root);

    BOOST_CHECK_EQUAL(root->height(), 1);
    BOOST_CHECK_EQUAL(n->height(), 2);

    BOOST_CHECK_EQUAL(root->leaf(), false);
    BOOST_CHECK_EQUAL(n->leaf(), true);

    BOOST_CHECK_EQUAL(root->has_mother(), false);
    BOOST_CHECK_EQUAL(n->has_mother(), true);
}

BOOST_AUTO_TEST_CASE(TreeTest){
    vector<StrTree> daughters;
    vector<StrTree> np_daughters;
    vector<StrTree> vp_daughters;

    np_daughters.push_back(StrTree("NPP"));
    vp_daughters.push_back(StrTree("V"));
    daughters.push_back(StrTree("NP", np_daughters));
    daughters.push_back(StrTree("VP", vp_daughters));
    StrTree* tree = new Tree<string>("TOP", daughters);

    std::cout << tree << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(PtbTreeSuite)

BOOST_AUTO_TEST_SUITE_END()
