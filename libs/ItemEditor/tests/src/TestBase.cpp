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
#include "TestBase.h"
#include "ItemViewTestEdit.h"
#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

void TestBase::displayWidget(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  widget->show();
  while(widget->isVisible()){
    QTest::qWait(500);
  }
}

void TestBase::displayWidget(QWidget& widget)
{
  displayWidget(&widget);
}

void TestBase::displayModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  QDialog dialog;
  auto *layout = new QGridLayout;

  auto *listView = new QListView;
  auto *tableView = new QTableView;
  auto *treeView = new QTreeView;
  auto *comboBox = new QComboBox;
  auto *editableComboBox = new QComboBox;

  layout->addWidget(listView, 0, 0, 3, 1);
  layout->addWidget(tableView, 0, 1, 3, 1);
  layout->addWidget(treeView, 0, 2, 3, 1);
  layout->addWidget(comboBox, 0, 3, 1, 1);
  layout->addWidget(editableComboBox, 1, 3, 1, 1);
  dialog.setLayout(layout);

  listView->setModel(model);
  tableView->setModel(model);
  treeView->setModel(model);
  comboBox->setModel(model);
  comboBox->setMinimumWidth(100);
  editableComboBox->setModel(model);
  editableComboBox->setMinimumWidth(100);
  editableComboBox->setEditable(true);

  dialog.setWindowTitle("Dialog to display a model");
  dialog.resize(1000, 500);
  dialog.exec();
}

void TestBase::displayModel(QAbstractItemModel& model)
{
  displayModel(&model);
}

void TestBase::displayModels(QAbstractItemModel* sourceModel, QAbstractProxyModel* proxyModel)
{
  Q_ASSERT(sourceModel != nullptr);
  Q_ASSERT(proxyModel != nullptr);

  QDialog dialog;
  auto *layout = new QGridLayout;
  auto *sourceViewLabel = new QLabel("Source");
  auto *sourceView = new QTableView;
  auto *proxyViewLabel = new QLabel("Proxy");
  auto *proxyView = new QTableView;

  layout->addWidget(sourceViewLabel, 0, 0);
  layout->addWidget(sourceView, 1, 0);
  layout->addWidget(proxyViewLabel, 0, 1);
  layout->addWidget(proxyView, 1, 1);
  dialog.setLayout(layout);

  sourceView->setModel(sourceModel);
  proxyView->setModel(proxyModel);

  dialog.setWindowTitle("Dialog to display source and proxy model");
  dialog.resize(1000, 500);
  dialog.exec();
}

void TestBase::displayModels(QAbstractItemModel& sourceModel, QAbstractProxyModel& proxyModel)
{
  displayModels(&sourceModel, &proxyModel);
}

bool TestBase::setModelData(QAbstractItemModel* model, int row, int column, const QVariant& value, Qt::ItemDataRole role)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  auto index = model->index(row, column);
  if(!index.isValid()){
    qDebug() << "TestBase::setModelData() - index is not valid: " << index;
    return false;
  }

  return model->setData(index, value, role);
}

bool TestBase::setModelData(QAbstractItemModel& model, int row, int column, const QVariant& value, Qt::ItemDataRole role)
{
  return setModelData(&model, row, column, value, role);
}

QVariant TestBase::getModelData(const QAbstractItemModel* model, int row, int column, Qt::ItemDataRole role)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  auto index = model->index(row, column);
  if(!index.isValid()){
    qDebug() << "TestBase::getModelData() - index is not valid: " << index;
    return QVariant();
  }

  return model->data(index, role);
}

QVariant TestBase::getModelData(const QAbstractItemModel& model, int row, int column, Qt::ItemDataRole role)
{
  return getModelData(&model, row, column, role);
}

Qt::ItemFlags TestBase::getModelFlags(const QAbstractItemModel* model, int row, int column)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  auto index = model->index(row, column);
  if(!index.isValid()){
    qDebug() << "TestBase::getModelFlags() - index is not valid: " << index;
    return Qt::NoItemFlags;
  }

  return model->flags(index);
}

Qt::ItemFlags TestBase::getModelFlags(const QAbstractItemModel& model, int row, int column)
{
  return getModelFlags(&model, row, column);
}

void TestBase::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void TestBase::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
}

void TestBase::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void TestBase::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
}
