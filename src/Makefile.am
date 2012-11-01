#SUBDIRS = training

if WANT_LTR
LIBS+= -lrt 
endif

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I.
#AM_LDFLAGS = $(BOOST_LDFLAGS)

bin_PROGRAMS = simple_lorgparser twostage_lorgparser feature_extractor
#gram2clmodel

common_sources = rules/Types4Rules.h rules/MetaProduction.h \
	rules/Production.cpp rules/Production.h \
	rules/Rule.cpp rules/Rule.h \
	grammars/compact_binary_rules.h grammars/compact_binary_rules.cpp \
	grammars/Grammar.h grammars/Grammar.cpp \
	utils/SymbolTable.cpp utils/SymbolTable.h \
	utils/Tagger.h utils/Tagger.cpp \
	utils/TokeniserFactory.h utils/TokeniserFactory.cpp\
	utils/Tokeniser.h utils/Tokeniser.cpp utils/TokeniserSpec.h \
	utils/EnglishSpec.h Bracketing.h \
	utils/PtbPsTree.cpp utils/PtbPsTree.h utils/PsTree.h \
	utils/Tree.h utils/tree_detail/BreadthFirstTreeIterator.h  \
	utils/tree_detail/ConstDepthFirstTreeIterator.h  \
	utils/tree_detail/ConstLeafTreeIterator.h  utils/tree_detail/ConstTreeIterator.h  \
	utils/tree_detail/DepthFirstTreeIterator.h  utils/tree_detail/LeafTreeIterator.h  \
	utils/tree_detail/Node.h  utils/tree_detail/TreeIterator.h \
	utils/LorgConstants.h\
	utils/ConfigTable.h utils/ConfigTable.cpp utils/LorgOptions.h \
	utils/data_parsers/ParseError.h \
	utils/RandomGenerator.h utils/RandomGenerator.cpp \
	utils/tree_detail/BreadthFirstTreeIterator.h utils/tree_detail/ConstDepthFirstTreeIterator.h \
	utils/tree_detail/ConstLeafTreeIterator.h utils/tree_detail/ConstTreeIterator.h \
	utils/tree_detail/DepthFirstTreeIterator.h utils/tree_detail/LeafTreeIterator.h \
	utils/tree_detail/Node.h utils/tree_detail/TreeIterator.h \
	lexicon/WordSignature.h \
	lexicon/WordSignatureFactory.h lexicon/WordSignatureFactory.cpp \
	lexicon/BerkeleyEnglishUMapping.h lexicon/BerkeleyEnglishUMapping.cpp \
	lexicon/BaselineFrenchUMapping.h lexicon/BaselineFrenchUMapping.cpp \
	lexicon/EnglishIGMapping.h lexicon/EnglishIGMapping.cpp \
	lexicon/FrenchIGMapping.h lexicon/FrenchIGMapping.cpp \
	lexicon/ItalianIGMapping.h lexicon/ItalianIGMapping.cpp \
	lexicon/ArabicUMapping.h lexicon/ArabicUMapping.cpp \
	lexicon/ArabicIGMapping.h lexicon/ArabicIGMapping.cpp \
        classifier/NaiveBayesClassifier.h classifier/ProbModel.h \
        classifier/Classifier.h classifier/FreqDist.h \
	ChartCKY.h \
	Word.cpp Word.h \
	ParseSolution.h \
	parsers/ParserCKY.h\
	LorgApp.h LorgApp.cpp \
	LorgParseApp.h LorgParseApp.cpp \
	feature_extract/Extract.h feature_extract/Extract.cpp \
	feature_extract/Feature.h feature_extract/Feature.cpp \
	TagGuesser.h

simple_lorgparser_SOURCES = ${common_sources}
simple_lorgparser_SOURCES += simple_lorgparser.cpp \
	grammars/compact_binary_rules_simple.cpp \
	utils/data_parsers/RuleInputParser.cpp utils/data_parsers/RuleInputParser.h \
	edges/Edge.cpp edges/Edge.h \
	parsers/ParserCKYBest.cpp parsers/ParserCKYBest.h \
	PCKYBestCell.cpp PCKYBestCell.h \
	SimpleLorgParseApp.h SimpleLorgParseApp.cpp


simple_lorgparser_CPPFLAGS= $(BOOST_CPPFLAGS) $(TBB_CPPFLAGS) -I. -DNDEBUG
simple_lorgparser_CXXFLAGS= -O6 -Wall -Wextra -pipe
#simple_lorgparser_CXXFLAGS= -O3 -Wall -pipe -fomit-frame-pointer -march=core2  -mtune=core2
#simple_lorgparser_CXXFLAGS= -O3 -Wall -pipe -DNDEBUG -fomit-frame-pointer

#flags for debugging
#simple_lorgparser_CPPFLAGS= $(BOOST_CPPFLAGS) -I.
#simple_lorgparser_CXXFLAGS= -O2 -g -pg -Wall



if WANT_TCMALLOC
simple_lorgparser_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(TBB_LDFLAGS) $(TBB_LIBS) -ltcmalloc
else
simple_lorgparser_LDADD = $(BOOST_LDFLAGS)  $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(TBB_LDFLAGS) $(TBB_LIBS)
endif
#simple_lorgparser_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) -lprofiler

twostage_lorgparser_SOURCES = $(common_sources)
twostage_lorgparser_SOURCES += twostage_lorgparser.cpp \
	grammars/compact_binary_rules_annotated.cpp \
	grammars/AnnotatedLabelsInfo.h grammars/AnnotatedLabelsInfo.cpp \
	grammars/GrammarAnnotated.h utils/RuleVect2Map.h\
	utils/data_parsers/BURuleInputParser.cpp utils/data_parsers/BURuleInputParser.h \
	utils/data_parsers/AnnotHistoriesParser.h utils/data_parsers/AnnotHistoriesParser.cpp \
	edges/PackedEdge.h edges/PackedEdgeDaughters.h edges/PackedEdgeProbability.h \
	edges/ViterbiProbability.h edges/maxrule_functions.h \
	edges/MaxRuleProbability.h edges/MaxRuleProbabilityKB.h \
	edges/MaxRuleProbabilityMultiple.h \
	PCKYAllCell.h parsers/ParserCKYAll.h \
	parsers/ParserCKYAllViterbi.h parsers/ParserCKYAllMaxVar.h \
	parsers/ParserCKYAllMaxVarKB.h parsers/ParserCKYAllMaxVarMultiple.h \
	parsers/ParserCKYAllFactory.h \
	rules/AnnotatedRule.h edges/AnnotationInfo.h edges/AnnotationInfo.cpp\
	rules/C2f.h \
	rules/BRule.h rules/BRule.cpp rules/BRuleC2f.h\
	rules/URule.h rules/URule.cpp rules/URuleC2f.h\
	rules/LexicalRule.h rules/LexicalRule.cpp rules/LexicalRuleC2f.h \
	TwoStageLorgParseApp.h

twostage_lorgparser_CPPFLAGS= $(BOOST_CPPFLAGS) $(TBB_CPPFLAGS) -I. -DNDEBUG
twostage_lorgparser_CXXFLAGS= -O6 -Wall -Wextra -pipe
#twostage_lorgparser_CXXFLAGS= -Ofast -Wall -Wextra -pipe
#twostage_lorgparser_CXXFLAGS= -O3 -Wall -pipe -fomit-frame-pointer -march=core2  -mtune=core2
#twostage_lorgparser_CXXFLAGS= -O3 -Wall -pipe -DNDEBUG -fomit-frame-pointer

#flags for debugging
#twostage_lorgparser_CPPFLAGS= $(BOOST_CPPFLAGS) -I.
#twostage_lorgparser_CXXFLAGS= -O1 -g -pg -Wall -Wextra -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls



if WANT_TCMALLOC
twostage_lorgparser_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(TBB_LDFLAGS) $(TBB_LIBS) -ltcmalloc
else
twostage_lorgparser_LDADD = $(BOOST_LDFLAGS)  $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(TBB_LDFLAGS) $(TBB_LIBS)
endif
#twostage_lorgparser_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)  $(BOOST_THREAD_LIB) -lprofiler


# gram2clmodel_CXXFLAGS= -O2 -g -pg -Wall
# gram2clmodel_SOURCES = gram2clmodel.cpp TagGuesser.cpp utils/SymbolTable.cpp \
# 	utils/Features.cpp rules/Production.cpp rules/Rule.cpp \
# 	utils/data_parsers/RuleInputParser.cpp

# gram2clmodel_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)
# # gram2clmodel_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) -lprofiler -ltcmalloc


feature_extractor_CXXFLAGS= -O3 -Wall -Wextra -pipe
feature_extractor_SOURCES = feature_extract/ExtractApp.h \
	feature_extract/ExtractApp.cpp \
	LorgApp.h LorgApp.cpp \
	feature_extract/Extract.h feature_extract/Extract.cpp \
	feature_extract/Feature.h feature_extract/Feature.cpp \
	utils/data_parsers/PTBInputParser.h utils/data_parsers/PTBInputParser.cpp \
	utils/PtbPsTree.cpp utils/PtbPsTree.h utils/PsTree.h \
	utils/Tree.h utils/tree_detail/BreadthFirstTreeIterator.h  \
	utils/tree_detail/ConstDepthFirstTreeIterator.h  \
	utils/tree_detail/ConstLeafTreeIterator.h  utils/tree_detail/ConstTreeIterator.h  \
	utils/tree_detail/DepthFirstTreeIterator.h  utils/tree_detail/LeafTreeIterator.h  \
	utils/tree_detail/Node.h  utils/tree_detail/TreeIterator.h \
	utils/LorgConstants.h\
	utils/ConfigTable.h utils/ConfigTable.cpp utils/LorgOptions.h \
	utils/data_parsers/ParseError.h \
	utils/SymbolTable.h utils/SymbolTable.cpp \
	rules/Types4Rules.h rules/MetaProduction.h \
	rules/Production.cpp rules/Production.h

#\	rules/Rule.cpp rules/Rule.h


feature_extractor_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)
