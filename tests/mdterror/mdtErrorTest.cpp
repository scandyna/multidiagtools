/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtErrorTest.h"
#include "mdtApplication.h"
#include "mdtErrorV2.h"

#include <QDebug>

void mdtErrorTest::constructAndCopyTest()
{
  /*
   * Default constructed
   */
  mdtErrorV2 error1;
  QVERIFY(error1.isNull());
  QVERIFY(error1.level() == mdtErrorV2::NoError);
  QVERIFY(error1.text().isEmpty());
  QVERIFY(error1.informativeText().isEmpty());
  // Calling error() is allowed on null mdtError
  QCOMPARE(error1.error<int>(), 0);
  /*
   * Copy construct
   */
  // Set some attributes to error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  error1.setInformativeText("error1 info");
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  // Construct error2 on base of error1
  mdtErrorV2 error2(error1);
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QCOMPARE(error2.informativeText(), QString("error1 info"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  /*
   * COW test
   */
  // Set error2
  error2.setError<int>(2, "error2", mdtErrorV2::Info);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == mdtErrorV2::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  // Update error2 informative text
  error2.setInformativeText("error2 info");
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QCOMPARE(error2.informativeText(), QString("error2 info"));
  QVERIFY(error2.level() == mdtErrorV2::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  // Set error2 to a other error type
  error2.setError<double>(2.1, "error2.1", mdtErrorV2::Warning);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<double>(), 2.1);
  QCOMPARE(error2.text(), QString("error2.1"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == mdtErrorV2::Warning);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  /*
   * Check many copies of error
   */
  // Set error1
  error1.setError<int>(5, "shared error 5", mdtErrorV2::Info);
  // Copy construct 2 errors
  mdtErrorV2 error3(error1);
  mdtErrorV2 error4(error1);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<int>(), 5);
  QCOMPARE(error3.text(), QString("shared error 5"));
  QVERIFY(error3.level() == mdtErrorV2::Info);
  // Check error4
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  // Set error3
  error3.setError<double>(3.0, "error3", mdtErrorV2::Warning);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == mdtErrorV2::Warning);
  // Check that error1 is untouched
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 5);
  QCOMPARE(error1.text(), QString("shared error 5"));
  QVERIFY(error1.level() == mdtErrorV2::Info);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  // Clear error1
  error1.clear();
  // Check error1
  QVERIFY(error1.isNull());
  // Check that error3 is untouched
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == mdtErrorV2::Warning);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  /*
   * Copy assignment
   */
  // Set error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  // Assign error1 to error2
  error2 = error1;
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  // Set error1
  error1.setError<double>(1.1, "error1.1", mdtErrorV2::Warning);
  // Check error1
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<double>(), 1.1);
  QCOMPARE(error1.text(), QString("error1.1"));
  QVERIFY(error1.level() == mdtErrorV2::Warning);
  // Check that error2 is untouched
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  /*
   * Update tests
   * (setError() allways resets, check other setters)
   */
  // To check that COW works, we set error2 to other type than error1
  error2.setError<double>(2.5, "error2.5", mdtErrorV2::Warning);
  // Set error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  // Assign error1 to error2
  qDebug() << "TEST: error2 = error1";
  error2 = error1;
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check informative text
  QVERIFY(error1.informativeText().isEmpty());
  QVERIFY(error2.informativeText().isEmpty());
  // Set informative text to error1
  qDebug() << "TEST: error1.setInformativeText()";
  error1.setInformativeText("error1 info");
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check error1
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  // Check that error2 is untouched
  QVERIFY(error2.informativeText().isEmpty());
  // Set informative text to error2
  qDebug() << "TEST: error2.setInformativeText()";
  error2.setInformativeText("error2 info");
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check error2
  QCOMPARE(error2.informativeText(), QString("error2 info"));
  // Check that error1 is untouched
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  /*
   * Move
   */
  /// \todo Implement once understanding move..
  qDebug() << "Move ...";
  error2 = std::move(error1);
  qDebug() << "Done";
  qDebug() << "swap ...";
  std::swap(error1, error2);
  qDebug() << "Done";
}

void mdtErrorTest::errorStackTest()
{
  std::vector<mdtErrorV2> errorStack;
  /*
   * Some check errors with empty stack, etc...
   */
  mdtErrorV2 error1;
  mdtErrorV2 error2;
  mdtErrorV2 error3;
  mdtErrorV2 error4;
  // Try to get stack from null error
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());
  errorStack = error1.getErrorStack();
  QVERIFY(errorStack.empty());
  errorStack = error2.getErrorStack();
  QVERIFY(errorStack.empty());
  // Setup errors
  error1.setError("Error 1", mdtErrorV2::Error);
  error2.setError("Error 2", mdtErrorV2::Info);
  // Try to get error stack from error that does not have stack
  errorStack = error1.getErrorStack();
  QVERIFY(errorStack.empty());
  errorStack = error2.getErrorStack();
  QVERIFY(errorStack.empty());
  /*
   * Stack error1 -> error2 :
   * error2
   *   |-[error1]
   */
  error2.stackError(error1);
  errorStack = error2.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  /*
   * Immagine som senario:
   * User wants to open a file, using a software with some GUI.
   * The software calls some helper function,
   * witch also call a system function.
   * The system function fails (because of missing write acces, f.ex.)
   * and also returns a error with a system specific (and technical) text.
   * Then, the software helper function automatically fails,
   * and simply gets error returned by the system function.
   * Finally, the software generates a error with a more user friendly message,
   * but also some back trace of what technically failed
   */
  mdtErrorV2 systemFunctionError;
  mdtErrorV2 systemFunctionApiError;
  mdtErrorV2 helperFunctionError;
  mdtErrorV2 guiError;

  // First, system API function fails
  systemFunctionApiError.setError<int>(-1, "write access error", mdtErrorV2::Error);
  // Then, system function generates a error with some details and stacks API error
  systemFunctionError.setError<int>(-1, "Could not open file 'fake.txt'", mdtErrorV2::Error);
  systemFunctionError.stackError(systemFunctionApiError);
  errorStack = systemFunctionError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("write access error"));
  // Helper function also fails, but does not provide more informations
  helperFunctionError = systemFunctionError;
  errorStack = helperFunctionError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("write access error"));
  // Finally, the GUI function makes some checks, and generates a error with user friendly message
  auto errorCode = helperFunctionError.error<int>();
  switch(errorCode){
    case -1:
      guiError.setError("Could not open document 'fake.txt' because of missing write access", mdtErrorV2::Error);
      break;
    case -2:
      guiError.setError("Could not open document 'fake.txt' because it allready exists", mdtErrorV2::Error);
      break;
  }
  guiError.stackError(helperFunctionError);
  // The guiError is also displayed (f.ex. with a mdtErrorDialog)
  QCOMPARE(guiError.text(), QString("Could not open document 'fake.txt' because of missing write access"));
  errorStack = guiError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)2);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("Could not open file 'fake.txt'"));
  QCOMPARE(errorStack.at(1).error<int>(), -1);
  QCOMPARE(errorStack.at(1).text(), QString("write access error"));
  /*
   * Check Copy-On-Write
   */
  // error3 and error4 will be stacked to error1 and error2, set them up
  error3.setError<long int>(30, "error3", mdtErrorV2::Warning);
  error4.setError<float>(4.0, "error4", mdtErrorV2::Info);
  // Set error1 and error2 to some error
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  // Assign error1 -> error1
  error2 = error1;
  QVERIFY(error1.getErrorStack().empty());
  QVERIFY(error2.getErrorStack().empty());
  // Stack to error1
  qDebug() << "TEST: stack to error1";
  error1.stackError(error3);
  // Check error1 stack
  errorStack = error1.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<long int>(), (long int)30);
  QCOMPARE(errorStack.at(0).text(), QString("error3"));
  // Check that error2 is untouched
  QVERIFY(error2.getErrorStack().empty());
  // Stack to error2
  qDebug() << "TEST: stack to error2";
  error2.stackError(error4);
  // Check error2
  errorStack = error2.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<float>(), 4.0);
  QCOMPARE(errorStack.at(0).text(), QString("error4"));
  // Check that error1 is untouched
  errorStack = error1.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<long int>(), (long int)30);
  QCOMPARE(errorStack.at(0).text(), QString("error3"));

}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtErrorTest errorTest;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&errorTest, argc, argv);
}
