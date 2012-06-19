#include "mdtTest.h"
#include <stdlib.h>
#include <time.h>

mdtTest::mdtTest()
{  
}

mdtTest::~mdtTest()
{
}

bool mdtTest::verify(bool statement, const char* statementStr, const char* description, const char* file, int line)
{
  // If a statement is false (=verification fails), we display the details
  if(!statement){
    cerr << "Test fail in file " << file << endl;
    cerr << "-> Line " << line << " , statement (" << statementStr << ") returned false" << endl;
    cerr << "-> Test description: " << description << endl;
    return false;
  }
  return true;
}

void mdtTest::randomValueInit()
{
  srand(time(0));
  srand(randomValue(1000, RAND_MAX));
}

int mdtTest::randomValue(int min, int max)
{
  return (int)( (double)min + (int)((double)max*rand()/(RAND_MAX + (double)min)) );
}
