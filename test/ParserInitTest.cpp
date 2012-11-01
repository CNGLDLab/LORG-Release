#include <boost/test/unit_test.hpp>
#include <string>
#include <Tokeniser.h>
#include <EnglishSpec.h>
#include <sstream>

using namespace std;

BOOST_AUTO_TEST_SUITE(SymbolTableSuite)

BOOST_AUTO_TEST_CASE(STInitTest){
    SymbolTable& st_word  = SymbolTable::instance_word();
    SymbolTable& st_word2 = SymbolTable::instance_word();
    BOOST_CHECK_EQUAL(&st_word, &st_word2);

    SymbolTable& st_nt  = SymbolTable::instance_nt();
    SymbolTable& st_nt2 = SymbolTable::instance_nt();
    BOOST_CHECK_NE(&st_nt, &st_word);
    BOOST_CHECK_EQUAL(&st_nt, &st_nt2);
}

BOOST_AUTO_TEST_CASE(STInsertionTest){
    string str = "cat";
    SymbolTable& st_word = SymbolTable::instance_word();
    st_word.insert(str);
    BOOST_CHECK_NO_THROW(st_word.get_label_id(str));

    unsigned i = st_word.get_label_id(str);
    BOOST_CHECK_EQUAL(i, (unsigned)st_word.insert(str));
    BOOST_CHECK_EQUAL(i, 0u); //The first element is equal to zero
}

BOOST_AUTO_TEST_CASE(STRetrievalTest) {
    //Be aware that instance are static singleton instance, so "cat"
    //from previous tests DOES exists.
    string str = "dog";
    BOOST_CHECK_THROW(SymbolTable::instance_word().get_label_id(str), Miss);
    BOOST_CHECK_THROW(SymbolTable::instance_word().get_label_string(1), Miss);

    SymbolTable::instance_word().insert(str);
    BOOST_CHECK_EQUAL(SymbolTable::instance_word().get_label_id(str), 1);
    BOOST_CHECK_EQUAL(SymbolTable::instance_word().get_label_string(1), str);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(WordSignatureSuite)

BOOST_AUTO_TEST_CASE(TrivialWordSignatureUnknownMappingTest){
    TrivialWordSignature *tws = new TrivialWordSignature();
    BOOST_CHECK_EQUAL(tws->get_unknown_mapping("", 0), SymbolTable::unknown_string);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(WordSuite)

BOOST_AUTO_TEST_CASE(WordTest){
    Word *word = new Word("cat", 0, 1, vector<string>());

    BOOST_CHECK_EQUAL(word->get_end(), 1);
    BOOST_CHECK_EQUAL(word->get_start(), 0);
    BOOST_CHECK_EQUAL(word->get_form(), "cat");
    BOOST_CHECK_EQUAL(word->is_tagged(), false);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(BracketingSuite)

BOOST_AUTO_TEST_CASE(BracketingOverlapTest){
    bracketing b1(0, true, 3, true);
    bracketing b2(1, true, 2, true);

    BOOST_CHECK_EQUAL(b2.overlap(b1), true);
	BOOST_CHECK_EQUAL(b1.overlap(b2), true);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------

BOOST_AUTO_TEST_SUITE(TokeniserSuite)

BOOST_AUTO_TEST_CASE(TokModeTest) {
    TokMode input_mode  = Tokeniser::string_to_tokmode("raw");
    BOOST_CHECK_EQUAL(input_mode, Raw);
    input_mode  = Tokeniser::string_to_tokmode("tok");
    BOOST_CHECK_EQUAL(input_mode, Tok);
    input_mode  = Tokeniser::string_to_tokmode("tag");
    BOOST_CHECK_EQUAL(input_mode, Tag);
    BOOST_CHECK_THROW(Tokeniser::string_to_tokmode("zzz"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(TokeniserInstantiationTest){
    TokMode mode = Tokeniser::string_to_tokmode("tok");
    Tokeniser* tokenizer = new Tokeniser(new EnglishSpec(), false, mode, "", "", '#');

    BOOST_CHECK(tokenizer);
}

// memory access violation...
/*
BOOST_AUTO_TEST_CASE(TokeniserTokModeTest){
    TokMode mode = Tokeniser::string_to_tokmode("tok");
    Tokeniser tokeniser(new EnglishSpec(), false, mode, "", "", '#');
    istringstream istrstring("The dog barks .");

    vector<Word> words;
    vector<bracketing> brackets;
    vector<string> comments;
    string raw;
    tokeniser.tokenise(istrstring, raw, words, brackets, comments);

    BOOST_CHECK_GT(words.size(), 0);
}
*/

BOOST_AUTO_TEST_SUITE_END()
