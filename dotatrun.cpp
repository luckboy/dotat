//
// dotatrun.cpp - .@ Langauge
// by £ukasz Szpakowski
//

#include <fstream>
#include <iostream>
#include "dotat.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  if(argc!=2) {
    cerr << "Usage: " << argv[0] << " <program file>" << endl;
    return 1;
  }
  const char *filename=argv[1];
  ifstream ifs(filename);

  if(!ifs.is_open()) {
    cerr << filename << ": Can\'t open file" << endl;
    return 1;
  }
  try {
    dotat::Interp interp;

    interp.interp(ifs);
  } catch(dotat::ParseError &e) {
    cerr << filename << ": " << e.what() << endl;
    return 1;
  } catch(exception &e) {
    cerr << e.what() << endl;
  }
  return 0;
}
