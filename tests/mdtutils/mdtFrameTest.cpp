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
#include "mdtFrameTest.h"
#include "mdtFrame.h"
#include "mdtFrameAscii.h"
#include "mdtFrameModbusTcp.h"
#include "mdtApplication.h"
#include <QTest>
#include <cstring>

void mdtFrameTest::initTest()
{
  mdtFrame f;

  // Verify default values
  QVERIFY2(f.capacity() == 0, "Verify that capacity matches");
  QVERIFY2(f.isEmpty(), "Verify initial empty flag");
  QVERIFY2(f.isFull(), "Verify initial full flag");
  QVERIFY2(f.remainCapacity() == 0, "Verify initial remain capacity");
  QVERIFY2(f.bytesToStore() == 0, "Verify initial bytes to store");
  QVERIFY2(!f.isComplete(), "Verify the complete flag");

  // Verify initial capacity
  f.reserve(10);
  QVERIFY2(f.capacity() == 10, "Verify that capacity matches");

  // Verify initial flags
  QVERIFY2(f.isEmpty(), "Verify initial empty flag");
  QVERIFY2(!f.isFull(), "Verify initial full flag");
  QVERIFY2(f.remainCapacity() == 10, "Verify initial remain capacity");
  QVERIFY2(f.bytesToStore() == 10, "Verify initial bytes to store");
  QVERIFY2(!f.isComplete(), "Verify the complete flag");

  // Verify values after clear
  f.clear();
  QVERIFY2(f.capacity() == 10, "Verify that capacity matches after clear");
  QVERIFY2(f.isEmpty(), "Verify empty flag after clear");
  QVERIFY2(!f.isFull(), "Verify full flag after clear");
  QVERIFY2(f.remainCapacity() == 10, "Verify remain capacity after clear");
  QVERIFY2(f.bytesToStore() == 10, "Verify bytes to store after clear");
  QVERIFY2(!f.isComplete(), "Verify the complete flag after clear");
}

void mdtFrameTest::putDataTest()
{
  mdtFrame f;
  mdtFrameAscii fa;
  char data[11] = "0123456789";

  /*
   * mdtFrame
   */
  f.reserve(5);
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  // Put a char
  QVERIFY(f.putData(data, 1) == 1);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 4);
  QVERIFY(f.bytesToStore() == 4);
  QVERIFY(f == "0");
  // Put 4 bytes
  QVERIFY(f.putData(&data[1], 4) == 4);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f == "01234");

  // Re-init
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  // Try to put to many data
  QVERIFY(f.putData(data, 7) == 5);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f == "01234");

  /*
   * mdtFrame (Raw)
   */
  f.setDirectlyComplete(true);
  f.clear();

  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  // Put a char
  QVERIFY(f.putData(data, 1) == 1);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 4);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "0");
  // Put 4 bytes
  QVERIFY(f.putData(&data[1], 4) == 4);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01234");

  // Re-init
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  // Try to put to many data
  QVERIFY(f.putData(data, 7) == 5);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01234");

  /*
   * mdtFrameAscii
   */
  fa.reserve(5);

  // Initial values
  QVERIFY(fa.isEmpty());
  QVERIFY(!fa.isFull());
  QVERIFY(fa.remainCapacity() == 5);
  QVERIFY(fa.bytesToStore() == 5);
  // Put a char
  QVERIFY(fa.putData(data, 1) == 1);
  // Check values
  QVERIFY(!fa.isEmpty());
  QVERIFY(!fa.isFull());
  QVERIFY(fa.remainCapacity() == 4);
  QVERIFY(fa.bytesToStore() == 4);
  QVERIFY(fa == "0");
  // Put 4 bytes
  QVERIFY(fa.putData(&data[1], 4) == 4);
  // Check values
  QVERIFY(!fa.isEmpty());
  QVERIFY(fa.isFull());
  QVERIFY(fa.remainCapacity() == 0);
  QVERIFY(fa.bytesToStore() == 0);
  QVERIFY(fa == "01234");

  // Re-init
  fa.clear();
  QVERIFY(fa.isEmpty());
  QVERIFY(!fa.isFull());
  QVERIFY(fa.remainCapacity() == 5);
  QVERIFY(fa.bytesToStore() == 5);
  // Try to put to many data
  QVERIFY(fa.putData(data, 7) == 5);
  // Check values
  QVERIFY(!fa.isEmpty());
  QVERIFY(fa.isFull());
  QVERIFY(fa.remainCapacity() == 0);
  QVERIFY(fa.bytesToStore() == 0);
  QVERIFY(fa == "01234");
}

void mdtFrameTest::asciiReceptionTest()
{
  mdtFrame f;
  char data[6] = "01234";
  char eof;

  f.reserve(5);

  /* Put data. EOF token is the * char
   * Test case with EOF reached before frame is full
   */
  eof = '*';
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  // Put a char
  QVERIFY(f.putUntil(data, '*', 1) == 1);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 4);
  QVERIFY(f.bytesToStore() == 4);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0");
  // Put 2 chars
  QVERIFY(f.putUntil(&data[1], '*', 2) == 2);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 2);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "012");
  // Put EOF char
  QVERIFY(f.putUntil(&eof, '*', 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Put another char, must not be stored
  QVERIFY(f.putUntil(data, '*', 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is the * char
   * Test case with EOF reached exactly 1 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, '*', 4) == 4);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 1);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123");
  // Add the EOF char and check values
  QVERIFY(f.putUntil(&eof, '*', 1) == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "0123");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is the * char
   * Test case with EOF never reached
   */
  // Put data
  QVERIFY(f.putUntil(data, '*', 5) == 5);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Try to put more data
  QVERIFY(f.putUntil(data, '*', 5) == 0);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
}

void mdtFrameTest::asciiReceptionIgnoreNullValuesTest()
{
  mdtFrame f;
  char data[6] = "01234";
  char eof;
  char nullData[6] = {'\0','\0','\0','\0','\0','\0'};

  f.reserve(5);

  /* Put data. EOF token is the * char
   * Test case with EOF reached before frame is full
   */
  eof = '*';
  // Initial values
  f.setIgnoreNullValues(true);
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  // Put a char
  QVERIFY(f.putUntil(data, '*', 1) == 1);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 4);
  QVERIFY(f.bytesToStore() == 4);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0");
  // Put null values and chek that values are not stored
  QVERIFY(f.putUntil(nullData, '*', 3) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 4);
  QVERIFY(f.bytesToStore() == 4);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0");
  // Put 2 chars
  QVERIFY(f.putUntil(&data[1], '*', 2) == 2);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 2);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "012");
  // Put EOF char
  QVERIFY(f.putUntil(&eof, '*', 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Put another char, must not be stored
  QVERIFY(f.putUntil(data, '*', 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is the * char
   * Test case with EOF reached exactly 1 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, '*', 4) == 4);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 1);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123");
  // Add the EOF char and check values
  QVERIFY(f.putUntil(&eof, '*', 1) == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "0123");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is the * char
   * Test case with EOF never reached
   */
  // Put data
  QVERIFY(f.putUntil(data, '*', 5) == 5);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Try to put more data
  QVERIFY(f.putUntil(data, '*', 5) == 0);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
}

void mdtFrameTest::asciiReceptionEofStrTest()
{
  mdtFrame f;
  char data[6] = "01234";
  QByteArray eof;

  f.reserve(5);

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF reached 3 byte before frame becomes full
   */
  eof = "*-";
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  // Put 2 chars
  QVERIFY(f.putUntil(data, eof, 2) == 2);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 3);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01");
  // Put EOF char
  QVERIFY(f.putUntil(eof.data(), eof, 2) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01");
  // Put another char, must not be stored
  QVERIFY(f.putUntil(data, eof, 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with part of EOF reached exactly 1 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 4) == 4);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 1);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123");
  // Add a part of EOF sequence and check values
  QVERIFY(f.putUntil(eof.data(), eof, 1) == 1);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123*");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF reached exactly 2 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 3) == 3);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 2);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "012");
  // Add EOF sequence and check values
  QVERIFY(f.putUntil(eof.data(), eof, 2) == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF never reached
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 5) == 5);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Try to put more data
  QVERIFY(f.putUntil(data, eof, 5) == 0);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
}

void mdtFrameTest::asciiReceptionEofStrIgnoreNullValuesTest()
{
  mdtFrame f;
  char data[6] = "01234";
  QByteArray eof;
  char nullData[6] = {'\0','\0','\0','\0','\0','\0'};

  f.reserve(5);

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF reached 3 byte before frame becomes full
   */
  eof = "*-";
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  f.setIgnoreNullValues(true);
  // Put 2 chars
  QVERIFY(f.putUntil(data, eof, 2) == 2);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 3);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01");
  // Put null values and chek that values are not stored
  QVERIFY(f.putUntil(nullData, eof, 2) == 0);
  // Put EOF char
  QVERIFY(f.putUntil(eof.data(), eof, 2) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01");
  // Put another char, must not be stored
  QVERIFY(f.putUntil(data, eof, 1) == 0);
  // Check values
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "01");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with part of EOF reached exactly 1 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 4) == 4);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 1);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123");
  // Add a part of EOF sequence and check values
  QVERIFY(f.putUntil(eof.data(), eof, 1) == 1);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "0123*");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF reached exactly 2 byte before frame becomes full
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 3) == 3);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 2);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "012");
  // Add EOF sequence and check values
  QVERIFY(f.putUntil(eof.data(), eof, 2) == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 2);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f == "012");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());

  /* Put data. EOF token is "*-" sequence
   * Test case with EOF never reached
   */
  // Put data
  QVERIFY(f.putUntil(data, eof, 5) == 5);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Try to put more data
  QVERIFY(f.putUntil(data, eof, 5) == 0);
  // check flags
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.isFull());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f == "01234");
  // Clear the frame and check values
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(!f.isFull());
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());  
}

void mdtFrameTest::takeDataTest()
{
  mdtFrame f;
  char data[6] = "01234";
  char destData[6] = {'\0'};

  f.reserve(5);
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  
  // Store some data and check values
  f.append(data);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f == "01234");

  // Take some data and check values
  QVERIFY(f.take(destData, 1) == 1);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(strcmp(destData, "0") == 0);
  QVERIFY(f == "1234");

  // Take some data and check values
  QVERIFY(f.take(destData, f.size()) == 4);
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(strcmp(destData, "1234") == 0);
  QVERIFY(f == "");
  
  // Try to take data from empty frame
  destData[0] = '\0';
  destData[1] = '\0';
  destData[2] = '\0';
  QVERIFY(f.take(destData, 3) == 0);
  QVERIFY(f.isEmpty());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(strcmp(destData, "") == 0);
  QVERIFY(f == "");
}

void mdtFrameTest::modbusTcpEncodeTest()
{
  mdtFrameModbusTcp f;
  QByteArray pdu;

  f.reserve(10);
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 10);
  QVERIFY(!f.isComplete());
  QVERIFY(f.modbusLength() == 0);

  // MBAP Header - Default Unit ID (0xFF)
  f.setTransactionId(0x0205);
  f.encode();
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.modbusLength() == 1);
  QVERIFY(f.at(0) == 0x02);
  QVERIFY(f.at(1) == 0x05);
  QVERIFY(f.at(2) == 0);
  QVERIFY(f.at(3) == 0);
  QVERIFY(f.at(4) == 0x00);
  QVERIFY(f.at(5) == 0x01);
  QVERIFY((quint8)f.at(6) == 0xFF);


  // MBAP Header
  f.setTransactionId(0x1234);
  f.setUnitId(0x25);
  f.encode();
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.modbusLength() == 1);
  QVERIFY(f.at(0) == 0x12);
  QVERIFY(f.at(1) == 0x34);
  QVERIFY(f.at(2) == 0);
  QVERIFY(f.at(3) == 0);
  QVERIFY(f.at(4) == 0x00);
  QVERIFY(f.at(5) == 0x01);
  QVERIFY(f.at(6) == 0x25);
  // MBAP Header + PDU
  f.setTransactionId(0x0576);
  f.setUnitId(0x42);
  pdu = "abc";
  f.setPdu(pdu);
  f.encode();
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.at(0) == 0x05);
  QVERIFY(f.at(1) == 0x76);
  QVERIFY(f.at(2) == 0);
  QVERIFY(f.at(3) == 0);
  QVERIFY(f.at(4) == 0x00);
  QVERIFY(f.at(5) == 0x04);
  QVERIFY(f.at(6) == 0x42);
  QVERIFY(f.at(7) == 'a');
  QVERIFY(f.at(8) == 'b');
  QVERIFY(f.at(9) == 'c');

/*
  qDebug() << "f[0]: 0x" << hex << (int)f.at(0);
  qDebug() << "f[1]: 0x" << hex << (int)f.at(1);
  qDebug() << "f[2]: 0x" << hex << (int)f.at(2);
  qDebug() << "f[3]: 0x" << hex << (int)f.at(3);
  qDebug() << "f[4]: 0x" << hex << (int)f.at(4);
  qDebug() << "f[5]: 0x" << hex << (int)f.at(5);
  qDebug() << "f[6]: 0x" << hex << (int)f.at(6);
  qDebug() << "f[7]: " << f.at(7);
  qDebug() << "f[8]: " << f.at(8);
  qDebug() << "f[9]: " << f.at(9);
*/
}

void mdtFrameTest::modbusTcpDecodeTest()
{
  mdtFrameModbusTcp f;
  QByteArray srcData;
  char c;

  f.reserve(10);
  // Initial values
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 10);
  QVERIFY(!f.isComplete());
  QVERIFY(f.modbusLength() == 0);

  // Build the frame
  srcData.clear();
  // Transaction ID
  srcData.append(0x10);
  srcData.append(0x52);
  // Protocol ID
  srcData.append((char)0);
  srcData.append((char)0);
  // Length: 4
  srcData.append((char)0);
  srcData.append(0x04);
  // Unit ID
  srcData.append(0x16);
  // PDU
  srcData.append("123");
  QVERIFY(srcData.size() == 10);

  /*
   * Cpapacity == MODBUS frame size
   */

  // Check 1 pass store
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 10);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 10);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");

  // Check byte per byte store
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 10);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  for(int i=0; i<srcData.size(); i++){
    c = srcData.at(i);
    f.putData(&c, 1);
  }
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");

  // Check variable size store - WW: alter srcData
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 10);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Put some data
  QVERIFY(f.putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 8);
  QVERIFY(f.bytesToStore() == 8);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Put some data
  QVERIFY(f.putData(srcData.data(), 3) == 3);
  srcData.remove(0, 3);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 5);
  QVERIFY(f.bytesToStore() == 5);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Put some data - After this putData() call, frame's length can be decoded
  QVERIFY(f.putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 3);
  QVERIFY(f.bytesToStore() == 3);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 1);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Put some data
  QVERIFY(f.putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 1);
  QVERIFY(f.bytesToStore() == 1);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 3);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Put some data
  QVERIFY(f.putData(srcData.data(), 1) == 1);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(f.transactionId() == 0x1052);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x16);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "123");

  /*
   * Cpapacity > MODBUS frame size
   */

  // Build the frame
  srcData.clear();
  // Transaction ID
  srcData.append(0x53);
  srcData.append(0x76);
  // Protocol ID
  srcData.append((char)0);
  srcData.append((char)0);
  // Length: 4
  srcData.append((char)0);
  srcData.append(0x04);
  // Unit ID
  srcData.append(0x95);
  // PDU
  srcData.append("ABC");
  QVERIFY(srcData.size() == 10);

  // Init frame
  f.reserve(20);

  // Check 1 pass store
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 20);
  QVERIFY(f.bytesToStore() == 20);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 10);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f.transactionId() == 0x5376);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x95);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "ABC");
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(f.transactionId() == 0x5376);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x95);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "ABC");

  // Check byte per byte store
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 20);
  QVERIFY(f.bytesToStore() == 20);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  for(int i=0; i<srcData.size(); i++){
    c = srcData.at(i);
    f.putData(&c, 1);
  }
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 10);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(f.isComplete());
  QVERIFY(f.transactionId() == 0x5376);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x95);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "ABC");
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(f.transactionId() == 0x5376);
  QVERIFY(f.modbusLength() == 4);
  QVERIFY(f.unitId() == 0x95);
  QVERIFY(f.getPdu().size() == 3);
  QVERIFY(f.getPdu() == "ABC");

  /*
   * Cpapacity < MODBUS frame size
   * Can happen if frame size field is wrong,
   *  (or user application does a mistake on init with a size < 263)
   */

  // Build the frame
  srcData.clear();
  // Transaction ID
  srcData.append(0x53);
  srcData.append(0x76);
  // Protocol ID
  srcData.append((char)0);
  srcData.append((char)0);
  // Length: 27
  srcData.append((char)0);
  srcData.append(0x1B);
  // Unit ID
  srcData.append(0x95);
  // PDU
  srcData.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  QVERIFY(srcData.size() == 33);

  // Init frame
  f.reserve(20);

  // Check 1 pass store
  f.clear();
  QVERIFY(f.isEmpty());
  QVERIFY(!f.isFull());
  QVERIFY(f.remainCapacity() == 20);
  QVERIFY(f.bytesToStore() == 20);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 0);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 14);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
  // Try to put more - must no work
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 14);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);


  /*
   * Check with invalid data
   */

  // Data
  srcData = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QVERIFY(srcData.size() > 20);
  
  // Init frame
  f.reserve(20);
  f.clear();
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 14);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
 
  /*
   * Check with invalid frame size
   */

  // Data
  srcData = "abcd";
  srcData.append(0xFF); // Frame size H
  srcData.append(0xFF); // Frame size L
  srcData = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QVERIFY(srcData.size() > 20);
  
  // Init frame
  f.reserve(20);
  f.clear();
  QVERIFY(f.putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isFull());
  QVERIFY(f.remainCapacity() == 0);
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isComplete());
  QVERIFY(f.transactionId() == 0);
  QVERIFY(f.modbusLength() == 14);
  QVERIFY(f.unitId() == 0xFF);
  QVERIFY(f.getPdu().size() == 0);
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtFrameTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
