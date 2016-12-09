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
#ifndef MDT_MODEL_EDITOR_FILTER_PROXY_MODEL_TEST_H
#define MDT_MODEL_EDITOR_FILTER_PROXY_MODEL_TEST_H

#include "Mdt/ItemModel/VariantTableModel.h"
#include <QObject>
#include <QtTest/QtTest>

class FilterProxyModelTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void filterTest();
  void filterBenchmark();
  void filterBenchmark_data();
  void filterLikeBenchmark();
  void filterLikeBenchmark_data();

 private:

  void populateModel(Mdt::ItemModel::VariantTableModel & model);
};

#endif // #ifndef MDT_MODEL_EDITOR_FILTER_PROXY_MODEL_TEST_H
