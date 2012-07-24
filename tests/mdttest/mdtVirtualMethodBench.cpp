/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtVirtualMethodBench.h"
#include <QCoreApplication>
#include <QElapsedTimer>

using namespace mdtVirtualMethodBenchNamespace;

/*
 * Complete class, to test direct calls
 */

volatile int complete::add(int a, int b)
{
  return a+b;
}

/*
 * Base class with virtual method
 */

volatile int base1::add(int a, int b)
{
  return a+b;
}

volatile int derived1::add(int a, int b)
{
  return a+b;
}

/*
 * Classes derived from Base class with pure virtual method
 */

volatile int derived2::add(int a, int b)
{
  return a+b;
}

volatile int derived22::add(int a, int b)
{
  return a+b;
}

volatile int derived23::add(int a, int b)
{
  return a+b;
}

/*
 * Test class
 */

void mdtVirtualMethodBench::initTestCase()
{
  randomValueInit();
}

void mdtVirtualMethodBench::pointerBasedBench()
{
  QElapsedTimer timer;
  qint64 dt1, dt2;
  qint64 iterationCount = 100000000;
  qint64 i;
  volatile int a, b, r;
  double directCallTime, callTime;

  complete *pc = new complete;
  base1 *pb1 = new base1;
  derived1 *pd1 = new derived1;
  base1 *pbd1 = new derived1;
  derived2 *pd2 = new derived2;
  derived22 *pd22 = new derived22;
  derived23 *pd23 = new derived23;

  // Set values
  a = randomValue(-1000000, 1000000);
  b = randomValue(-1000000, 1000000);

  /*
   * We use the double loop technic to mesure times
   * The first loop executes the full code, the second
   *  only the for() code.
   * At end, we substract empty loop measure to working loop measure.
   */

  // Bench the complete class (simple calls)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pc->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  directCallTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Direct call:";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << directCallTime << " [us] (" << 1000.0*directCallTime << " [ns])";

  // Bench the base1 class (direct call of virtual method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pb1->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Direct call of virtual method:";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived1 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pd1->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method (base is virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived1 class (call of derived method, with polymorph call)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pbd1->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Polymorph call (base is virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived2 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pd2->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived22 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pd22->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method in class derived22 (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived23 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pd23->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method in class derived23 (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  qDebug() << "Dont't be worry about result, it's just to use r variable, so r: " << r;

  // Free memory
  delete pc;
  delete pb1;
  delete pd1;
  delete pbd1;
  delete pd2;
  delete pd22;
  delete pd23;
}

void mdtVirtualMethodBench::stackBasedBench()
{
  QElapsedTimer timer;
  qint64 dt1, dt2;
  qint64 iterationCount = 100000000;
  qint64 i;
  volatile int a, b, r;
  double directCallTime, callTime;

  complete c;
  base1 b1;
  derived1 d1;
  derived2 d2;
  derived2 *pd2 = &d2;
  derived22 d22;
  derived23 d23;

  // Set values
  a = randomValue(-1000000, 1000000);
  b = randomValue(-1000000, 1000000);

  /*
   * We use the double loop technic to mesure times
   * The first loop executes the full code, the second
   *  only the for() code.
   * At end, we substract empty loop measure to working loop measure.
   */

  // Bench the complete class (simple calls)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    c.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  directCallTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Direct call:";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << directCallTime << " [us] (" << 1000.0*directCallTime << " [ns])";

  // Bench the base1 class (direct call of virtual method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    b1.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Direct call of virtual method:";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived1 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    d1.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method (base is virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived2 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    d2.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived2 class (call of derived method) - With a pointer to d2
  timer.start();
  for(i=0; i<iterationCount; ++i){
    pd2->add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method (base is pure virtual) with a pointer to d2:";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived22 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    d22.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method in class derived22 (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  // Bench the derived23 class (call of derived method)
  timer.start();
  for(i=0; i<iterationCount; ++i){
    d23.add(a, b);
  }
  dt1 = timer.elapsed();
  timer.start();
  for(i=0; i<iterationCount; ++i){
    r = a+b;
  }
  dt2 = timer.elapsed();
  callTime = 1000.0*(double)(dt1-dt2)/(double)iterationCount;
  qDebug() << "Call of inherited method in class derived23 (base is pure virtual):";
  qDebug() << "-> Procecced " << iterationCount/1000000 << " mio calls in " << 1000*(dt1-dt2) << " [us] (" << (dt1-dt2) << "[ms])";
  qDebug() << "-> Time per call: " << callTime << " [us] (" << 1000.0*callTime << " [ns])";
  if(directCallTime > 0.000001){
    qDebug() << "-> Direct call is " << callTime/directCallTime << "x faster";
  }

  qDebug() << "Dont't be worry about result, it's just to use r variable, so r: " << r;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  mdtVirtualMethodBench test;

  return QTest::qExec(&test, argc, argv);
}
