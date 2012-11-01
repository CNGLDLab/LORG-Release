#include "ExtractApp.h"
#include "utils/data_parsers/PTBInputParser.h"


ExtractApp::ExtractApp() : LorgApp(),  extractor()
{
  in = &std::cin;
  out= &std::cout;

  std::vector<Feature> v;
  v.push_back(RuleFeature());
  v.push_back(RuleParentFeature());
  v.push_back(RuleGrandParentFeature());
  v.push_back(BiGramNodeFeature());
  v.push_back(HeavyFeature());
  v.push_back(NeighboursFeature());
  v.push_back(NeighboursExtFeature());
  v.push_back(WordFeature2());
  v.push_back(WordFeature3());
  v.push_back(WordFeatureGen2());
  v.push_back(WordFeatureGen3());

  extractor = Extract(v);
}

LorgOptions ExtractApp::get_options() const
{
  LorgOptions options;
  return options;
}


int ExtractApp::run()
{
  std::string raw_string;

  while(std::getline((*in),raw_string)) {

    if(raw_string == "" || raw_string == "no parse found" || raw_string == "(())")
      (*out) << std::endl;
    else {
      std::string result;
      PtbPsTree tree = PTBInputParser::from_string(raw_string)[0]; // assume one tree per line
      tree.remove_function(); // FIXME : make it a command-line
      extractor.extract(tree, result);
      (*out) << result << std::endl;
    }
  }

  (*out) << std::endl;
  return 0;
}

int main(int argc, char *argv[])
{
  ExtractApp ea;
  ea.run();

  return 0;
}
