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
#ifndef MDT_BUFFER_TEST_H
#define MDT_BUFFER_TEST_H

#include "mdtTest.h"
#include "mdtBuffer.h"

/*
 * Template tests
 * Template classes cannot directely be called by QTestLib, so
 * we use another macro, VERIFY(), inside.
 * Then, the calls are made with the mdtBufferTestRun class.
 */

template <class T> class mdtBufferTest : public mdtTest
{
 public:

  // Initialisation tests
  bool initTest();

  // Test simple, cas idéal
  bool readWriteTest1();

  // Remplis par paquet , avec des quantités parfois trop grandes
  bool readWriteTest2();

  // Test le cas ou la zone de départ se situe ailleur qu'au début
  bool readWriteTest3();

  // Test le stockage avec arrêt sur une élément
  bool readWriteTestUntilToken();

  // Test le stockage avec arrêt sur une élément et l'ignorance des caractères null
  bool readWriteTestUntilTokenNullChars();

  // Test la copie avec le constructeur de copie
  bool copyTest();

  // Test le dépot par élément avec putOne()
  bool putOneTest();

  // Test la lecture par éléments avec takeOne()
  bool takeOneTest();

  // Test d'efficaticté comparé à memcpy() en utilisant la méthode put() pour le stockage
  bool timingTest(size_t iterationCount, size_t dataLen);

 protected:

  // Renvois un pointeur sur un tableau de len d'éléments
  //  contenant 0, 1, 2, 3, ...., len-1
  // Retourne un pointeur null en cas d'échec
  // La libération mémoire est à faire après fin d'utilisation
  T *dataNumeric(size_t len);

  // Compare 2 tableaux d'éléments
  // Renvois true si tous les éléments correspondent
  bool compare(T *a, T *b, size_t len);
  
  // Affiche les résultats d'une mesure de transfert et renvois le débit
  double displayDoubleLoopRateResults(string preMsg, size_t iterationCount, size_t elementsCountPerIteration, double workerLoopTime, double emptyLoopTime);
};

/*
 * This tests are called from QTestLib
 */

class mdtBufferTestRun : public mdtTest
{
 Q_OBJECT
 
 private slots:

  // Run tests with type char
  void charTypeTests();
};

#endif  // #ifndef MDT_BUFFER_TEST_H
