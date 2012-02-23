#include "mdtFrameTest.h"
#include "mdtFrame.h"

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
  char data[11] = "0123456789";

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
