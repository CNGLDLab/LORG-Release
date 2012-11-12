set(twostage_lorgparser_test_sources
  ../src/edges/AnnotationInfo.cpp
  ../src/edges/BinaryTrainingTree.cpp
  ../src/edges/Edge.cpp
  ../src/edges/TrainingNode.cpp
  ../src/grammars/AnnotatedLabelsInfo.cpp
  ../src/grammars/compact_binary_rules.cpp
  ../src/grammars/compact_binary_rules_annotated.cpp
  ../src/grammars/compact_binary_rules_simple.cpp
  ../src/grammars/Grammar.cpp
  ../src/lexicon/ArabicIGMapping.cpp
  ../src/lexicon/ArabicUMapping.cpp
  ../src/lexicon/BaselineFrenchUMapping.cpp
  ../src/lexicon/BasicLexicon.cpp
  ../src/lexicon/BerkeleyEnglishUMapping.cpp
  ../src/lexicon/BerkeleyLexicon.cpp
  ../src/lexicon/EnglishIGMapping.cpp
  ../src/lexicon/FrenchIGMapping.cpp
  ../src/lexicon/ItalianIGMapping.cpp
  ../src/lexicon/Lexicon.cpp
  ../src/lexicon/LexiconFactory.cpp
  ../src/lexicon/WordSignatureFactory.cpp
  ../src/parsers/ParserCKYBest.cpp
  ../src/PCKYBestCell.cpp
  ../src/rules/BRule.cpp
  ../src/rules/BRuleTraining.cpp
  ../src/rules/LexicalRule.cpp
  ../src/rules/LexicalRuleTraining.cpp
  ../src/rules/Production.cpp
  ../src/rules/Rule.cpp
  ../src/rules/URule.cpp
  ../src/rules/URuleTraining.cpp
  ../src/utils/ConfigTable.cpp
  ../src/utils/data_parsers/AnnotHistoriesParser.cpp
  ../src/utils/data_parsers/PTBInputParser.cpp
  ../src/utils/data_parsers/RuleInputParser.cpp
  ../src/utils/PtbPsTree.cpp
  ../src/utils/RandomGenerator.cpp
  ../src/utils/SymbolTable.cpp
  ../src/utils/Tagger.cpp
  ../src/utils/Tokeniser.cpp
  ../src/Word.cpp
  ParserEdgeTest.cpp
  ParserInitTest.cpp
  ParserRulesTest.cpp
  ParserTreeTest.cpp
  main.cpp
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O6 -Wall -Wextra -pipe -ffast-math -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fno-inline -Wall -pg")
    #set(CMAKE_CXX_FLAGS_DEBUG -Wall -Wextra  -O2  -g  -pg -DBENCHMARK)
    #set(CMAKE_CXX_FLAGS_DEBUG -O1  -g  -Wall -Wextra -pg)
endif()

set(common_libs ${RTLIB} ${TBB_LIBS} ${TCMALLOC_LIB})

add_executable(twostage_lorgparser_test ${twostage_lorgparser_test_sources})
target_link_libraries(twostage_lorgparser_test ${common_libs} ${Boost_LIBRARIES})
# install
install (TARGETS twostage_lorgparser_test DESTINATION bin)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
