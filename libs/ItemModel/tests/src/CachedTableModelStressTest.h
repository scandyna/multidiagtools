/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "TestBase.h"
#include "Mdt/Container/VariantRecord.h"
#include <QThread>

#include <QDebug>

class AsyncPersonRecordGenerator : public QThread
{
  Q_OBJECT

 public:

  AsyncPersonRecordGenerator(int rowCountToGenarete)
    : mRowCountToGenerate(rowCountToGenarete)
  {
    Q_ASSERT(mRowCountToGenerate > 0);
  }

 signals:

  void newRecordAvailable(const Mdt::Container::VariantRecord & record);

 private:

  void run() override
  {
    for(int row = 0; row < mRowCountToGenerate; ++row){
//       qDebug() << "Gen record " << row;
      emit newRecordAvailable( makeRecord(row) );
    }
  }

  static Mdt::Container::VariantRecord makeRecord(int row)
  {
    const QString name = QLatin1String("Name ") + QString::number(row);

    return Mdt::Container::VariantRecord{row, name};
  }

  int mRowCountToGenerate;
};

class CachedTableModelStressTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();

  void readOnlyFetchAllAsyncTest();
  void readOnlyFetchAllAsyncTest_data();

  void editableFetchAllAsyncTest();
  void editableFetchAllAsyncTest_data();

  void readOnlyFetchAllAsyncTableViewTest();
  void readOnlyFetchAllAsyncTableViewTest_data();

  void editableFetchAllAsyncTableViewTest();
  void editableFetchAllAsyncTableViewTest_data();

};
