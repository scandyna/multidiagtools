/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlTableWidget.h"
#include "mdtSortFilterProxyModel.h"
#include <QTableView>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QPushButton>
#include <QList>
#include <QKeyEvent>
#include <QObject>
#include <QWidget>
#include <QIcon>

#include <QItemSelection>

#include <QKeyEvent>
#include <QKeySequence>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <memory>

#include <QDebug>

using namespace std;

/*
 * mdtSqlTableWidgetKeyEventFilter implementation
 */

/**
mdtSqlTableWidgetKeyEventFilter::mdtSqlTableWidgetKeyEventFilter(QObject *parent)
 : QObject(parent)
{
}

bool mdtSqlTableWidgetKeyEventFilter::eventFilter(QObject *obj, QEvent *event)
{
  Q_ASSERT(obj != 0);
  Q_ASSERT(event != 0);

  if(event->type() == QEvent::KeyPress){
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    // emit signal if we have a known key
    switch(keyEvent->key()){
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Delete:
      case Qt::Key_Down:
        emit knownKeyPressed(keyEvent->key());
        return true;
      default:
        return false;
    }
  }else{
    return QObject::eventFilter(obj, event);
  }
}
*/

/*
 * mdtSqlTableWidgetItemDelegate implementation
 */
/**
mdtSqlTableWidgetItemDelegate::mdtSqlTableWidgetItemDelegate(QObject *parent)
 : QStyledItemDelegate(parent)
{
}

QWidget *mdtSqlTableWidgetItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  emit dataEditionBegins();
  return QStyledItemDelegate::createEditor(parent, option, index);
}
*/

/*
 * mdtSqlTableWidget implementation
 */

mdtSqlTableWidget::mdtSqlTableWidget(QWidget *parent)
/// : mdtAbstractSqlWidget(parent),
///   pvController(new mdtSqlTableViewController)
 : QWidget(parent),
   pvController(new mdtSqlTableViewController)
{
  QVBoxLayout *layout = new QVBoxLayout;
  pvTopHorizontalLayout = new QHBoxLayout;
  pvBottomHorizontalLayout = new QHBoxLayout;
  QPushButton *pb = new QPushButton;

  // Setup table view
  pvTableView = new QTableView;
  pvTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
  ///connect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  // Setup table view controller
  pvController->setTableView(pvTableView);
  // Install event filter on table view to catch some key events
  /**
  mdtSqlTableWidgetKeyEventFilter *keyEventFilter = new mdtSqlTableWidgetKeyEventFilter(this);
  pvTableView->installEventFilter(keyEventFilter);
  connect(keyEventFilter, SIGNAL(knownKeyPressed(int)), this, SLOT(onTableViewKnownKeyPressed(int)));
  */
  // Set our custom item delegate
  /**
  mdtSqlTableWidgetItemDelegate *delegate = new mdtSqlTableWidgetItemDelegate(this);
  pvTableView->setItemDelegate(delegate);
  connect(delegate, SIGNAL(dataEditionBegins()), this, SLOT(onDataEditionBegins()));
  */
  // Button to resize view to contents
  pb = new QPushButton;
  pb->setIcon(QIcon::fromTheme("zoom-fit-best"));
  pb->setToolTip(tr("Resize view to contents"));
  connect(pb, SIGNAL(clicked()), this, SLOT(resizeViewToContents()));
  pvTopHorizontalLayout->addWidget(pb);
  // Button to refresh data
  pb = new QPushButton;
  pb->setIcon(QIcon::fromTheme("view-refresh"));
  pb->setToolTip(tr("Reload data"));
  connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
  pvTopHorizontalLayout->addWidget(pb);
  // Button to copy table to clipboard
  pb = new QPushButton;
  pb->setIcon(QIcon::fromTheme("edit-copy"));
  pb->setToolTip(tr("Copy all data to clipboard"));
  connect(pb, SIGNAL(clicked()), this, SLOT(copyTableToClipBoard()));
  pvTopHorizontalLayout->addWidget(pb);
  pvTopHorizontalLayout->addStretch();
  // Layouts
  layout->addLayout(pvTopHorizontalLayout);
  layout->addWidget(pvTableView);
  layout->addLayout(pvBottomHorizontalLayout);
  setLayout(layout);

  pbNavToFirst = 0;
  pbNavToPrevious = 0;
  pbNavToNext = 0;
  pbNavToLast = 0;
  createLocalEditionElements();
  connectLocalEditionSignals();
  ///pvDelegateIsEditingData = false;
  ///pvDefaultColumnToSelect = 0;

  setEditionEnabled(false);
}

mdtSqlTableWidget::~mdtSqlTableWidget()
{
}

void mdtSqlTableWidget::setTableName(const QString& tableName, QSqlDatabase db, const QString& userFriendlyTableName)
{
  pvController->setTableName(tableName, db, userFriendlyTableName);
}

void mdtSqlTableWidget::setTableController(shared_ptr< mdtSqlTableViewController > controller)
{
  Q_ASSERT(controller);

  disconnectLocalEditionSignals();
  pvController = controller;
  pvController->setTableView(pvTableView);
  connectLocalEditionSignals();
}

bool mdtSqlTableWidget::addChildTable(const mdtSqlRelationInfo& relationInfo, QSqlDatabase db, const QString& userFriendlyChildTableName)
{
  return pvController->addChildController<mdtSqlTableViewController>(relationInfo, db, userFriendlyChildTableName);
}

bool mdtSqlTableWidget::addChildTable(const mdtSqlRelationInfo& relationInfo, const QString& userFriendlyChildTableName)
{
  return pvController->addChildController<mdtSqlTableViewController>(relationInfo, userFriendlyChildTableName);
}

shared_ptr< mdtSqlTableViewController > mdtSqlTableWidget::tableController(const QString& tableName)
{
  if(pvController->tableName() == tableName){
    return pvController;
  }
  return pvController->childController<mdtSqlTableViewController>(tableName);
}

/**
int mdtSqlTableWidget::currentRow() const
{
  Q_ASSERT(proxyModel() != 0);

  QModelIndex index;

  qDebug() << "Current view index: " << pvTableView->currentIndex();
  index = proxyModel()->mapToSource(pvTableView->currentIndex());
  
  return index.row();
}
*/

void mdtSqlTableWidget::setEditionEnabled(bool enable)
{
  if(enable){
    pvTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
  }else{
    pvTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
}

void mdtSqlTableWidget::enableLocalEdition()
{
  pbInsert->setVisible(true);
  pbSubmit->setVisible(true);
  pbRevert->setVisible(true);
  pbRemove->setVisible(true);
  setEditionEnabled(true);
}

void mdtSqlTableWidget::addWidgetToLocalBar(QWidget *widget)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(pvBottomHorizontalLayout != 0);

  pvBottomHorizontalLayout->addWidget(widget);
}

void mdtSqlTableWidget::addStretchToLocalBar()
{
  Q_ASSERT(pvBottomHorizontalLayout != 0);

  pvBottomHorizontalLayout->addStretch();
}

QItemSelectionModel *mdtSqlTableWidget::selectionModel()
{
  return pvTableView->selectionModel();
}

QTableView *mdtSqlTableWidget::tableView()
{
  return pvTableView;
}

void mdtSqlTableWidget::setHeaderData(const QString &fieldName, const QString &data)
{
  pvController->setHeaderData(fieldName, data);
  /**
  Q_ASSERT(model() != 0);

  int column;

  column = model()->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  model()->setHeaderData(column, Qt::Horizontal, data);
  */
}

void mdtSqlTableWidget::setColumnHidden(int column, bool hide)
{
  pvTableView->setColumnHidden(column, hide);
  pvController->setDefaultColumnToSelect(pvController->firstVisibleColumnIndex());
  /**
  if(model() != 0){
    setDefaultColumnToSelect(firstVisibleColumnIndex());
  }
  */
}

void mdtSqlTableWidget::setColumnHidden(const QString &fieldName, bool hide)
{
  setColumnHidden(pvController->fieldIndex(fieldName), hide);
  /**
  Q_ASSERT(model() != 0);

  int column;

  column = model()->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  setColumnHidden(column, hide);
  */
}

void mdtSqlTableWidget::setDefaultColumnToSelect(int column)
{
  pvController->setDefaultColumnToSelect(column);
  /**
  Q_ASSERT(model() != 0);

  if((column < 0)||(column >= model()->columnCount())){
    pvDefaultColumnToSelect = 0;
  }else{
    pvDefaultColumnToSelect = column;
  }
  */
}

void mdtSqlTableWidget::setDefaultColumnToSelect(const QString &fieldName)
{
  pvController->setDefaultColumnToSelect(fieldName);
  /**
  Q_ASSERT(model() != 0);

  setDefaultColumnToSelect(model()->fieldIndex(fieldName));
  */
}

/**
void mdtSqlTableWidget::sort()
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(proxyModel() != 0);

  if(!proxyModel()->hasColumnToSort()){
    return;
  }
  // If model is empty, canFetchMore will allways return true, prevent this
  if(model()->rowCount() < 1){
    return;
  }
  while(model()->canFetchMore()){
    model()->fetchMore();
  }
  pvTableView->sortByColumn(0);
}
*/

// mdtSqlTableSelection mdtSqlTableWidget::currentSelection(const QStringList &fieldList)
// {
//   Q_ASSERT(model() != 0);
//   Q_ASSERT(selectionModel() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   mdtSqlTableSelection s;
//   QModelIndexList selectionIndexList;
//   QModelIndexList modelIndexList;
//   int i;
// 
//   /*
//    * Get selected indexes. We want that data are token from top-left to bottom right,
//    *  not the order of selection.
//    *  f.ex. if user selected items from bottom to top,
//    *  we want to return items from top to bottom.
//    */
//   selectionIndexList = selectionModel()->selectedIndexes();
//   qSort(selectionIndexList.begin(), selectionIndexList.end());
//   for(i = 0; i < selectionIndexList.size(); ++i){
//     modelIndexList.append(proxyModel()->mapToSource(selectionIndexList.at(i)));
//   }
//   s.setIndexList(modelIndexList, fieldList, model());
// 
//   return s;
// }


// mdtSqlTableSelection mdtSqlTableWidget::currentSelection(const QString &field)
// {
//   Q_ASSERT(model() != 0);
//   Q_ASSERT(selectionModel() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   QStringList fieldList;
// 
//   fieldList.append(field);
// 
//   return currentSelection(fieldList);
// }

QList<QModelIndexList> mdtSqlTableWidget::indexListOfSelectedRowsByRowsList(const QList<int> &columnList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QModelIndexList indexes;
  QList<QModelIndexList> result;
  QModelIndexList rowResult;
  QModelIndex index;
  QList<int> selectedRows;
  int i, j;
  int row;
  int column;

  // Get currently selected indexes
  indexes = selectionModel()->selectedIndexes();
  // Build a list of selected rows
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    Q_ASSERT(row >= 0);
    if(!selectedRows.contains(row)){
      selectedRows.append(row);
    }
  }
  qSort(selectedRows.begin(), selectedRows.end());
  // Build list with only interresting columns
  for(i = 0; i < selectedRows.size(); ++i){
    row = selectedRows.at(i);
    rowResult.clear();
    for(j = 0; j < columnList.size(); ++j){
      column = columnList.at(j);
      index = model()->index(row, column);
      Q_ASSERT(index.isValid());
      rowResult.append(index);
    }
    result.append(rowResult);
  }

  return result;
}

QList<QModelIndexList> mdtSqlTableWidget::indexListOfSelectedRowsByRowsList(const QStringList &fieldList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QList<int> columns;
  int i;

  for(i = 0; i < fieldList.size(); ++i){
    columns.append(model()->fieldIndex(fieldList.at(i)));
  }

  return indexListOfSelectedRowsByRowsList(columns);
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QList<int> &columnList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QModelIndexList indexes;
  QModelIndexList result;
  QModelIndex index;
  QList<int> selectedRows;
  int i, j;
  int row;
  int column;

  // Get currently selected indexes
  indexes = selectionModel()->selectedIndexes();
  // Build a list of selected rows
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    Q_ASSERT(row >= 0);
    if(!selectedRows.contains(row)){
      selectedRows.append(row);
    }
  }
  qSort(selectedRows.begin(), selectedRows.end());
  // Build list with only interresting columns
  for(i = 0; i < selectedRows.size(); ++i){
    row = selectedRows.at(i);
    for(j = 0; j < columnList.size(); ++j){
      column = columnList.at(j);
      index = model()->index(row, column);
      Q_ASSERT(index.isValid());
      result.append(index);
    }
  }

  return result;
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QStringList &fieldList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QList<int> columns;
  int i;

  for(i = 0; i < fieldList.size(); ++i){
    columns.append(model()->fieldIndex(fieldList.at(i)));
  }

  return indexListOfSelectedRows(columns);
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QString &field)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  return indexListOfSelectedRows(QStringList(field));
}

/**
void mdtSqlTableWidget::setCurrentIndex(int row)
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  QModelIndex index;

  if(row >= 0){
    index = model()->index(row, pvDefaultColumnToSelect);
  }
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}
*/

void mdtSqlTableWidget::resizeViewToContents()
{
  Q_ASSERT(pvTableView != 0);

  pvTableView->resizeColumnsToContents();
  pvTableView->resizeRowsToContents();
}

void mdtSqlTableWidget::refresh()
{
  pvController->select();
}

/**
void mdtSqlTableWidget::onDataEditionBegins()
{
  pvDelegateIsEditingData = true;
  emit dataEdited();
}
*/

/**
void mdtSqlTableWidget::onDataChanged(const QModelIndex &, const QModelIndex &)
{
  pvDelegateIsEditingData = false;
}
*/

// void mdtSqlTableWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
// {
//   Q_ASSERT(proxyModel() != 0);
// 
//   if(!childWidgetsAreInVisaluzingState(true)){
//     return;
//   }
//   /*
//    * Here, we have to check the source of the received index.
//    * If it comes from selection model (from view),
//    *  we have to map the index to source model index,
//    *  else we have a problem when sorting is activ.
//    */
//   if(current.isValid()){
//     if(current.model() == proxyModel()){
//       QModelIndex index;
//       index = proxyModel()->mapToSource(current);
//       emit currentRowChanged(index.row());
//     }else{
//       emit currentRowChanged(current.row());
//     }
//   }else{
//     emit currentRowChanged(-1);
//   }
// }

// void mdtSqlTableWidget::onTableViewKnownKeyPressed(int key)
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(model() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   QModelIndex index;
// 
//   switch(key){
//     case Qt::Key_Enter:
//     case Qt::Key_Return:
//       /*
//        * We want to save data if we are in Editing or EditingNewRow state.
//        * But, we must be shure that no editor is open in delegate to prevent data loss.
//        * (If we call submit()/submitAll() on model, it will recieve invalid data
//        *  for index that has a opened editor, and silently revert these data)
//        */
//       // Save data if we are in Editing state
//       if((currentState() == Editing) || (currentState() == EditingNewRow)){
//         if(!pvDelegateIsEditingData){
//           submit();
//         }
//       }
//       break;
//     case Qt::Key_Down:
//       // If we are at last row, we insert one - Only in visualizing state
//       index = pvTableView->selectionModel()->currentIndex();
//       if((index.row() == (model()->rowCount() - 1)) && (currentState() == Visualizing) && (pvTableView->editTriggers() != QAbstractItemView::NoEditTriggers)){
//         // Insert new row and select it
//         insert();
//       }
//       // Select new current index
//       index = proxyModel()->index(index.row()+1, index.column());
//       if(index.isValid()){
//         switch(pvTableView->selectionBehavior()){
//           case QAbstractItemView::SelectItems:
//             pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select);
//             break;
//           case QAbstractItemView::SelectRows:
//             pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select | QItemSelectionModel::Rows);
//             break;
//           case QAbstractItemView::SelectColumns:
//             pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select | QItemSelectionModel::Columns);
//             break;
//         }
//       }
//   }
// }

// void mdtSqlTableWidget::onModelSelected()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   QModelIndex index;
// 
//   if(model() == 0){
//     return;
//   }
//   if(model()->rowCount() > 0){
//     sort();
//     index = proxyModel()->index(0, pvDefaultColumnToSelect);
//     pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
//     qDebug() << "mdtSqlTableWidget::onModelSelected() , selected index: " << index;
//     pvTableView->resizeRowsToContents();
//   }else{
//     onCurrentRowChanged(index, index);
//   }
// }

// void mdtSqlTableWidget::copyTableToClipBoard()
// {
//   Q_ASSERT(model() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   QModelIndex index;
//   QClipboard *clipboard = QApplication::clipboard();
//   QString data;
//   QSortFilterProxyModel *m;
//   int row, col;
// 
//   m = proxyModel();
//   // We call sort, so all data will be fetched (and sorted)
//   sort();
//   // Copy header
//   for(col = 0; col < m->columnCount(); ++col){
//     if(!pvTableView->isColumnHidden(col)){
//       data += "\"" + m->headerData(col, Qt::Horizontal).toString() + "\"";
//       if(col < (m->columnCount()-1)){
//         data += "\t";
//       }
//     }
//   }
//   data += "\n";
//   // copy data
//   for(row = 0; row < m->rowCount(); ++row){
//     for(col = 0; col < m->columnCount(); ++col){
//       index = m->index(row, col);
//       if(!pvTableView->isColumnHidden(index.column())){
//         data += "\"" + m->data(index).toString() + "\"";
//         if(col < (m->columnCount()-1)){
//           data += "\t";
//         }
//       }
//     }
//     data += "\n";
//   }
//   // Send to clipboard
//   clipboard = QApplication::clipboard();
//   Q_ASSERT(clipboard != 0);
//   QMimeData *mimeData = new QMimeData;
//   mimeData->setText(data);
//   clipboard->setMimeData(mimeData);
// }

void mdtSqlTableWidget::copyTableToClipBoard()
{
  QClipboard *clipboard = QApplication::clipboard();
  QString data;
  int row, col;

  // We call sort, so all data will be fetched (and sorted)
  sort();
  // Copy header
  for(col = 0; col < pvController->columnCount(); ++col){
    if(!pvTableView->isColumnHidden(col)){
      data += "\"" + pvController->headerData(col).toString() + "\"";
      if(col < (pvController->columnCount()-1)){
        data += "\t";
      }
    }
  }
  data += "\n";
  // copy data
  for(row = 0; row < pvController->rowCount(false); ++row){
    for(col = 0; col < pvController->columnCount(); ++col){
      if(!pvTableView->isColumnHidden(col)){
        data += "\"" + pvController->data(row, col).toString() + "\"";
        if(col < (pvController->columnCount()-1)){
          data += "\t";
        }
      }
    }
    data += "\n";
  }
  // Send to clipboard
  clipboard = QApplication::clipboard();
  Q_ASSERT(clipboard != 0);
  QMimeData *mimeData = new QMimeData;
  mimeData->setText(data);
  clipboard->setMimeData(mimeData);
}

void mdtSqlTableWidget::keyPressEvent(QKeyEvent* event)
{
  Q_ASSERT(event != 0);

  if(event->matches(QKeySequence::Copy)){
    copySelectionToClipBoard();
  }
}

// void mdtSqlTableWidget::copySelectionToClipBoard()
// {
//   Q_ASSERT(model() != 0);
//   Q_ASSERT(selectionModel() != 0);
//   Q_ASSERT(proxyModel() != 0);
// 
//   QModelIndexList selectionIndexList;
//   QModelIndex previousModelIndex, currentModelIndex;
//   QClipboard *clipboard = QApplication::clipboard();
//   QString data;
//   int i;
// 
//   /*
//    * Get selected indexes. We want that data are token from top-left to bottom right,
//    *  not the order of selection.
//    *  f.ex. if user selected items from bottom to top,
//    *  we want to return items from top to bottom.
//    */
//   selectionIndexList = selectionModel()->selectedIndexes();
//   qSort(selectionIndexList.begin(), selectionIndexList.end());
//   for(i = 0; i < selectionIndexList.size(); ++i){
//     if(!pvTableView->isColumnHidden(selectionIndexList.at(i).column())){
//       currentModelIndex = proxyModel()->mapToSource(selectionIndexList.at(i));
//       if(previousModelIndex.isValid()){
//         if(currentModelIndex.row() == previousModelIndex.row()){
//           data += "\t";
//         }else{
//           data += "\n";
//         }
//       }
//       data += "\"" + model()->data(currentModelIndex).toString() + "\"";
//     }
//     previousModelIndex = currentModelIndex;
//   }
//   // Send to clipboard
//   clipboard = QApplication::clipboard();
//   Q_ASSERT(clipboard != 0);
//   QMimeData *mimeData = new QMimeData;
//   mimeData->setText(data);
//   clipboard->setMimeData(mimeData);
// }

void mdtSqlTableWidget::copySelectionToClipBoard()
{
  Q_ASSERT(selectionModel() != 0);

  QModelIndexList selectionIndexList;
  QModelIndex previousModelIndex, currentModelIndex;
  QClipboard *clipboard = QApplication::clipboard();
  QString data;
  int i;

  /*
   * Get selected indexes. We want that data are token from top-left to bottom right,
   *  not the order of selection.
   *  f.ex. if user selected items from bottom to top,
   *  we want to return items from top to bottom.
   */
  selectionIndexList = selectionModel()->selectedIndexes();
  qSort(selectionIndexList.begin(), selectionIndexList.end());
  for(i = 0; i < selectionIndexList.size(); ++i){
    if(!pvTableView->isColumnHidden(selectionIndexList.at(i).column())){
      if(previousModelIndex.isValid()){
        if(currentModelIndex.row() == previousModelIndex.row()){
          data += "\t";
        }else{
          data += "\n";
        }
      }
      data += "\"" + pvController->data(currentModelIndex).toString() + "\"";
    }
    previousModelIndex = currentModelIndex;
  }
  // Send to clipboard
  clipboard = QApplication::clipboard();
  Q_ASSERT(clipboard != 0);
  QMimeData *mimeData = new QMimeData;
  mimeData->setText(data);
  clipboard->setMimeData(mimeData);
}

// void mdtSqlTableWidget::doSetModel(QSqlTableModel *model)
// {
//   Q_ASSERT(model != 0);
// 
//   model->setEditStrategy(QSqlTableModel::OnManualSubmit);
//   pvTableView->setModel(proxyModel());
//   connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
//   // A selection model is created after setModel() by view itself
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   connect(pvTableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onCurrentRowChanged(const QModelIndex&, const QModelIndex&)));
//   pvDelegateIsEditingData = false;
// }

// bool mdtSqlTableWidget::doSubmit()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(model() != 0);
// 
//   if(!model()->submitAll()){
//     displayDatabaseError(model()->lastError());
//     return false;
//   }
// 
//   return true;
// }

// bool mdtSqlTableWidget::doRevert()
// {
//   Q_ASSERT(model() != 0);
// 
//   model()->revertAll();
// 
//   return true;
// }

// bool mdtSqlTableWidget::doInsert()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(model() != 0);
// 
//   int row;
//   QModelIndex index;
// 
//   // Insert new row at end
//   row = model()->rowCount();
//   model()->insertRow(row);
//   index = model()->index(row, pvDefaultColumnToSelect);
//   pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
//   qDebug() << "doInsert, selected index: " << index;
// 
//   return true;
// }

// bool mdtSqlTableWidget::doSubmitNewRow()
// {
//   return doSubmit();
// }

// bool mdtSqlTableWidget::doRevertNewRow()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(model() != 0);
// 
//   int row;
// 
//   // We simply remove the last row
//   row = model()->rowCount()-1;
//   if(row >= 0){
//     if(!pvTableView->model()->removeRow(row)){
//       displayDatabaseError(model()->lastError());
//       return false;
//     }
//   }
// 
//   return true;
// }

// bool mdtSqlTableWidget::doRemove()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
//   Q_ASSERT(model() != 0);
// 
//   int i;
//   int ret;
//   int row;
//   QList<int> rows;
//   QMessageBox msgBox;
//   QModelIndexList indexes = pvTableView->selectionModel()->selectedIndexes();
// 
//   // If nothing was selected, we do nothing
//   if(indexes.size() < 1){
//     return true;
//   }
//   // We ask confirmation to the user
//   msgBox.setText(tr("You are about to delete selected rows in table ") + userFriendlyTableName());
//   msgBox.setInformativeText(tr("Do you want to continue ?"));
//   msgBox.setIcon(QMessageBox::Warning);
//   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//   msgBox.setDefaultButton(QMessageBox::No);
//   ret = msgBox.exec();
//   if(ret != QMessageBox::Yes){
//     return true;
//   }
//   // Build rows list
//   for(i = 0; i < indexes.size(); ++i){
//     row = indexes.at(i).row();
//     if(!rows.contains(row)){
//       rows.append(row);
//     }
//   }
//   // Delete selected rows
//   for(i = 0; i < rows.size(); ++i){
//     if(!model()->removeRow(rows.at(i))){
//       displayDatabaseError(model()->lastError());
//       model()->revertAll();
//       return false;
//     }
//   }
//   if(!model()->submitAll()){
//     model()->revert();
//     displayDatabaseError(model()->lastError());
//     return false;
//   }
// 
//   return true;
// }

// void mdtSqlTableWidget::toFirst()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
// 
//   QModelIndex index;
// 
//   if(pvTableView->model() == 0){
//     return;
//   }
//   index = pvTableView->model()->index(0, pvDefaultColumnToSelect);
//   pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
// }
// 
// void mdtSqlTableWidget::toLast()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
// 
//   QModelIndex index;
//   int row;
// 
//   if(pvTableView->model() == 0){
//     return;
//   }
//   index = pvTableView->selectionModel()->currentIndex();
//   row = pvTableView->model()->rowCount()-1;
//   if(index.row() < row){
//     index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
//     pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
//   }
// }
// 
// void mdtSqlTableWidget::toPrevious()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
// 
//   QModelIndex index;
//   int row;
// 
//   if(pvTableView->model() == 0){
//     return;
//   }
//   // Get current index, check that we are not at the begining and move if Ok
//   index = pvTableView->selectionModel()->currentIndex();
//   if(index.row() > 0){
//     row = index.row()-1;
//     index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
//     pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
//   }
// }
// 
// void mdtSqlTableWidget::toNext()
// {
//   Q_ASSERT(pvTableView->selectionModel() != 0);
// 
//   QModelIndex index;
//   int row;
// 
//   if(pvTableView->model() == 0){
//     return;
//   }
//   // Get current index, check that we are not at the end and move if Ok
//   index = pvTableView->selectionModel()->currentIndex();
//   if(index.row() < (pvTableView->model()->rowCount())-1){
//     row = index.row()+1;
//     index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
//     pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
//   }
// }

/**
int mdtSqlTableWidget::firstVisibleColumnIndex()
{
  Q_ASSERT(model() != 0);

  int col;

  for(col = 0; col < model()->columnCount(); ++col){
    if(!pvTableView->isColumnHidden(col)){
      qDebug() << "firstVisibleColumnIndex(): " << col;
      return col;
    }
  }

  return -1;
}
*/

void mdtSqlTableWidget::createLocalEditionElements()
{
  Q_ASSERT(layout() != 0);
  Q_ASSERT(pvBottomHorizontalLayout != 0);

  // Create actions
  pbInsert = new QPushButton("New");
  pbSubmit = new QPushButton("Save");
  pbRevert = new QPushButton("Cancel");
  pbRemove = new QPushButton("Delete");
  // As default, functions are disabled
  pbInsert->setVisible(false);
  pbSubmit->setVisible(false);
  pbRevert->setVisible(false);
  pbRemove->setVisible(false);
  /**
  connect(this, SIGNAL(insertEnabledStateChanged(bool)), pbInsert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(removeEnabledStateChanged(bool)), pbRemove, SLOT(setEnabled(bool)));
  */
  /**
  connect(pbInsert, SIGNAL(clicked()), this, SLOT(insert()));
  connect(pbSubmit, SIGNAL(clicked()), this, SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), this, SLOT(revert()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(remove()));
  */
  // Add to layout
  pvBottomHorizontalLayout->addWidget(pbInsert);
  pvBottomHorizontalLayout->addWidget(pbSubmit);
  pvBottomHorizontalLayout->addWidget(pbRevert);
  pvBottomHorizontalLayout->addWidget(pbRemove);
  // Update buttons first time
  ///setEditionEnabled(true);
}

void mdtSqlTableWidget::connectLocalEditionSignals()
{
  // Connect actions enable/disable
  connect(pvController.get(), SIGNAL(insertEnabledStateChanged(bool)), pbInsert, SLOT(setEnabled(bool)));
  connect(pvController.get(), SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(pvController.get(), SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  connect(pvController.get(), SIGNAL(removeEnabledStateChanged(bool)), pbRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(pbInsert, SIGNAL(clicked()), pvController.get(), SLOT(insert()));
  connect(pbSubmit, SIGNAL(clicked()), pvController.get(), SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), pvController.get(), SLOT(revert()));
  connect(pbRemove, SIGNAL(clicked()), pvController.get(), SLOT(remove()));
}

void mdtSqlTableWidget::disconnectLocalEditionSignals()
{
  // Connect actions enable/disable
  disconnect(pvController.get(), SIGNAL(insertEnabledStateChanged(bool)), pbInsert, SLOT(setEnabled(bool)));
  disconnect(pvController.get(), SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  disconnect(pvController.get(), SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  disconnect(pvController.get(), SIGNAL(removeEnabledStateChanged(bool)), pbRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  disconnect(pbInsert, SIGNAL(clicked()), pvController.get(), SLOT(insert()));
  disconnect(pbSubmit, SIGNAL(clicked()), pvController.get(), SLOT(submit()));
  disconnect(pbRevert, SIGNAL(clicked()), pvController.get(), SLOT(revert()));
  disconnect(pbRemove, SIGNAL(clicked()), pvController.get(), SLOT(remove()));
}
