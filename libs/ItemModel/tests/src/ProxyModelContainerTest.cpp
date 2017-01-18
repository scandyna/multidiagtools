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
#include "ProxyModelContainerTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/ItemModel/FilterProxyModel.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/SortProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSortFilterProxyModel>

#include <QDebug>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
using ItemModel::ProxyModelContainer;
using ItemModel::FilterProxyModel;
using ItemModel::RelationFilterProxyModel;
using ItemModel::SortProxyModel;

void ProxyModelContainerTest::initTestCase()
{
}

void ProxyModelContainerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ProxyModelContainerTest::initialStateTest()
{
  ProxyModelContainer container;

  QVERIFY(container.sourceModel() == nullptr);
  QCOMPARE(container.proxyModelCount(), 0);
}

void ProxyModelContainerTest::appendTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  FilterProxyModel filterModel;
  SortProxyModel sortModel;
  RelationFilterProxyModel relationModel;

  /*
   * Set source model
   */
  container.setSourceModel(&model);
  QVERIFY(container.sourceModel() == &model);
  /*
   * Append a filter model
   */
  container.appendProxyModel(&filterModel);
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &model);
  /*
   * Append sort model
   */
  container.appendProxyModel(&sortModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &filterModel);
  /*
   * Append relation model
   */
  container.appendProxyModel(&relationModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QCOMPARE(container.proxyModelCount(), 3);
  QVERIFY(container.proxyModelAt(1) == &sortModel);
  QVERIFY(container.proxyModelAt(1)->sourceModel() == &filterModel);
  QVERIFY(container.lastProxyModel() == &relationModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &sortModel);
}

void ProxyModelContainerTest::appendThenSetSourceModelTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  FilterProxyModel filterModel;
  SortProxyModel sortModel;

  /*
   * Append a filter model
   */
  container.appendProxyModel(&filterModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == nullptr);
  QVERIFY(container.lastProxyModel() == &filterModel);
  /*
   * Append sort model
   */
  container.appendProxyModel(&sortModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == nullptr);
  QVERIFY(container.lastProxyModel() == &sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &filterModel);
  /*
   * Set source model
   */
  container.setSourceModel(&model);
  QVERIFY(container.sourceModel() == &model);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &filterModel);
}

void ProxyModelContainerTest::prependTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  FilterProxyModel filterModel;
  SortProxyModel sortModel;
  RelationFilterProxyModel relationModel;

  /*
   * Set source model
   */
  container.setSourceModel(&model);
  QVERIFY(container.sourceModel() == &model);
  /*
   * Prepend a filter model
   */
  container.prependProxyModel(&filterModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &model);
  /*
   * Prepend sort model
   */
  container.prependProxyModel(&sortModel);
  QVERIFY(container.firstProxyModel() == &sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &sortModel);
  /*
   * Prepend relation model
   */
  container.prependProxyModel(&relationModel);
  QVERIFY(container.firstProxyModel() == &relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QCOMPARE(container.proxyModelCount(), 3);
  QVERIFY(container.proxyModelAt(1) == &sortModel);
  QVERIFY(container.proxyModelAt(1)->sourceModel() == &relationModel);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &sortModel);
}

void ProxyModelContainerTest::prependThenSetSourceModelTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  FilterProxyModel filterModel;
  SortProxyModel sortModel;

  /*
   * Prepend a filter model
   */
  container.prependProxyModel(&filterModel);
  QVERIFY(container.firstProxyModel() == &filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == nullptr);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == nullptr);
  /*
   * Prepend sort model
   */
  container.prependProxyModel(&sortModel);
  QVERIFY(container.firstProxyModel() == &sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == nullptr);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &sortModel);
  /*
   * Set source model
   */
  container.setSourceModel(&model);
  QVERIFY(container.sourceModel() == &model);
  QVERIFY(container.firstProxyModel() == &sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == &filterModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &sortModel);
}

void ProxyModelContainerTest::searchTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  RelationFilterProxyModel relationModel;
  FilterProxyModel filterModel;
  SortProxyModel sortModel;
  QSortFilterProxyModel notInContainerModel;

  /*
   * Setup container
   */
  container.setSourceModel(&model);
  container.appendProxyModel(&relationModel);
  container.appendProxyModel(&filterModel);
  container.appendProxyModel(&sortModel);
  /*
   * Check getting index of proxy model
   */
  QCOMPARE(container.indexOfProxyModel(&relationModel), 0);
  QCOMPARE(container.indexOfProxyModel(&filterModel), 1);
  QCOMPARE(container.indexOfProxyModel(&sortModel), 2);
  QCOMPARE(container.indexOfProxyModel(&notInContainerModel), -1);
  /*
   * Check getting index of proxy model of type
   */
  QCOMPARE(container.indexOfFirstProxyModelOfType<RelationFilterProxyModel>(), 0);
  QCOMPARE(container.indexOfFirstProxyModelOfType<FilterProxyModel>(), 1);
  QCOMPARE(container.indexOfFirstProxyModelOfType<SortProxyModel>(), 2);
  QCOMPARE(container.indexOfFirstProxyModelOfType<QSortFilterProxyModel>(), -1);
  QCOMPARE(container.indexOfFirstProxyModelOfType<QAbstractProxyModel>(), -1);
  /*
   * Check contains proxy model of type
   */
  QVERIFY(container.containsProxyModelOfType<RelationFilterProxyModel>());
  QVERIFY(container.containsProxyModelOfType<FilterProxyModel>());
  QVERIFY(container.containsProxyModelOfType<SortProxyModel>());
  QVERIFY(!container.containsProxyModelOfType<QSortFilterProxyModel>());
  QVERIFY(!container.containsProxyModelOfType<QAbstractProxyModel>());
  /*
   * Check getting proxy model of type
   */
  QVERIFY(container.firstProxyModelOfType<RelationFilterProxyModel>() == &relationModel);
  QVERIFY(container.firstProxyModelOfType<FilterProxyModel>() == &filterModel);
  QVERIFY(container.firstProxyModelOfType<SortProxyModel>() == &sortModel);
  QVERIFY(container.firstProxyModelOfType<QSortFilterProxyModel>() == nullptr);
  QVERIFY(container.firstProxyModelOfType<QAbstractProxyModel>() == nullptr);
}

void ProxyModelContainerTest::removeAtTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  QPointer<RelationFilterProxyModel> relationModel = new RelationFilterProxyModel;
  QPointer<FilterProxyModel>  filterModel = new FilterProxyModel;
  QPointer<SortProxyModel> sortModel = new SortProxyModel;

  /*
   * Setup container
   */
  container.setSourceModel(&model);
  container.appendProxyModel(relationModel);
  container.appendProxyModel(filterModel);
  container.appendProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 3);
  QVERIFY(container.firstProxyModel() == relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.proxyModelAt(1) == filterModel);
  QVERIFY(container.proxyModelAt(1)->sourceModel() == relationModel);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == filterModel);
  /*
   * Remove in the middle
   */
  container.removeProxyModelAt(1);
  QCOMPARE(container.proxyModelCount(), 2);
  QVERIFY(container.firstProxyModel() == relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == relationModel);
  /*
   * Remove first
   */
  container.removeProxyModelAt(0);
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &model);
  /*
   * Remove last
   */
  container.removeProxyModelAt(0);
  QCOMPARE(container.proxyModelCount(), 0);
  QVERIFY(!relationModel.isNull());
  QVERIFY(!filterModel.isNull());
  QVERIFY(!sortModel.isNull());
  /*
   * Restore
   */
  container.setSourceModel(&model);
  container.appendProxyModel(relationModel);
  container.appendProxyModel(filterModel);
  container.appendProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 3);
  QVERIFY(container.firstProxyModel() == relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.proxyModelAt(1) == filterModel);
  QVERIFY(container.proxyModelAt(1)->sourceModel() == relationModel);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == filterModel);
  /*
   * Delete in the middle
   */
  container.deleteProxyModelAt(1);
  QCOMPARE(container.proxyModelCount(), 2);
  QVERIFY(container.firstProxyModel() == relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == relationModel);
  /*
   * Delete first
   */
  container.deleteProxyModelAt(0);
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == &model);

  /*
   * Delete last
   */
  container.deleteProxyModelAt(0);
  QCOMPARE(container.proxyModelCount(), 0);
  QVERIFY(relationModel.isNull());
  QVERIFY(filterModel.isNull());
  QVERIFY(sortModel.isNull());
}

void ProxyModelContainerTest::removeAtIndexBugTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  QPointer<FilterProxyModel>  filterModel = new FilterProxyModel;
  QPointer<SortProxyModel> sortModel = new SortProxyModel;

  /*
   * Setup container
   */
  container.setSourceModel(&model);
  container.appendProxyModel(filterModel);
  container.appendProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 2);
  QVERIFY(container.firstProxyModel() == filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == filterModel);
  /*
   * Remove last
   */
  container.removeProxyModelAt(1);
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == filterModel);
  /*
   * Remove last
   */
  container.removeProxyModelAt(0);
  QCOMPARE(container.proxyModelCount(), 0);
  /*
   * Free
   */
  delete filterModel;
  delete sortModel;
}

void ProxyModelContainerTest::searchRemoveTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  QPointer<RelationFilterProxyModel> relationModel = new RelationFilterProxyModel;
  QPointer<FilterProxyModel>  filterModel = new FilterProxyModel;
  QPointer<SortProxyModel> sortModel = new SortProxyModel;

  /*
   * Setup container
   */
  container.setSourceModel(&model);
  container.appendProxyModel(relationModel);
  container.appendProxyModel(filterModel);
  container.appendProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 3);
  QVERIFY(container.firstProxyModel() == relationModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.proxyModelAt(1) == filterModel);
  QVERIFY(container.proxyModelAt(1)->sourceModel() == relationModel);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == filterModel);
  /*
   * Remove existing
   */
  container.removeFirstProxyModelOfType<RelationFilterProxyModel>();
  QCOMPARE(container.proxyModelCount(), 2);
  QVERIFY(container.firstProxyModel() == filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  /*
   * Try to remove non existing
   */
  container.removeFirstProxyModelOfType<QAbstractProxyModel>();
  QCOMPARE(container.proxyModelCount(), 2);
  /*
   * Delete existing
   */
  container.deleteFirstProxyModelOfType<FilterProxyModel>();
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == sortModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  /*
   * Try to delete non existing
   */
  container.deleteFirstProxyModelOfType<QAbstractProxyModel>();
  QCOMPARE(container.proxyModelCount(), 1);
  /*
   * Check pointers and free
   */
  QVERIFY(!relationModel.isNull());
  QVERIFY(filterModel.isNull());
  QVERIFY(!sortModel.isNull());
  delete relationModel;
  delete sortModel;
}

void ProxyModelContainerTest::searchPointerRemoveTest()
{
  ProxyModelContainer container;
  VariantTableModel model;
  QPointer<FilterProxyModel>  filterModel = new FilterProxyModel;
  QPointer<SortProxyModel> sortModel = new SortProxyModel;
  QPointer<QSortFilterProxyModel> notInContainerModel = new QSortFilterProxyModel;
  /*
   * Setup container
   */
  container.setSourceModel(&model);
  container.appendProxyModel(filterModel);
  container.appendProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 2);
  QVERIFY(container.firstProxyModel() == filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == sortModel);
  QVERIFY(container.lastProxyModel()->sourceModel() == filterModel);
  /*
   * Remove existing
   */
  container.removeProxyModel(sortModel);
  QCOMPARE(container.proxyModelCount(), 1);
  QVERIFY(container.firstProxyModel() == filterModel);
  QVERIFY(container.firstProxyModel()->sourceModel() == &model);
  QVERIFY(container.lastProxyModel() == filterModel);
  /*
   * Try to remove non existing
   */
  container.removeProxyModel(notInContainerModel);
  QCOMPARE(container.proxyModelCount(), 1);
  /*
   * Try to delete non existing
   */
  container.deleteProxyModel(notInContainerModel);
  QCOMPARE(container.proxyModelCount(), 1);
  /*
   * Delete existing
   */
  container.deleteProxyModel(filterModel);
  QCOMPARE(container.proxyModelCount(), 0);
  /*
   * Check pointers and free
   */
  QVERIFY(!sortModel.isNull());
  delete sortModel;
  QVERIFY(filterModel.isNull());
  QVERIFY(!notInContainerModel.isNull());
  delete notInContainerModel;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ProxyModelContainerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
