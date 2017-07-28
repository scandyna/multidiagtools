/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_CORE_TEST_BASE_H
#define MDT_PLAIN_TEXT_CORE_TEST_BASE_H

#include "Mdt/Application.h"
#include "Mdt/PlainText/RecordTemplate.h"
#include "Mdt/PlainText/RecordListTemplate.h"
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QTemporaryFile>
#include <QtTest/QtTest>
#include <Qt>

#include <QDebug>

class QAbstractItemModel;

class TestBase : public QObject
{
 Q_OBJECT

 public:

  /*! \internal Call of Qt compare helper
   *
   * QTest::compare_helper() is part of the private API of Qt.
   * Hope that it will not change in the future ?
   */
  static bool compare_helper_nok(const QString & failureMsg,
                                 char *val1, char *val2,
                                 const char *actual, const char *expected,
                                 const char *file, int line)
  {
    return QTest::compare_helper(false, failureMsg.toLocal8Bit().constData(), val1, val2, actual, expected, file, line);
  }

  static bool compare_helper_ok(char *val1, char *val2,
                                const char *actual, const char *expected,
                                const char *file, int line)
  {
    return QTest::compare_helper(true, nullptr, val1, val2, actual, expected, file, line);
  }

 protected:

  static QVariant getModelData(const QAbstractItemModel *model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);
  static QVariant getModelData(const QAbstractItemModel & model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);

  static bool writeTemporaryTextFile(QTemporaryFile & file, const QString & data, const QByteArray & encoding = QByteArray("UTF-8"));


};

namespace QTest{

  template <typename T>
  inline bool qCompare(const Mdt::PlainText::RecordTemplate<T> & r1, const Mdt::PlainText::RecordTemplate<T> & r2,
                       const char *actual, const char *expected, const char *file, int line)
  {
    const int actualSize = r1.columnCount();
    const int expectedSize = r2.columnCount();
    if(actualSize != expectedSize){
      const QString msg = QString("Compared records have different column counts.\n"
                                  "   Actual   (%1) columns: %2\n"
                                  "   Expected (%3) columns: %4")
                                  .arg(QString(actual)).arg(actualSize)
                                  .arg(QString(expected)).arg(expectedSize);
      return ::TestBase::compare_helper_nok(msg, nullptr, nullptr, actual, expected, file, line);
    }
    const int n = actualSize;
    for(int col = 0; col < n; ++col){
      if(r1.data(col) != r2.data(col)){
        char *val1 = toString(r1.data(col));
        char *val2 = toString(r2.data(col));
        const QString msg = QString("Compared records differs at index %1.\n"
                                    "   Actual   (%2): %3\n"
                                    "   Expected (%4): %5")
                                    .arg(col)
                                    .arg(QString(actual)).arg( QString(val1) )
                                    .arg(QString(expected)).arg( QString(val2) );
        delete[] val1;
        delete[] val2;
        return ::TestBase::compare_helper_nok(msg, nullptr, nullptr, actual, expected, file, line);
      }
    }

    return ::TestBase::compare_helper_ok(nullptr, nullptr, actual, expected, file, line);
  }

  template <typename RecordType, typename T>
  inline bool qCompare(const Mdt::PlainText::RecordListTemplate<RecordType, T> & l1, const Mdt::PlainText::RecordListTemplate<RecordType, T> & l2,
                       const char *actual, const char *expected, const char *file, int line)
  {
    const int actualRowCount = l1.rowCount();
    const int expectedRowCount = l2.rowCount();
    if(actualRowCount != expectedRowCount){
      const QString msg = QString("Compared record lists have different row counts.\n"
                                  "   Actual   (%1) rows: %2\n"
                                  "   Expected (%3) rows: %4")
                                  .arg(QString(actual)).arg(actualRowCount)
                                  .arg(QString(expected)).arg(expectedRowCount);
      return ::TestBase::compare_helper_nok(msg, nullptr, nullptr, actual, expected, file, line);
    }
    const int rowCount = actualRowCount;
    for(int row = 0; row < rowCount; ++row){
      const int actualColumnCount = l1.columnCount(row);
      const int expectedColumnCount = l2.columnCount(row);
      if(actualColumnCount != expectedColumnCount){
        const QString msg = QString("Compared record lists have different column counts at row %1.\n"
                                    "   Actual   (%2) columns: %3\n"
                                    "   Expected (%4) columns: %5")
                                    .arg(row)
                                    .arg(QString(actual)).arg(actualColumnCount)
                                    .arg(QString(expected)).arg(expectedColumnCount);
        return ::TestBase::compare_helper_nok(msg, nullptr, nullptr, actual, expected, file, line);
      }
      const int columnCount = actualColumnCount;
      for(int col = 0; col < columnCount; ++col){
        if(l1.data(row, col) != l2.data(row, col)){
          char *val1 = toString(l1.data(row, col));
          char *val2 = toString(l2.data(row, col));
          const QString msg = QString("Compared record lists differs at row %1, column %2.\n"
                                      "   Actual   (%3): %4\n"
                                      "   Expected (%5): %6")
                                      .arg(row).arg(col)
                                      .arg(QString(actual)).arg( QString(val1) )
                                      .arg(QString(expected)).arg( QString(val2) );
          delete[] val1;
          delete[] val2;
          return ::TestBase::compare_helper_nok(msg, nullptr, nullptr, actual, expected, file, line);
        }
      }
    }

    return ::TestBase::compare_helper_ok(nullptr, nullptr, actual, expected, file, line);
  }
} // namespace QTest{

#endif // #ifndef MDT_PLAIN_TEXT_CORE_TEST_BASE_H
