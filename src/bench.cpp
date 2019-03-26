#include "common/algebra/Operators.hpp"
#include "hyper/codegen/Translator.hpp"
#include <iostream>

using namespace std;
using namespace runtime;
using namespace algebra;

int main() {


  cout<<"Test all is in execution"<<endl;
  Relation test;
  test.name = "RelationName";
  test.insert("country", make_unique<Integer>());

  auto scan = make_unique<Scan>(test);
  Print print(move(scan), {"country"});

  hyper::Translator::produce(print, cout);
  return 0;
}
