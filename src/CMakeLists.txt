add_subdirectory(training)

set(common_sources 
         rules/Production.cpp
         rules/Rule.cpp
         grammars/compact_binary_rules.cpp
         grammars/Grammar.cpp
         utils/SymbolTable.cpp
         utils/Tagger.cpp
         utils/TokeniserFactory.cpp
         utils/Tokeniser.cpp
         utils/PtbPsTree.cpp
         utils/LorgConstants.h
         utils/ConfigTable.cpp
         utils/RandomGenerator.cpp
         lexicon/WordSignatureFactory.cpp
         lexicon/BerkeleyEnglishUMapping.cpp
         lexicon/BaselineFrenchUMapping.cpp
         lexicon/EnglishIGMapping.cpp
         lexicon/FrenchIGMapping.cpp
         lexicon/ItalianIGMapping.cpp
         lexicon/ArabicUMapping.cpp
         lexicon/ArabicIGMapping.cpp
         Word.cpp
         parsers/ParserCKY.h
         LorgApp.cpp
         LorgParseApp.cpp
         feature_extract/Extract.cpp
         feature_extract/Feature.cpp)

set(simple_lorgparser_SOURCES
         ${common_sources}
         simple_lorgparser.cpp
         grammars/compact_binary_rules_simple.cpp
         utils/data_parsers/RuleInputParser.cpp
         edges/Edge.cpp
         parsers/ParserCKYBest.cpp
         PCKYBestCell.cpp
         SimpleLorgParseApp.cpp)

set(twostage_lorgparser_SOURCES
         ${common_sources}
         twostage_lorgparser.cpp
         grammars/compact_binary_rules_annotated.cpp
         grammars/AnnotatedLabelsInfo.cpp
         utils/data_parsers/BURuleInputParser.cpp
         utils/data_parsers/AnnotHistoriesParser.cpp
         edges/AnnotationInfo.cpp
         rules/BRule.cpp rules/URule.cpp rules/LexicalRule.cpp)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
    set(CMAKE_CXX_FLAGS_RELEASE "-O6 -Wall -Wextra -pipe -DNDEBUG")
    #set(CMAKE_CXX_FLAGS_RELEASE -Ofast -Wall -Wextra -pipe
    #set(CMAKE_CXX_FLAGS_RELEASE -O3 -Wall -pipe -fomit-frame-pointer -march=core2  -mtune=core2")
    #set(CMAKE_CXX_FLAGS_RELEASE -O3 -Wall -pipe -DNDEBUG -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -Wextra -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls")
endif()


add_executable(simple_lorgparser ${simple_lorgparser_SOURCES})
add_executable(twostage_lorgparser ${twostage_lorgparser_SOURCES})

set(common_libs ${RTLIB} ${TBB_LIBS} ${TCMALLOC_LIB})
target_link_libraries(simple_lorgparser ${common_libs} ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(twostage_lorgparser ${common_libs} ${Boost_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# install
install (TARGETS simple_lorgparser twostage_lorgparser DESTINATION bin)
