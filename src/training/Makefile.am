## Makefile.am -- Process this file with automake to produce Makefile.in

bin_PROGRAMS = tb2gram

if WANT_LTR
LIBS+= -lrt 
endif


AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(TBB_CPPFLAGS) -I$(top_srcdir)/src
tb2gram_CXXFLAGS = -O6 -Wall -Wextra -pipe -ffast-math -DNDEBUG
#tb2gram_CXXFLAGS = -O3 -Wall -pipe #-fdiagnostics-show-option

#flags for debugging
#CXXFLAGS = -O0 -g -fno-inline -Wall -pg
#tb2gram_CXXFLAGS = -Wall -Wextra  -O2  -g  -pg -DBENCHMARK
#tb2gram_CXXFLAGS = -O1  -g  -Wall -Wextra -pg

tb2gram_SOURCES = tb2gram.cpp \
	../LorgApp.h ../LorgApp.cpp LorgTrainerApp.h LorgTrainerApp.cpp \
	../utils/PtbPsTree.cpp ../utils/PtbPsTree.h \
	Treebank.h Treebank.cpp	TreebankFactory.h \
	../utils/data_parsers/ParseError.h \
	../utils/data_parsers/PTBInputParser.cpp ../utils/data_parsers/PTBInputParser.h \
	../utils/SymbolTable.cpp ../utils/SymbolTable.h \
	../rules/Production.cpp ../rules/Production.h \
	../rules/Rule.cpp ../rules/Rule.h \
	../utils/ConfigTable.cpp ../utils/ConfigTable.h	\
	EMTrainer.h EMTrainer.cpp \
	../utils/RandomGenerator.cpp ../utils/RandomGenerator.h \
	../rules/BRule.h ../rules/BRule.cpp  \
	../rules/URule.h ../rules/URule.cpp \
	../rules/BRuleTraining.h ../rules/BRuleTraining.cpp  \
	../rules/URuleTraining.h ../rules/URuleTraining.cpp \
	../rules/LexicalRule.h ../rules/LexicalRule.cpp \
	../rules/LexicalRuleTraining.h ../rules/LexicalRuleTraining.cpp \
	../grammars/AnnotatedLabelsInfo.h ../grammars/AnnotatedLabelsInfo.cpp \
	../grammars/Grammar.h ../grammars/Grammar.cpp \
	../lexicon/Lexicon.h ../lexicon/Lexicon.cpp \
	../lexicon/BasicLexicon.h ../lexicon/BasicLexicon.cpp \
	../lexicon/BerkeleyLexicon.h ../lexicon/BerkeleyLexicon.cpp  \
	../lexicon/WordSignature.h \
	../lexicon/LexiconFactory.h ../lexicon/LexiconFactory.cpp \
	../lexicon/WordSignatureFactory.h ../lexicon/WordSignatureFactory.cpp \
	../lexicon/BerkeleyEnglishUMapping.h ../lexicon/BerkeleyEnglishUMapping.cpp \
	../lexicon/BaselineFrenchUMapping.h ../lexicon/BaselineFrenchUMapping.cpp \
	../lexicon/EnglishIGMapping.h ../lexicon/EnglishIGMapping.cpp \
	../lexicon/FrenchIGMapping.h ../lexicon/FrenchIGMapping.cpp \
	../lexicon/ItalianIGMapping.h ../lexicon/ItalianIGMapping.cpp \
	../lexicon/ArabicUMapping.h ../lexicon/ArabicUMapping.cpp \
	../lexicon/ArabicIGMapping.h ../lexicon/ArabicIGMapping.cpp \
	../edges/AnnotationInfo.h ../edges/AnnotationInfo.cpp \
	../edges/BinaryTrainingTree.h ../edges/BinaryTrainingTree.cpp \
	../edges/TrainingNode.h ../edges/TrainingNode.cpp \
	TrainingGrammar.h TrainingGrammar.cpp

if WANT_TCMALLOC
 tb2gram_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_THREAD_LIB) $(TBB_LDFLAGS) $(TBB_LIBS) -lm -ltcmalloc
else
 tb2gram_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_THREAD_LIB) $(TBB_LDFLAGS) $(TBB_LIBS) -lm
endif
# tb2gram_LDADD = $(BOOST_LDFLAGS) $(BOOST_REGEX_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB) -lm



#AM_CXXFLAGS = $(BOOST_CPPFLAGS)

AM_LDLFAGS = $(BOOST_LDFLAGS)
