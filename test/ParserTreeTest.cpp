#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "utils/SymbolTable.h"
#include "utils/PtbPsTree.h"
#include "utils/data_parsers/PTBInputParser.h"

using namespace std;

typedef Tree<string> StrTree;

struct FixtureIteratorTree{
    //(TOP (NP NPP) (VP V))
    FixtureIteratorTree()
    {
        vector<StrTree> daughters;
        vector<StrTree> np_daughters;
        vector<StrTree> vp_daughters;
        np_daughters.push_back(StrTree("NPP"));
        vp_daughters.push_back(StrTree("V"));
        daughters.push_back(StrTree("NP", np_daughters));
        daughters.push_back(StrTree("VP", vp_daughters));

        tree = new Tree<string>("TOP", daughters);
    }

    ~FixtureIteratorTree()
    {
        delete tree;
        tree = NULL;
    }

    StrTree *tree;
};


struct FixturePTBTree{
    /*( (S
        (S-TPC-1
            (NP-SBJ (NNS People))
            (VP (VBD were) (ADVP (RB even)) (VP (VBG hoarding) (NP (NNS bags)))))
        (, ,)
        (NP-SBJ (PRP he))
        (VP (VBZ says) (S (-NONE- *T*-1)))
        (. .)
    )) */
    FixturePTBTree()
    {
        vector<PtbPsTree> s, s_tpc, np_sbj, vp, advp,
                vp2, np, np_sbj2, vp3, s2;

        PtbPsTree ponct_point = create_one_child(".", ".");
        PtbPsTree ponct_comma = create_one_child(",", ",");

        np_sbj.push_back(create_one_child("NNS", "People"));

        s_tpc.push_back(create_tree("NP-SBJ", np_sbj));

        vp.push_back(create_one_child("VBD", "were"));
        advp.push_back(create_one_child("RB", "even"));

        vp2.push_back(create_one_child("VBG", "hoarding"));
        np.push_back(create_one_child("NNS", "bags"));
        vp2.push_back(create_tree("NP", np));

        vp.push_back(create_tree("ADVP", advp));
        vp.push_back(create_tree("VP", vp2));

        s_tpc.push_back(create_tree("VP", vp));

        s.push_back(create_tree("S-TPC-1", s_tpc));
        s.push_back(ponct_comma);

        np_sbj2.push_back(create_one_child("PRP", "he"));
        s.push_back(create_tree("NP-SBJ", np_sbj2));

        vp3.push_back(create_one_child("VBZ", "says"));
        s2.push_back(create_one_child("-NONE-", "*T*-1"));
        vp3.push_back(create_tree("S", s2));

        s.push_back(create_tree("VP", vp3));
        s.push_back(ponct_point);

        m_tree = new PtbPsTree("S", s);
    }


    inline PtbPsTree create_one_child(string POS, string leaf)
    {
        vector<PtbPsTree> child;
        child.push_back(PtbPsTree(leaf));
        PtbPsTree tree(POS, child);

        return tree;
    }

    inline PtbPsTree create_tree(string nonTerm, vector<PtbPsTree> children)
    {
        return PtbPsTree(nonTerm, children);
    }

    ~FixturePTBTree()
    {
        delete m_tree;
        m_tree = NULL;
    }

    PtbPsTree *m_tree;
};

struct FixturePTBTreeBinarisePossible{
    /* ( (S
        (NP-SBJ he)
        (VBZ says)
        (. .)
    )) */
    FixturePTBTreeBinarisePossible()
    {
        vector<PtbPsTree> s, sub_s;

        PtbPsTree np_sbj = create_one_child("NP-SBJ", "he");
        PtbPsTree vbz    = create_one_child("VBZ", "says");
        PtbPsTree ponct  = create_one_child(".", ".");

        sub_s.push_back(np_sbj);
        sub_s.push_back(vbz);
        sub_s.push_back(ponct);

        PtbPsTree sub_s_tree("SUB", sub_s);
        s.push_back(sub_s_tree);

        m_tree = new PtbPsTree("S", s);
    }


    inline PtbPsTree create_one_child(string POS, string leaf)
    {
        vector<PtbPsTree> child;
        child.push_back(PtbPsTree(leaf));
        PtbPsTree tree(POS, child);

        return tree;
    }

    inline PtbPsTree create_tree(string nonTerm, vector<PtbPsTree> children)
    {
        return PtbPsTree(nonTerm, children);
    }

    ~FixturePTBTreeBinarisePossible()
    {
        delete m_tree;
        m_tree = NULL;
    }

    PtbPsTree *m_tree;
};

BOOST_AUTO_TEST_SUITE(BasicTreeSuite)

BOOST_AUTO_TEST_CASE(NodeTreeTest){
    Node<string>* root = new Node<string>("TOP");
    Node<string>* n = new Node<string>("TOP", root);

    BOOST_CHECK_EQUAL(root->height(), 1U);
    BOOST_CHECK_EQUAL(n->height(), 2U);

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

    BOOST_CHECK_EQUAL(tree->size(), 5U);
    BOOST_CHECK_EQUAL(tree->height(), 3U);
    BOOST_CHECK_EQUAL(tree->number_of_leaves(), 2U);

    tree->add_last_daughter(tree->dfbegin(), "TEST");

    BOOST_CHECK_EQUAL(tree->size(), 6U);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_FIXTURE_TEST_SUITE(TreeIteratorsSuite, FixtureIteratorTree)

BOOST_AUTO_TEST_CASE(TreeDepthFirstIteratorTest){
    vector<string> depth_string;
    depth_string.push_back("TOP");
    depth_string.push_back("NP");
    depth_string.push_back("NPP");
    depth_string.push_back("VP");
    depth_string.push_back("V");

    unsigned i = 0;
    DepthFirstTreeIterator<string> df;
    for (df = tree->dfbegin(); df != tree->dfend(); ++df)
    {
        BOOST_CHECK_NE(i, depth_string.size());
        BOOST_CHECK_EQUAL(*df, depth_string[i++]);
    }
    ++df;
    BOOST_CHECK_EQUAL(*df, "TOP");
    ++df;--df;
    BOOST_CHECK_EQUAL(*df, "TOP");
}

// FIXME
// BOOST_AUTO_TEST_CASE(BreadthIteratorTest){
//     BOOST_CHECK_THROW(tree->bfbegin(), NotImplementedException);
// }

BOOST_AUTO_TEST_CASE(LeafTreeIteratorTest){
    vector<string> leaf_string;
    leaf_string.push_back("NPP");
    leaf_string.push_back("V");

    unsigned i = 0;
    LeafTreeIterator<string> l;
    for (l = tree->lbegin(); l != tree->lend(); ++l)
    {
        BOOST_CHECK_NE(i, leaf_string.size());
        BOOST_CHECK_EQUAL(*l, leaf_string[i++]);
    }
    ++l;
    BOOST_CHECK_EQUAL(*l, "NPP");
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_FIXTURE_TEST_SUITE(PtbTreeSuite, FixturePTBTree)

BOOST_AUTO_TEST_CASE(PtbCleanLabelTest){
    boost::unordered::unordered_set<string> remove;
    remove.insert("-NONE-");
    m_tree->clean(remove);

    for(DepthFirstTreeIterator<string> df = m_tree->dfbegin(); df != m_tree->dfend(); ++df)
        BOOST_CHECK_NE(*df, "-NONE-");
}

BOOST_AUTO_TEST_CASE(PtbRemoveFunctionTest){
    m_tree->remove_function();
    DepthFirstTreeIterator<string> df = m_tree->dfbegin();
    ++df;
    ++df;
    BOOST_CHECK_NE(*df, "NP-SBJ");
    BOOST_CHECK_EQUAL(*df, "NP");
}

/*( (S
    (S-TPC-1
        (NP-SBJ (NNS People))
        (VP (VBD were) (ADVP (RB even)) (VP (VBG hoarding) (NP (NNS bags)))))
    (, ,)
    (NP-SBJ (PRP he))
    (VP (VBZ says) (S (-NONE- *T*-1)))
    (. .)
)) */

BOOST_AUTO_TEST_CASE(PtbProductionsTest){
    vector<Production> internals, lexicals;
    m_tree->productions(internals, lexicals);

    vector<string> verify_internals;
    verify_internals.push_back("S");
    verify_internals.push_back("S-TPC-1");
    verify_internals.push_back(",");
    verify_internals.push_back("NP-SBJ");
    verify_internals.push_back("VP");
    verify_internals.push_back(".");
    verify_internals.push_back("S-TPC-1");
    verify_internals.push_back("NP-SBJ");
    verify_internals.push_back("VP");
    verify_internals.push_back("NP-SBJ");
    verify_internals.push_back("NNS");
    verify_internals.push_back("VP");
    verify_internals.push_back("VBD");
    verify_internals.push_back("ADVP");
    verify_internals.push_back("VP");
    verify_internals.push_back("ADVP");
    verify_internals.push_back("RB");
    verify_internals.push_back("VP");
    verify_internals.push_back("VBG");
    verify_internals.push_back("NP");
    verify_internals.push_back("NP");
    verify_internals.push_back("NNS");
    verify_internals.push_back("NP-SBJ");
    verify_internals.push_back("PRP");
    verify_internals.push_back("VP");
    verify_internals.push_back("VBZ");
    verify_internals.push_back("S");
    verify_internals.push_back("S");
    verify_internals.push_back("-NONE-");

    //For this tree, numbers of internals should be 10
    BOOST_CHECK_EQUAL(internals.size(), 10U);

    SymbolTable& nt = SymbolTable::instance_nt();
    unsigned i = 0;
    for(vector<Production>::const_iterator it = internals.begin();
        it != internals.end(); ++it)
    {
        BOOST_REQUIRE_LT(i, verify_internals.size());
        Production p = *it;

        BOOST_CHECK_EQUAL(p.get_lhs(), int(nt.get_label_id(verify_internals.at(i))));
        for(unsigned j = 0; j < p.get_rhs().size(); j++)
          BOOST_CHECK_EQUAL(p.get_rhs(j), int(nt.get_label_id(verify_internals.at(++i))));
        i++;
    }

    /////////////////////////
    vector<string> verify_lexicals;
    verify_lexicals.push_back("NNS");
    verify_lexicals.push_back("People");
    verify_lexicals.push_back("VBD");
    verify_lexicals.push_back("were");
    verify_lexicals.push_back("RB");
    verify_lexicals.push_back("even");
    verify_lexicals.push_back("VBG");
    verify_lexicals.push_back("hoarding");
    verify_lexicals.push_back("NNS");
    verify_lexicals.push_back("bags");
    verify_lexicals.push_back(",");
    verify_lexicals.push_back(",");
    verify_lexicals.push_back("PRP");
    verify_lexicals.push_back("he");
    verify_lexicals.push_back("VBZ");
    verify_lexicals.push_back("says");
    verify_lexicals.push_back("-NONE-");
    verify_lexicals.push_back("*T*-1");
    verify_lexicals.push_back(".");
    verify_lexicals.push_back(".");

    //For this tree, numbers of lexicals should be 10
    BOOST_CHECK_EQUAL(lexicals.size(), 10U);

    SymbolTable& w = SymbolTable::instance_word();
    i = 0;
    for(vector<Production>::const_iterator it = lexicals.begin();
        it != lexicals.end(); ++it)
    {
        BOOST_REQUIRE_LT(i, verify_lexicals.size());
        Production p = *it;

        BOOST_CHECK_EQUAL(p.get_rhs().size(), 1U);
        BOOST_CHECK_EQUAL(p.get_lhs(), int(nt.get_label_id(verify_lexicals.at(i))));
        BOOST_CHECK_EQUAL(p.get_rhs0(), int(w.get_label_id(verify_lexicals.at(++i))));
        i++;
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(BinarisingTreeSuite, FixturePTBTreeBinarisePossible)

BOOST_AUTO_TEST_CASE(PtbBinarisationTest){
    m_tree->binarise(LEFT, -1);

    DepthFirstTreeIterator<string> it = m_tree->dfbegin();
    ++it;++it;
    BOOST_CHECK_EQUAL(*(it.right()), "[(VBZ)(.)]");

    m_tree->unbinarise();
    DepthFirstTreeIterator<string> it2 = m_tree->dfbegin();
    ++it2;
    BOOST_CHECK_EQUAL(*it2, "NP-SBJ");
    ++it2;++it2;
    BOOST_CHECK_EQUAL(*it2, "VBZ");
    ++it2;++it2;
    BOOST_CHECK_EQUAL(*it2, ".");
}

BOOST_AUTO_TEST_SUITE_END()
