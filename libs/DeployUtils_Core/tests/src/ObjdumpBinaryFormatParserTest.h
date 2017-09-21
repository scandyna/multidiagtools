/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef OBJDUMP_BINARY_FORMAT_PARSER_TEST_H
#define OBJDUMP_BINARY_FORMAT_PARSER_TEST_H

#include "TestBase.h"
#include <QString>
#include <QStringList>

class ObjdumpBinaryFormatParserTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void fileFormatGrammarTest();
  void fileFormatGrammarTest_data();

  void architectureGrammarTest();
  void architectureGrammarTest_data();

  void parserTest();
  void parserTest_data();

//  private:
// 
//   static QString objdumpHeaderWindows();
//   static QString objdumpEntryWindows(const QString & libraryName);
//   static QString objdumpEntryListWindows(const QStringList & libraryNameList);
//   static QString objdumpOutputWindows(const QStringList & libraryNameList);
};

#endif // #ifndef OBJDUMP_BINARY_FORMAT_PARSER_TEST_H
