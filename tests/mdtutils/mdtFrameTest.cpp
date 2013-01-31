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
#include "mdtFrameUsbTmc.h"
#include "mdtFrameUsbControl.h"
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
  f.clear();
  f.clearSub();
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
  f.clear();
  f.clearSub();
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
  f.clear();
  f.clearSub();
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
  qDebug() << "HEX string: " << f.toHex();

}

void mdtFrameTest::modbusTcpDecodeTest()
{
  mdtFrameModbusTcp *mf = new mdtFrameModbusTcp;
  mdtFrame *f = mf;   // Check the subclassing problem
  QByteArray srcData;
  char c;

  f->reserve(10);
  // Initial values
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->modbusLength() == 0);

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
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 10);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(f->isComplete());
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");

  // Check byte per byte store
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  for(int i=0; i<srcData.size(); i++){
    c = srcData.at(i);
    f->putData(&c, 1);
  }
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(f->isComplete());
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");

  // Check variable size store - WW: alter srcData
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Put some data
  QVERIFY(f->putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 8);
  QVERIFY(f->bytesToStore() == 8);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Put some data
  QVERIFY(f->putData(srcData.data(), 3) == 3);
  srcData.remove(0, 3);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 5);
  QVERIFY(f->bytesToStore() == 5);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Put some data - After this putData() call, frame's length can be decoded
  QVERIFY(f->putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 3);
  QVERIFY(f->bytesToStore() == 3);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 1);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Put some data
  QVERIFY(f->putData(srcData.data(), 2) == 2);
  srcData.remove(0, 2);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 1);
  QVERIFY(f->bytesToStore() == 1);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 3);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Put some data
  QVERIFY(f->putData(srcData.data(), 1) == 1);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(f->isComplete());
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(mf->transactionId() == 0x1052);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x16);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "123");

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
  f->reserve(20);

  // Check 1 pass store
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 20);
  QVERIFY(f->bytesToStore() == 20);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 10);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(f->isComplete());
  QVERIFY(mf->transactionId() == 0x5376);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x95);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "ABC");
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(mf->transactionId() == 0x5376);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x95);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "ABC");

  // Check byte per byte store
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 20);
  QVERIFY(f->bytesToStore() == 20);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  for(int i=0; i<srcData.size(); i++){
    c = srcData.at(i);
    f->putData(&c, 1);
  }
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 10);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(f->isComplete());
  QVERIFY(mf->transactionId() == 0x5376);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x95);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "ABC");
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(mf->transactionId() == 0x5376);
  QVERIFY(mf->modbusLength() == 4);
  QVERIFY(mf->unitId() == 0x95);
  QVERIFY(mf->getPdu().size() == 3);
  QVERIFY(mf->getPdu() == "ABC");

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
  f->reserve(20);

  // Check 1 pass store
  f->clear();
  f->clearSub();
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 20);
  QVERIFY(f->bytesToStore() == 20);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 0);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 14);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
  // Try to put more - must no work
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 14);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);


  /*
   * Check with invalid data
   */

  // Data
  srcData = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QVERIFY(srcData.size() > 20);
  
  // Init frame
  f->reserve(20);
  f->clear();
  f->clearSub();
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 14);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);
 
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
  f->reserve(20);
  f->clear();
  f->clearSub();
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 20);
  QVERIFY(!f->isEmpty());
  QVERIFY(f->isFull());
  QVERIFY(f->remainCapacity() == 0);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isComplete());
  QVERIFY(mf->transactionId() == 0);
  QVERIFY(mf->modbusLength() == 14);
  QVERIFY(mf->unitId() == 0xFF);
  QVERIFY(mf->getPdu().size() == 0);

  /*
   * Transaction ID bug (2012-11-13)
   */

  // Build the frame
  srcData.clear();
  // Transaction ID
  srcData.append((char)0x00);
  srcData.append(127);
  // Protocol ID
  srcData.append((char)0);
  srcData.append((char)0);
  // Length: 1
  srcData.append((char)0);
  srcData.append((char)1);
  // Unit ID
  srcData.append(0x95);
  // PDU
  ///srcData.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  QVERIFY(srcData.size() == 7);

  // Init frame
  f->reserve(20);
  f->clear();
  f->clearSub();
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 7);
  QVERIFY(!f->isEmpty());
  qDebug() << "TR ID: " << mf->transactionId();
  QVERIFY(mf->transactionId() == 127);

  // Build the frame
  srcData.clear();
  // Transaction ID
  srcData.append((char)0x00);
  srcData.append(128);
  // Protocol ID
  srcData.append((char)0);
  srcData.append((char)0);
  // Length: 1
  srcData.append((char)0);
  srcData.append((char)1);
  // Unit ID
  srcData.append(0x95);
  // PDU
  ///srcData.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  QVERIFY(srcData.size() == 7);

  // Init frame
  f->reserve(20);
  f->clear();
  f->clearSub();
  QVERIFY(f->putData(srcData.data(), srcData.size()) == 7);
  QVERIFY(!f->isEmpty());
  qDebug() << "TR ID: " << mf->transactionId();
  QVERIFY(mf->transactionId() == 128);

  delete mf;
}

void mdtFrameTest::usbTmcEncodeTest()
{
  mdtFrameUsbTmc f;

  // Initial values
  QVERIFY(f.MsgID() == mdtFrameUsbTmc::MSG_ZERO);
  QVERIFY(f.bTag() == 0);
  QVERIFY(f.isEOM());

  // Build a Bulk-OUT frame (No end alignment bytes)
  f.setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  f.setbTag(1);
  f.setMessageData("1234");
  f.encode();
  // Check flags
  QVERIFY(f.size() == 16);
  QVERIFY(f.MsgID() == 1);
  QVERIFY(f.bTag() == 1);
  QVERIFY(f.isEOM());
  // Check frame
  QVERIFY((quint8)f.at(0) == 1);      // MsgID
  QVERIFY((quint8)f.at(1) == 1);      // bTag
  QVERIFY((quint8)f.at(2) == 0xFE);   // bTagInverse
  QVERIFY((quint8)f.at(3) == 0);      // Reserved
  QVERIFY((quint8)f.at(4) == 4);      // TransferSize, LLSB
  QVERIFY((quint8)f.at(5) == 0);      // TransferSize, LSB
  QVERIFY((quint8)f.at(6) == 0);      // TransferSize, MSB
  QVERIFY((quint8)f.at(7) == 0);      // TransferSize, MMSB
  QVERIFY((quint8)f.at(8) == 0x01);   // bmTransferAttributes
  QVERIFY((quint8)f.at(9) == 0x00);   // Reserved
  QVERIFY((quint8)f.at(10) == 0x00);  // Reserved
  QVERIFY((quint8)f.at(11) == 0x00);  // Reserved
  QVERIFY(f.at(12) == '1');   // Message data
  QVERIFY(f.at(13) == '2');   // Message data
  QVERIFY(f.at(14) == '3');   // Message data
  QVERIFY(f.at(15) == '4');   // Message data

  // Build a Bulk-OUT frame (With end alignment bytes)
  f.setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  f.setbTag(2);
  f.setMessageData("ABC");
  f.encode();
  // Check flags
  QVERIFY(f.size() == 16);
  QVERIFY(f.MsgID() == 1);
  QVERIFY(f.bTag() == 2);
  QVERIFY(f.isEOM());
  // Check frame
  QVERIFY((quint8)f.at(0) == 1);      // MsgID
  QVERIFY((quint8)f.at(1) == 2);      // bTag
  QVERIFY((quint8)f.at(2) == 0xFD);   // bTagInverse
  QVERIFY((quint8)f.at(3) == 0);      // Reserved
  QVERIFY((quint8)f.at(4) == 3);      // TransferSize, LLSB
  QVERIFY((quint8)f.at(5) == 0);      // TransferSize, LSB
  QVERIFY((quint8)f.at(6) == 0);      // TransferSize, MSB
  QVERIFY((quint8)f.at(7) == 0);      // TransferSize, MMSB
  QVERIFY((quint8)f.at(8) == 0x01);   // bmTransferAttributes
  QVERIFY((quint8)f.at(9) == 0x00);   // Reserved
  QVERIFY((quint8)f.at(10) == 0x00);  // Reserved
  QVERIFY((quint8)f.at(11) == 0x00);  // Reserved
  QVERIFY(f.at(12) == 'A');   // Message data
  QVERIFY(f.at(13) == 'B');   // Message data
  QVERIFY(f.at(14) == 'C');   // Message data
  QVERIFY(f.at(15) == 0x00);  // Alignment byte (not requierd to be 0x00, but should)

  /// \todo Add frame with sizes: 20, 40, 70
  // Build a Bulk-OUT frame (Size: 20 , 0x14)
  f.setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  f.setbTag(3);
  f.setMessageData(QByteArray(20, 'A'));
  f.encode();
  // Check flags
  QVERIFY(f.size() == 32);
  QVERIFY(f.MsgID() == 1);
  QVERIFY(f.bTag() == 3);
  QVERIFY(f.isEOM());
  // Check frame
  QVERIFY((quint8)f.at(0) == 1);      // MsgID
  QVERIFY((quint8)f.at(1) == 3);      // bTag
  QVERIFY((quint8)f.at(2) == 0xFC);   // bTagInverse
  QVERIFY((quint8)f.at(3) == 0);      // Reserved
  QVERIFY((quint8)f.at(4) == 20);     // TransferSize, LLSB
  QVERIFY((quint8)f.at(5) == 0);      // TransferSize, LSB
  QVERIFY((quint8)f.at(6) == 0);      // TransferSize, MSB
  QVERIFY((quint8)f.at(7) == 0);      // TransferSize, MMSB
  QVERIFY((quint8)f.at(8) == 0x01);   // bmTransferAttributes
  QVERIFY((quint8)f.at(9) == 0x00);   // Reserved
  QVERIFY((quint8)f.at(10) == 0x00);  // Reserved
  QVERIFY((quint8)f.at(11) == 0x00);  // Reserved
  QVERIFY(f.at(12) == 'A');   // Message data
  QVERIFY(f.at(19) == 'A');   // Message data

  // Build a Bulk-OUT frame (Size: 4672 , 0x1240)
  f.setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  f.setbTag(4);
  f.setMessageData(QByteArray(4672, 'B'));
  f.encode();
  // Check flags
  QVERIFY(f.size() == 4684);
  QVERIFY(f.MsgID() == 1);
  QVERIFY(f.bTag() == 4);
  QVERIFY(f.isEOM());
  // Check frame
  QVERIFY((quint8)f.at(0) == 1);      // MsgID
  QVERIFY((quint8)f.at(1) == 4);      // bTag
  QVERIFY((quint8)f.at(2) == 0xFB);   // bTagInverse
  QVERIFY((quint8)f.at(3) == 0);      // Reserved
  QVERIFY((quint8)f.at(4) == 0x40);   // TransferSize, LLSB
  QVERIFY((quint8)f.at(5) == 0x12);   // TransferSize, LSB
  QVERIFY((quint8)f.at(6) == 0);      // TransferSize, MSB
  QVERIFY((quint8)f.at(7) == 0);      // TransferSize, MMSB
  QVERIFY((quint8)f.at(8) == 0x01);   // bmTransferAttributes
  QVERIFY((quint8)f.at(9) == 0x00);   // Reserved
  QVERIFY((quint8)f.at(10) == 0x00);  // Reserved
  QVERIFY((quint8)f.at(11) == 0x00);  // Reserved
  QVERIFY(f.at(12) == 'B');   // Message data
  QVERIFY(f.at(4683) == 'B');   // Message data

  // Build a read request frame: DEV_DEP_MSG_IN
  f.setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  f.setbTag(5);
  f.setTransferSize(0x12345678);
  f.encode();
  // Check frame
  QCOMPARE(f.size(), 12);
  QVERIFY((quint8)f.at(0) == 2);      // MsgID
  QVERIFY((quint8)f.at(1) == 5);      // bTag
  QVERIFY((quint8)f.at(2) == 0xFA);   // bTagInverse
  QVERIFY((quint8)f.at(3) == 0);      // Reserved
  QVERIFY((quint8)f.at(4) == 0x78);   // TransferSize, LLSB
  QVERIFY((quint8)f.at(5) == 0x56);   // TransferSize, LSB
  QVERIFY((quint8)f.at(6) == 0x34);   // TransferSize, MSB
  QVERIFY((quint8)f.at(7) == 0x12);   // TransferSize, MMSB
  // We not support term char for now
  QVERIFY((quint8)f.at(8) == 0x00);   // bmTransferAttributes
  QVERIFY((quint8)f.at(9) == 0x00);   // TermChar
  QVERIFY((quint8)f.at(10) == 0x00);  // Reserved
  QVERIFY((quint8)f.at(11) == 0x00);  // Reserved

  /// \todo Add size tests (very big sizes :) )
}

void mdtFrameTest::usbTmcDecodeTest()
{
  mdtFrameUsbTmc *uf = new mdtFrameUsbTmc;
  mdtFrame *f = uf; // Check inheritance problem same time
  QByteArray srcData;
  QByteArray subData;
  char c;

  /*
   * Simple frame: No alignement bytes
   * Capacity == frame size (data+header)
   * Single transfer: EOM set
   * TermChar: not set
   */

  f->reserve(16);
  // Initial values
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 16);
  QVERIFY(f->bytesToStore() == 16);
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  QVERIFY(!uf->bTagOk());
  QVERIFY(!uf->MsgIDsupported());
  // Build the frame
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0xF7);                           // bTag
  srcData.append((char)0x08);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)4);                              // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append('A');                                  // Message data
  srcData.append('B');                                  // Message data
  srcData.append('C');                                  // Message data
  srcData.append('D');                                  // Message data
  QVERIFY(srcData.size() == 16);

  /* Put byte per byte */

  // MsgID
  c = srcData.at(0);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 1);
  QVERIFY(f->bytesToStore() == 15);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // bTag
  c = srcData.at(1);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 2);
  QVERIFY(f->bytesToStore() == 14);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // bTagInverse
  c = srcData.at(2);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 3);
  QVERIFY(f->bytesToStore() == 13);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Reserved
  c = srcData.at(3);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 4);
  QVERIFY(f->bytesToStore() == 12);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Transfer size, LLSB
  c = srcData.at(4);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 5);
  QVERIFY(f->bytesToStore() == 11);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Transfer size, LSB
  c = srcData.at(5);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 6);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Transfer size, MSB
  c = srcData.at(6);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 7);
  QVERIFY(f->bytesToStore() == 9);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Transfer size, MMSB
  c = srcData.at(7);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 8);
  QVERIFY(f->bytesToStore() == 8);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // bmTransferAttributes
  c = srcData.at(8);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 9);
  QVERIFY(f->bytesToStore() == 7);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Reserved
  c = srcData.at(9);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 10);
  QVERIFY(f->bytesToStore() == 6);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Reserved
  c = srcData.at(10);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 11);
  QVERIFY(f->bytesToStore() == 5);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Reserved
  c = srcData.at(11);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 12);
  QVERIFY(f->bytesToStore() == 4);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Data
  c = srcData.at(12);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 13);
  QVERIFY(f->bytesToStore() == 3);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(13);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 14);
  QVERIFY(f->bytesToStore() == 2);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(14);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 15);
  QVERIFY(f->bytesToStore() == 1);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(15);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 16);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  // Now we should have a complet frame
  QVERIFY(f->isComplete());
  QVERIFY(uf->messageData() == "ABCD");
  QVERIFY(uf->MsgID() == 2);            // MsgID: DEV_DEP_MSG_IN
  QVERIFY(uf->isEOM());
  QVERIFY(uf->bTag() == 0xF7);
  QVERIFY(uf->bTagOk());
  QVERIFY(uf->MsgIDsupported());

  /*
   * Simple frame: With alignement bytes
   * Capacity == frame size (header + data + alignement bytes)
   * Single transfer: EOM set
   * TermChar: not set
   */

  f->reserve(20);
  f->clear();
  f->clearSub();
  // Initial values
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 20);
  QVERIFY(f->bytesToStore() == 20);
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  QVERIFY(!uf->bTagOk());
  QVERIFY(!uf->MsgIDsupported());
  // Build the frame
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0xF8);                           // bTag
  srcData.append((char)0x07);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)5);                              // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append('1');                                  // Message data
  srcData.append('2');                                  // Message data
  srcData.append('3');                                  // Message data
  srcData.append('4');                                  // Message data
  srcData.append('5');                                  // Message data
  srcData.append((char)0);                              // Alignment byte
  srcData.append((char)0);                              // Alignment bytes
  srcData.append((char)0);                              // Alignment byte
  QVERIFY(srcData.size() == 20);

  /* Put byte per byte */

  // MsgID
  c = srcData.at(0);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 1);
  QVERIFY(f->bytesToStore() == 19);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bTag
  c = srcData.at(1);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 2);
  QVERIFY(f->bytesToStore() == 18);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(!uf->bTagOk());
  // bTagInverse
  c = srcData.at(2);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 3);
  QVERIFY(f->bytesToStore() == 17);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(3);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 4);
  QVERIFY(f->bytesToStore() == 16);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LLSB
  c = srcData.at(4);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 5);
  QVERIFY(f->bytesToStore() == 15);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LSB
  c = srcData.at(5);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 6);
  QVERIFY(f->bytesToStore() == 14);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MSB
  c = srcData.at(6);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 7);
  QVERIFY(f->bytesToStore() == 13);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MMSB
  c = srcData.at(7);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 8);
  QVERIFY(f->bytesToStore() == 12);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bmTransferAttributes
  c = srcData.at(8);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 9);
  QVERIFY(f->bytesToStore() == 11);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(9);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(10);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 11);
  QVERIFY(f->bytesToStore() == 9);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(11);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 12);
  QVERIFY(f->bytesToStore() == 8);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(12);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 13);
  QVERIFY(f->bytesToStore() == 7);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(13);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 14);
  QVERIFY(f->bytesToStore() == 6);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(14);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 15);
  QVERIFY(f->bytesToStore() == 5);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(15);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 16);
  QVERIFY(f->bytesToStore() == 4);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  /*
   * Now, store last data byte with alignement bytes.
   * Storing a alignement byte alone is not a realistic case
   */
  subData = srcData.right(4);
  QVERIFY(f->putData(subData.data(), 4) == 4);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  // Now we should have a complete frame
  QVERIFY(f->isComplete());
  QVERIFY(uf->messageData() == "12345");
  QVERIFY(uf->MsgID() == 2);            // MsgID: DEV_DEP_MSG_IN
  QVERIFY(uf->isEOM());
  QVERIFY(uf->bTag() == 0xF8);
  QVERIFY(uf->bTagOk());
  QVERIFY(uf->MsgIDsupported());

  /*
   * Capacity < frame size (header + data + alignement bytes)
   * Single transfer: EOM set
   * TermChar: not set
   */

  f->reserve(20);
  f->clear();
  f->clearSub();
  // Initial values
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 20);
  QVERIFY(f->bytesToStore() == 20);
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Build the frame
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0xF8);                           // bTag
  srcData.append((char)0x07);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)10);                             // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append('a');                                  // Message data
  srcData.append('b');                                  // Message data
  srcData.append('c');                                  // Message data
  srcData.append('d');                                  // Message data
  srcData.append('e');                                  // Message data
  srcData.append('f');                                  // Message data
  srcData.append('g');                                  // Message data
  srcData.append('h');                                  // Message data
  srcData.append('i');                                  // Message data
  srcData.append('j');                                  // Message data
  srcData.append((char)0);                              // Alignment byte
  srcData.append((char)0);                              // Alignment bytes
  QVERIFY(srcData.size() == 24);

  /* Put byte per byte */

  // MsgID
  c = srcData.at(0);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 1);
  QVERIFY(f->bytesToStore() == 19);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bTag
  c = srcData.at(1);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 2);
  QVERIFY(f->bytesToStore() == 18);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bTagInverse
  c = srcData.at(2);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 3);
  QVERIFY(f->bytesToStore() == 17);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(3);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 4);
  QVERIFY(f->bytesToStore() == 16);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LLSB
  c = srcData.at(4);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 5);
  QVERIFY(f->bytesToStore() == 15);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LSB
  c = srcData.at(5);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 6);
  QVERIFY(f->bytesToStore() == 14);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MSB
  c = srcData.at(6);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 7);
  QVERIFY(f->bytesToStore() == 13);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MMSB
  c = srcData.at(7);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 8);
  QVERIFY(f->bytesToStore() == 12);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bmTransferAttributes
  c = srcData.at(8);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 9);
  QVERIFY(f->bytesToStore() == 11);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(9);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 10);
  QVERIFY(f->bytesToStore() == 10);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(10);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 11);
  QVERIFY(f->bytesToStore() == 9);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(11);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 12);
  QVERIFY(f->bytesToStore() == 8);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data a
  c = srcData.at(12);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 13);
  QVERIFY(f->bytesToStore() == 7);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data b
  c = srcData.at(13);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 14);
  QVERIFY(f->bytesToStore() == 6);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data c
  c = srcData.at(14);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 15);
  QVERIFY(f->bytesToStore() == 5);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data d
  c = srcData.at(15);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 16);
  QVERIFY(f->bytesToStore() == 4);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data e
  c = srcData.at(16);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 17);
  QVERIFY(f->bytesToStore() == 3);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data f
  c = srcData.at(17);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 18);
  QVERIFY(f->bytesToStore() == 2);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data g
  c = srcData.at(18);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 19);
  QVERIFY(f->bytesToStore() == 1);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data h - Here frame becomes full, but not complete
  c = srcData.at(19);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Data i - Must not work
  c = srcData.at(20);
  QVERIFY(f->putData(&c, 1) == 0);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Data j - Must not work
  c = srcData.at(21);
  QVERIFY(f->putData(&c, 1) == 0);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Alignment byte - Must not work
  c = srcData.at(22);
  QVERIFY(f->putData(&c, 1) == 0);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Alignment byte - Must not work
  c = srcData.at(23);
  QVERIFY(f->putData(&c, 1) == 0);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");

  /*
   * Simple frame: With alignement bytes
   * Capacity > frame size (header + data + alignement bytes)
   * Single transfer: EOM set
   * TermChar: not set
   */

  f->reserve(1024);
  f->clear();
  f->clearSub();
  // Initial values
  QVERIFY(f->isEmpty());
  QVERIFY(!f->isFull());
  QVERIFY(f->remainCapacity() == 1024);
  QVERIFY(f->bytesToStore() == 1024);
  QVERIFY(!f->isComplete());
  QVERIFY(uf->messageData() == "");
  // Build the frame
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0xF8);                           // bTag
  srcData.append((char)0x07);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)5);                              // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append('A');                                  // Message data
  srcData.append('B');                                  // Message data
  srcData.append('C');                                  // Message data
  srcData.append('D');                                  // Message data
  srcData.append('E');                                  // Message data
  srcData.append((char)0);                              // Alignment byte
  srcData.append((char)0);                              // Alignment bytes
  srcData.append((char)0);                              // Alignment byte
  QVERIFY(srcData.size() == 20);

  /* Put byte per byte */

  // MsgID
  c = srcData.at(0);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 1);
  QVERIFY(f->bytesToStore() == 1023);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bTag
  c = srcData.at(1);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 2);
  QVERIFY(f->bytesToStore() == 1022);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bTagInverse
  c = srcData.at(2);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 3);
  QVERIFY(f->bytesToStore() == 1021);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(3);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 4);
  QVERIFY(f->bytesToStore() == 1020);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LLSB
  c = srcData.at(4);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 5);
  QVERIFY(f->bytesToStore() == 1019);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, LSB
  c = srcData.at(5);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 6);
  QVERIFY(f->bytesToStore() == 1018);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MSB
  c = srcData.at(6);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 7);
  QVERIFY(f->bytesToStore() == 1017);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Transfer size, MMSB
  c = srcData.at(7);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 8);
  QVERIFY(f->bytesToStore() == 1016);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // bmTransferAttributes
  c = srcData.at(8);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 9);
  QVERIFY(f->bytesToStore() == 1015);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(9);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 10);
  QVERIFY(f->bytesToStore() == 1014);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(10);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 11);
  QVERIFY(f->bytesToStore() == 1013);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Reserved
  c = srcData.at(11);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 12);
  QVERIFY(f->bytesToStore() == 1012);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(12);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 13);
  QVERIFY(f->bytesToStore() == 1011);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(13);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 14);
  QVERIFY(f->bytesToStore() == 1010);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(14);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 15);
  QVERIFY(f->bytesToStore() == 1009);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());
  // Data
  c = srcData.at(15);
  QVERIFY(f->putData(&c, 1) == 1);
  QVERIFY(f->size() == 16);
  QVERIFY(f->bytesToStore() == 1008);
  QVERIFY(!f->isEmpty());
  QVERIFY(!f->isComplete());

  /*
   * Now, store last data byte with alignement bytes.
   * Storing a alignement byte alone is not a realistic case
   */
  subData = srcData.right(4);
  QVERIFY(f->putData(subData.data(), 4) == 4);
  QVERIFY(f->size() == 20);
  QVERIFY(f->bytesToStore() == 0);
  QVERIFY(!f->isEmpty());
  // Now we should have a complete frame
  QVERIFY(f->isComplete());
  QVERIFY(uf->messageData() == "ABCDE");
  QVERIFY(uf->MsgID() == 2);            // MsgID: DEV_DEP_MSG_IN
  QVERIFY(uf->isEOM());
  QVERIFY(uf->bTag() == 0xF8);

  delete uf;
}

void mdtFrameTest::usbTmcDecodeRealCasesTest()
{
  mdtFrameUsbTmc f;
  QByteArray srcData;
  QByteArray tr1, tr2;

  /*
   * Device: Agilent U3606A
   * Quesry: *IDN?
   * Test: answer decode
   */

  // Build the answer like it is returned by device
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0x05);                           // bTag
  srcData.append((char)0xFA);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)57);                             // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set NOTE: verify
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append("Agilent Technologies,U3606A,MYxxxxxxxx,02.00-02.00-02.01\n");
  srcData.append((char)0);                              // Alignment byte
  srcData.append((char)0);                              // Alignment bytes
  srcData.append((char)0);                              // Alignment byte
  // We have a frame size: 12 + 57 + 3 = 72
  QVERIFY(srcData.size() == 72);

  // Read happens in 2 transfers: 1x 64 Bytes + 1x 17 Bytes
  // The second read of 17 Bytes is wrong, should be 8 (64+8=72)
  tr1 = srcData.left(64);
  QVERIFY(tr1.size() == 64);
  tr2 = srcData.right(8);
  QVERIFY(tr2.size() == 8);
  // Add 9 unknown data
  tr2.append("?????????");
  QVERIFY(tr2.size() == 17);

  // Init frame
  f.reserve(512);
  f.clear();
  f.clearSub();
  // Put first transfer data
  QVERIFY(f.putData(tr1.data(), tr1.size()) == tr1.size());
  QVERIFY(f.size() == tr1.size());
  QVERIFY(f.bytesToStore() > 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(!f.isComplete());
  // Put second transfer data, frame must become complete
  QVERIFY(f.putData(tr2.data(), tr2.size()) == tr2.size());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isComplete());
  QVERIFY(f.messageData() == "Agilent Technologies,U3606A,MYxxxxxxxx,02.00-02.00-02.01\n");

  /*
   * Device: Agilent DSO1012A
   * Quesry: *IDN?
   * Test: answer decode.
   *  The does not add alignement bytes (that's OK regarding USBTMC specs).
   */

  // Build the answer like it is returned by device
  srcData.clear();
  srcData.append((char)mdtFrameUsbTmc::DEV_DEP_MSG_IN); // MsgID
  srcData.append((char)0x02);                           // bTag
  srcData.append((char)0xFD);                           // bTagInverse
  srcData.append((char)0);                              // Reserved
  srcData.append((char)50);                             // TransferSize, LLSB
  srcData.append((char)0);                              // TransferSize, LSB
  srcData.append((char)0);                              // TransferSize, MSB
  srcData.append((char)0);                              // TransferSize, MMSB
  srcData.append((char)0x01);                           // bmTransferAttributes: EOM set
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append((char)0);                              // Reserved
  srcData.append("Agilent Technologies,DSO1012A,CNxxxxxxxx,00.04.02\n");
  // We have a frame size: 12 + 50 + 0 = 62
  QVERIFY(srcData.size() == 62);
  // Read happens in 1 transfer of 62 bytes
  tr1 = srcData;
  QVERIFY(tr1.size() == 62);
  // Init frame
  f.reserve(512);
  f.clear();
  f.clearSub();
  // Put data into frame and check
  QVERIFY(f.putData(tr1.data(), tr1.size()) == tr1.size());
  QVERIFY(f.size() == tr1.size());
  QVERIFY(f.bytesToStore() == 0);
  QVERIFY(!f.isEmpty());
  QVERIFY(f.isComplete());
  QVERIFY(f.messageData() == "Agilent Technologies,DSO1012A,CNxxxxxxxx,00.04.02\n");

}

void mdtFrameTest::usbControlEncodeTest()
{
  mdtFrameUsbControl f;

  // Initial states
  QVERIFY(!f.directionIsDeviceToHost());
  QVERIFY(f.requestType() == mdtFrameUsbControl::RT_STANDARD);
  QVERIFY(f.requestRecipient() == mdtFrameUsbControl::RR_DEVICE);
  QVERIFY(f.bRequest() == 0);
  QVERIFY(f.wValue() == 0);
  QVERIFY(f.wIndex() == 0);
  QVERIFY(f.wLength() == 0);

  // Set flags
  f.setDirectionDeviceToHost(false);
  f.setRequestType(mdtFrameUsbControl::RT_CLASS);
  f.setRequestRecipient(mdtFrameUsbControl::RR_OTHER);
  f.setbRequest(7);
  f.setwValue(5);
  f.setwIndex(6);
  f.setwLength(12);
  // Check flags
  QVERIFY(!f.directionIsDeviceToHost());
  QVERIFY(f.requestType() == mdtFrameUsbControl::RT_CLASS);
  QVERIFY(f.requestRecipient() == mdtFrameUsbControl::RR_OTHER);
  QVERIFY(f.bRequest() == 7);
  QVERIFY(f.wValue() == 5);
  QVERIFY(f.wIndex() == 6);
  QVERIFY(f.wLength() == 12);

  /*
   * Buid a USBTMC GET_CAPABILITIES request
   */
  f.clearSub();
  f.setDirectionDeviceToHost(true);
  f.setRequestType(mdtFrameUsbControl::RT_CLASS);
  f.setRequestRecipient(mdtFrameUsbControl::RR_INTERFACE);
  f.setbRequest(7); // GET_CAPABILITIES
  f.setwValue(0);
  f.setwIndex(0);   // Choose interface 0
  f.setwLength(0x18);
  f.encode();
  // Check frame
  QVERIFY(f.size() == 8);
  QVERIFY((quint8)f.at(0) == 0xA1);
  QVERIFY((quint8)f.at(1) == 7);    // bRequest
  QVERIFY((quint8)f.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f.at(7) == 0x00); // wLength, MSB

  /*
   * Check copy
   */
  mdtFrameUsbControl f2(f);
  // Check copy flags
  QVERIFY(f2.directionIsDeviceToHost());
  QVERIFY(f2.requestType() == mdtFrameUsbControl::RT_CLASS);
  QVERIFY(f2.requestRecipient() == mdtFrameUsbControl::RR_INTERFACE);
  QVERIFY(f2.bRequest() == 7);
  QVERIFY(f2.wValue() == 0);
  QVERIFY(f2.wIndex() == 0);
  QVERIFY(f2.wLength() == 0x18);
  // Check copy
  QVERIFY(f2.size() == 8);
  QVERIFY((quint8)f2.at(0) == 0xA1);
  QVERIFY((quint8)f2.at(1) == 7);    // bRequest
  QVERIFY((quint8)f2.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f2.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f2.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f2.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f2.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f2.at(7) == 0x00); // wLength, MSB
  // Clear f and check
  f.clear();
  f.clearSub();
  QVERIFY(f2.directionIsDeviceToHost());
  QVERIFY(f2.requestType() == mdtFrameUsbControl::RT_CLASS);
  QVERIFY(f2.requestRecipient() == mdtFrameUsbControl::RR_INTERFACE);
  QVERIFY(f2.bRequest() == 7);
  QVERIFY(f2.wValue() == 0);
  QVERIFY(f2.wIndex() == 0);
  QVERIFY(f2.wLength() == 0x18);
  QVERIFY(f2.size() == 8);
  QVERIFY((quint8)f2.at(0) == 0xA1);
  QVERIFY((quint8)f2.at(1) == 7);    // bRequest
  QVERIFY((quint8)f2.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f2.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f2.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f2.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f2.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f2.at(7) == 0x00); // wLength, MSB
  QVERIFY(!f.directionIsDeviceToHost());
  QVERIFY(f.requestType() == mdtFrameUsbControl::RT_STANDARD);
  QVERIFY(f.requestRecipient() == mdtFrameUsbControl::RR_DEVICE);
  QVERIFY(f.bRequest() == 0);
  QVERIFY(f.wValue() == 0);
  QVERIFY(f.wIndex() == 0);
  QVERIFY(f.wLength() == 0);
  QVERIFY(f.size() == 0);
  /*
   * Copy with = operator
   */
  f2.clear();
  f2.clearSub();
  QVERIFY(!f2.directionIsDeviceToHost());
  QVERIFY(f2.requestType() == mdtFrameUsbControl::RT_STANDARD);
  QVERIFY(f2.requestRecipient() == mdtFrameUsbControl::RR_DEVICE);
  QVERIFY(f2.bRequest() == 0);
  QVERIFY(f2.wValue() == 0);
  QVERIFY(f2.wIndex() == 0);
  QVERIFY(f2.wLength() == 0);
  /*
   * Re-build a USBTMC GET_CAPABILITIES request
   */
  f.clearSub();
  f.setDirectionDeviceToHost(true);
  f.setRequestType(mdtFrameUsbControl::RT_CLASS);
  f.setRequestRecipient(mdtFrameUsbControl::RR_INTERFACE);
  f.setbRequest(7); // GET_CAPABILITIES
  f.setwValue(0);
  f.setwIndex(0);   // Choose interface 0
  f.setwLength(0x18);
  f.encode();
  // Check frame
  QVERIFY(f.size() == 8);
  QVERIFY((quint8)f.at(0) == 0xA1);
  QVERIFY((quint8)f.at(1) == 7);    // bRequest
  QVERIFY((quint8)f.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f.at(7) == 0x00); // wLength, MSB
  // Copy
  f2 = f;
  // Check copy flags
  QVERIFY(f2.directionIsDeviceToHost());
  QVERIFY(f2.requestType() == mdtFrameUsbControl::RT_CLASS);
  QVERIFY(f2.requestRecipient() == mdtFrameUsbControl::RR_INTERFACE);
  QVERIFY(f2.bRequest() == 7);
  QVERIFY(f2.wValue() == 0);
  QVERIFY(f2.wIndex() == 0);
  QVERIFY(f2.wLength() == 0x18);
  // Check copy
  QVERIFY(f2.size() == 8);
  QVERIFY((quint8)f2.at(0) == 0xA1);
  QVERIFY((quint8)f2.at(1) == 7);    // bRequest
  QVERIFY((quint8)f2.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f2.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f2.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f2.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f2.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f2.at(7) == 0x00); // wLength, MSB
  // Clear f and check
  f.clear();
  f.clearSub();
  QVERIFY(f2.directionIsDeviceToHost());
  QVERIFY(f2.requestType() == mdtFrameUsbControl::RT_CLASS);
  QVERIFY(f2.requestRecipient() == mdtFrameUsbControl::RR_INTERFACE);
  QVERIFY(f2.bRequest() == 7);
  QVERIFY(f2.wValue() == 0);
  QVERIFY(f2.wIndex() == 0);
  QVERIFY(f2.wLength() == 0x18);
  QVERIFY(f2.size() == 8);
  QVERIFY((quint8)f2.at(0) == 0xA1);
  QVERIFY((quint8)f2.at(1) == 7);    // bRequest
  QVERIFY((quint8)f2.at(2) == 0);    // wValue, LSB
  QVERIFY((quint8)f2.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f2.at(4) == 0);    // wIndex, LSB
  QVERIFY((quint8)f2.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f2.at(6) == 0x18); // wLength, LSB
  QVERIFY((quint8)f2.at(7) == 0x00); // wLength, MSB
  QVERIFY(!f.directionIsDeviceToHost());
  QVERIFY(f.requestType() == mdtFrameUsbControl::RT_STANDARD);
  QVERIFY(f.requestRecipient() == mdtFrameUsbControl::RR_DEVICE);
  QVERIFY(f.bRequest() == 0);
  QVERIFY(f.wValue() == 0);
  QVERIFY(f.wIndex() == 0);
  QVERIFY(f.wLength() == 0);
  QVERIFY(f.size() == 0);

  /*
   * Build a USBTMC INITIATE_ABORT_BULK_IN request
   */
  f.clearSub();
  f.setDirectionDeviceToHost(true);
  f.setRequestType(mdtFrameUsbControl::RT_CLASS);
  f.setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  f.setbRequest(3); // INITIATE_ABORT_BULK_IN
  f.setwValue(213); // D7..D0: bTag
  f.setwIndex(0x82);   // Endpoint IN 2
  f.setwLength(0x02);
  f.encode();
  // Check frame
  QVERIFY(f.size() == 8);
  QVERIFY((quint8)f.at(0) == 0xA2);
  QVERIFY((quint8)f.at(1) == 3);    // bRequest
  QVERIFY((quint8)f.at(2) == 213);  // wValue, LSB
  QVERIFY((quint8)f.at(3) == 0);    // wValue, MSB
  QVERIFY((quint8)f.at(4) == 0x82); // wIndex, LSB
  QVERIFY((quint8)f.at(5) == 0);    // wIndex, MSB
  QVERIFY((quint8)f.at(6) == 0x02); // wLength, LSB
  QVERIFY((quint8)f.at(7) == 0x00); // wLength, MSB

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
