
if WANT_LTR
LIBS+= -lrt
endif

check_PROGRAMS              = twostage_lorgparser_test
noinst_PROGRAMS             = $(check_PROGRAMS)
AM_CPPFLAGS= -I../src -I../src/utils -I../src/classifier -I../src/lexicon -I../src/parsers -I../src/edges -I../src/grammars

common_sources = ../src/Bracketing.h \
    ../src/ChartCKY.h \
    ../src/classifier/Classifier.h \
    ../src/classifier/FreqDist.h \
    ../src/classifier/NaiveBayesClassifier.h \
    ../src/classifier/ProbModel.h \
    ../src/edges/AnnotationInfo.h \
    ../src/edges/BinaryTrainingTree.h \
    ../src/edges/Edge.h \
    ../src/edges/maxrule_functions.h \
    ../src/edges/MaxRuleProbability.h \
    ../src/edges/MaxRuleProbabilityKB.h \
    ../src/edges/MaxRuleProbabilityMultiple.h \
    ../src/edges/PackedEdge.h \
    ../src/edges/PackedEdgeDaughters.h \
    ../src/edges/PackedEdgeProbability.h \
    ../src/edges/TrainingNode.h \
    ../src/edges/ViterbiProbability.h \
    ../src/grammars/AnnotatedLabelsInfo.h \
    ../src/grammars/compact_binary_rules.h \
    ../src/grammars/Grammar.h \
    ../src/grammars/GrammarAnnotated.h \
    ../src/lexicon/ArabicIGMapping.h \
    ../src/lexicon/ArabicUMapping.h \
    ../src/lexicon/BaselineFrenchUMapping.h \
    ../src/lexicon/BasicLexicon.h \
    ../src/lexicon/BerkeleyEnglishUMapping.h \
    ../src/lexicon/BerkeleyLexicon.h \
    ../src/lexicon/EnglishIGMapping.h \
    ../src/lexicon/FrenchIGMapping.h \
    ../src/lexicon/ItalianIGMapping.h \
    ../src/lexicon/Lexicon.h \
    ../src/lexicon/LexiconFactory.h \
    ../src/lexicon/WordSignature.h \
    ../src/lexicon/WordSignatureFactory.h \
    ../src/parsers/ParserCKY.h \
    ../src/parsers/ParserCKYAll.h \
    ../src/parsers/ParserCKYAllFactory.h \
    ../src/parsers/ParserCKYAllMaxVar.h \
    ../src/parsers/ParserCKYAllMaxVarKB.h \
    ../src/parsers/ParserCKYAllMaxVarMultiple.h \
    ../src/parsers/ParserCKYAllViterbi.h \
    ../src/parsers/ParserCKYBest.h \
    ../src/ParseSolution.h \
    ../src/PCKYAllCell.h \
    ../src/PCKYBestCell.h \
    ../src/rules/AnnotatedRule.h \
    ../src/rules/BRule.h \
    ../src/rules/BRuleC2f.h \
    ../src/rules/BRuleTraining.h \
    ../src/rules/C2f.h \
    ../src/rules/LexicalRule.h \
    ../src/rules/LexicalRuleC2f.h \
    ../src/rules/LexicalRuleTraining.h \
    ../src/rules/MetaProduction.h \
    ../src/rules/Production.h \
    ../src/rules/Rule.h \
    ../src/rules/Types4Rules.h \
    ../src/rules/URule.h \
    ../src/rules/URuleC2f.h \
    ../src/rules/URuleTraining.h \
    ../src/TagGuesser.h \
    ../src/utils/ConfigTable.h \
    ../src/utils/data_parsers/AnnotHistoriesParser.h \
    ../src/utils/data_parsers/BURuleInputParser.h \
    ../src/utils/data_parsers/ParseError.h \
    ../src/utils/data_parsers/PTBInputParser.h \
    ../src/utils/data_parsers/RuleInputParser.h \
    ../src/utils/EnglishSpec.h \
    ../src/utils/LorgConstants.h \
    ../src/utils/LorgOptions.h \
    ../src/utils/PsTree.h \
    ../src/utils/PtbPsTree.h \
    ../src/utils/RandomGenerator.h \
    ../src/utils/RuleVect2Map.h \
    ../src/utils/SymbolTable.h \
    ../src/utils/Tagger.h \
    ../src/utils/Tokeniser.h \
    ../src/utils/TokeniserSpec.h \
    ../src/utils/Tree.h \
    ../src/utils/tree_detail/BreadthFirstTreeIterator.h \
    ../src/utils/tree_detail/ConstDepthFirstTreeIterator.h \
    ../src/utils/tree_detail/ConstLeafTreeIterator.h \
    ../src/utils/tree_detail/ConstTreeIterator.h \
    ../src/utils/tree_detail/DepthFirstTreeIterator.h \
    ../src/utils/tree_detail/LeafTreeIterator.h \
    ../src/utils/tree_detail/Node.h \
    ../src/utils/tree_detail/TreeIterator.h \
    ../src/Word.h \
    ../src/edges/AnnotationInfo.cpp \
    ../src/edges/BinaryTrainingTree.cpp \
    ../src/edges/Edge.cpp \
    ../src/edges/TrainingNode.cpp \
    ../src/grammars/AnnotatedLabelsInfo.cpp \
    ../src/grammars/compact_binary_rules.cpp \
    ../src/grammars/compact_binary_rules_annotated.cpp \
    ../src/grammars/compact_binary_rules_simple.cpp \
    ../src/grammars/Grammar.cpp \
    ../src/lexicon/ArabicIGMapping.cpp \
    ../src/lexicon/ArabicUMapping.cpp \
    ../src/lexicon/BaselineFrenchUMapping.cpp \
    ../src/lexicon/BasicLexicon.cpp \
    ../src/lexicon/BerkeleyEnglishUMapping.cpp \
    ../src/lexicon/BerkeleyLexicon.cpp \
    ../src/lexicon/EnglishIGMapping.cpp \
    ../src/lexicon/FrenchIGMapping.cpp \
    ../src/lexicon/ItalianIGMapping.cpp \
    ../src/lexicon/Lexicon.cpp \
    ../src/lexicon/LexiconFactory.cpp \
    ../src/lexicon/WordSignatureFactory.cpp \
    ../src/parsers/ParserCKYBest.cpp \
    ../src/PCKYBestCell.cpp \
    ../src/rules/BRule.cpp \
    ../src/rules/BRuleTraining.cpp \
    ../src/rules/LexicalRule.cpp \
    ../src/rules/LexicalRuleTraining.cpp \
    ../src/rules/Production.cpp \
    ../src/rules/Rule.cpp \
    ../src/rules/URule.cpp \
    ../src/rules/URuleTraining.cpp \
    ../src/utils/ConfigTable.cpp \
    ../src/utils/data_parsers/AnnotHistoriesParser.cpp \
    ../src/utils/data_parsers/PTBInputParser.cpp \
    ../src/utils/data_parsers/RuleInputParser.cpp \
    ../src/utils/PtbPsTree.cpp \
    ../src/utils/RandomGenerator.cpp \
    ../src/utils/SymbolTable.cpp \
    ../src/utils/Tagger.cpp \
    ../src/utils/Tokeniser.cpp \
    ../src/Word.cpp

#    ../src/lexicon/ArabicUNVMapping.h ../src/lexicon/ArabicUNVMapping.cpp
#    ../src/utils/data_parsers/BURuleInputParser.cpp

twostage_lorgparser_test_SOURCES = $(common_sources)
twostage_lorgparser_test_SOURCES += ParserEdgeTest.cpp \
									ParserInitTest.cpp \
									ParserRulesTest.cpp \
									ParserTreeTest.cpp \
									main.cpp

twostage_lorgparser_test_CPPFLAGS= $(BOOST_CPPFLAGS) $(TBB_CPPFLAGS) $(AM_CPPFLAGS)
twostage_lorgparser_test_CXXFLAGS= -O6 -Wall -Wextra -pipe

twostage_lorgparser_test_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB) $(TBB_LDFLAGS) $(TBB_LIBS)
