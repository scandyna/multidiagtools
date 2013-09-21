/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtTest.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <cfloat>
#include <QString>

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

void mdtTest::compare(double x, double y, int n, double min, double max, const char *file, int line)
{
  Q_ASSERT(n >= 0);

  double lsb = (max-min) / pow(2.0, n);

  if(qAbs(x-y) > (lsb+DBL_EPSILON)){
    QString msg = "Compared values are out of tolerence:\n";
    msg += "  Actual:   " + QString::number(x) + "\n";
    msg += "  Expected: " + QString::number(y) + " (min: " + QString::number(y-lsb-DBL_EPSILON) + " , max: " + QString::number(y+lsb+DBL_EPSILON) + ")\n";
    msg += "  LSB:      " + QString::number(lsb) + " (" + QString::number(n) + " bit resolution) \n";
    msg += "  SRC: " + QString(file) + "(" + QString::number(line) + ")";
    QFAIL(msg.toAscii());
  }
}
