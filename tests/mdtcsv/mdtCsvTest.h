/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CSV_TEST_H
#define MDT_CSV_TEST_H

#include "mdtTest.h"

class mdtCsvTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void fileSettingsWidgetTest();
  void parserSettingsTest();
  void parserSettingsWidgetTest();
  void fileParserSettingsDialogTest();
  void generatorSettingsTest();
  void generatorSettingsWidgetTest();
  void fileGeneratorSettingsDialogTest();
  void recordTest();
  void dataTest();
  void recordFormatTest();
  void csvStringInfoTest();
  void csvFileInfoTest();

  void csvStringParserIteratorTest();
  void csvStringParserIteratorBenchmark();

  void stringParserReadLineTest();
  void stringParserReadLineTest_data();
  void stringParserReadAllTest();
  void stringParserReadAllTest_data();

  void csvFileParserIteratorSharedDataTest();
  void csvFileParserIteratorSharedDataReadTest();
  void csvFileParserIteratorSharedDataReadTest_data();
  void csvFileParserIteratorTest();
  void csvFileParserMultiPassIteratorTest();
  void csvFileParserTest();
  void csvFileParserReadLineTest();
  void csvFileParserReadLineTest_data();
  void csvFileParserReadAllTest();
  void csvFileParserReadAllTest_data();

  void csvStringGeneratorFromRecordTest();
  void csvStringGeneratorFromRecordTest_data();
  void csvStringGeneratorFromDataTest();
  void csvStringGeneratorFromDataTest_data();
  void csvFileGeneratorWriteLineTest();
  void csvFileGeneratorWriteLineTest_data();
  void csvFileGeneratorWriteAllTest();
  void csvFileGeneratorWriteAllTest_data();

  void csvTableViewDataMapperTest();
  void csvTableViewExportThreadTest();
  void csvTableViewExportDialogTest();

 private:

  void buildCsvParserTestData();
  void buildCsvGeneratorTestData();
};

#endif // #ifndef MDT_CSV_TEST_H
