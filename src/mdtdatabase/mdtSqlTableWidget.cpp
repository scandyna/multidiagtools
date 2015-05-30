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
#include "mdtSqlTableWidget.h"
#include "mdtSortFilterProxyModel.h"
#include "mdtUiMessageHandler.h"
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
#include <QStringList>

#include <QItemSelection>

#include <QKeyEvent>
#include <QKeySequence>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <memory>

//#include <QDebug>

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
 * mdtSqlTableWidget implementation
 */

mdtSqlTableWidget::mdtSqlTableWidget(QWidget *parent)
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
  // Setup table view controller
  pvController->setTableView(pvTableView);
  connect( pvController.get(), SIGNAL(doubleClicked(const QSqlRecord&)), this, SIGNAL(doubleClicked(const QSqlRecord&)) );
  // Install event filter on table view to catch some key events
  /**
  mdtSqlTableWidgetKeyEventFilter *keyEventFilter = new mdtSqlTableWidgetKeyEventFilter(this);
  pvTableView->installEventFilter(keyEventFilter);
  connect(keyEventFilter, SIGNAL(knownKeyPressed(int)), this, SLOT(onTableViewKnownKeyPressed(int)));
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
  createLocalEditionElements();
  connectLocalEditionSignals();
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

  disconnect( pvController.get(), SIGNAL(doubleClicked(const QSqlRecord&)), this, SIGNAL(doubleClicked(const QSqlRecord&)) );
  disconnectLocalEditionSignals();
  pvController = controller;
  pvController->setTableView(pvTableView);
  connect( pvController.get(), SIGNAL(doubleClicked(const QSqlRecord&)), this, SIGNAL(doubleClicked(const QSqlRecord&)) );
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
}

void mdtSqlTableWidget::setColumnHidden(int column, bool hide)
{
  pvTableView->setColumnHidden(column, hide);
  pvController->setDefaultColumnToSelect(pvController->firstVisibleColumnIndex());
}

void mdtSqlTableWidget::setColumnHidden(const QString &fieldName, bool hide)
{
  setColumnHidden(pvController->fieldIndex(fieldName), hide);
}

void mdtSqlTableWidget::setDefaultColumnToSelect(int column)
{
  pvController->setDefaultColumnToSelect(column);
}

void mdtSqlTableWidget::setDefaultColumnToSelect(const QString &fieldName)
{
  pvController->setDefaultColumnToSelect(fieldName);
}

// bool mdtSqlTableWidget::exportToCsvFile(const QFileInfo & csvFile, const mdtCsvFileSettings& csvSettings, bool includeHeader)
// {
//   mdtCsvFile file;
//   QStringList lineData;
//   int row, col;
// 
//   // Do some checks on existing csvFile
//   if(csvFile.exists()){
//     if(!csvFile.isFile()){
//       mdtError e(tr("Cannot export data as CSV to path") + "'" + csvFile.absolutePath() + "'" , mdtError::Error);
//       e.setInformativeText(tr("Given path is not a file."));
//       MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
//       e.commit();
//       pvController->setLastError(e);
//       return false;
//     }
//     if(!csvFile.isWritable()){
//       mdtError e(tr("Cannot export data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
//       e.setInformativeText(tr("No write access to given file."));
//       MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
//       e.commit();
//       pvController->setLastError(e);
//       return false;
//     }
//   }
//   // Open CSV file
//   file.setFileName(csvFile.absoluteFilePath());
//   if(!file.open(QIODevice::WriteOnly)){
//     mdtError e(tr("Cannot export data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
//     MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
//     e.commit();
//     pvController->setLastError(e);
//     return false;
//   }
//   // We call sort, so all data will be fetched (and sorted)
//   sort();
//   // Write header if requested
//   if(includeHeader){
//     for(col = 0; col < pvController->columnCount(); ++col){
//       if(!pvTableView->isColumnHidden(col)){
//         lineData << pvController->headerData(col).toString();
//       }
//     }
//     if(!file.writeLine(lineData, csvSettings)){
//       mdtError e(tr("Error occured while exporting data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
//       MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
//       e.commit();
//       pvController->setLastError(e);
//       return false;
//     }
//   }
//   // Write data part
//   for(row = 0; row < pvController->rowCount(false); ++row){
//     lineData.clear();
//     for(col = 0; col < pvController->columnCount(); ++col){
//       if(!pvTableView->isColumnHidden(col)){
//         lineData << pvController->data(row, col).toString();
//       }
//     }
//     if(!file.writeLine(lineData, csvSettings)){
//       mdtError e(tr("Error occured while exporting data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
//       MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
//       e.commit();
//       pvController->setLastError(e);
//       return false;
//     }
//   }
// 
//   return true;
// }

void mdtSqlTableWidget::resizeViewToContents()
{
  Q_ASSERT(pvTableView != 0);

  pvTableView->resizeColumnsToContents();
  pvTableView->resizeRowsToContents();
}

void mdtSqlTableWidget::refresh()
{
  if(!pvController->select()){
    mdtUiMessageHandler messageHandler(this);
    messageHandler.setError(pvController->lastError());
    messageHandler.displayToUser();
  }
}

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

bool mdtSqlTableWidget::exportToCsvFile(const QFileInfo & csvFile, const mdtCsvFileSettings & csvSettings, bool includeHeader, const std::vector<int> & columns)
{
  mdtCsvFile file;
  QStringList lineData;
  int row;

  // Do some checks on existing csvFile
  if(csvFile.exists()){
    if(!csvFile.isFile()){
      mdtError e(tr("Cannot export data as CSV to path") + "'" + csvFile.absolutePath() + "'" , mdtError::Error);
      e.setInformativeText(tr("Given path is not a file."));
      MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
      e.commit();
      pvController->setLastError(e);
      return false;
    }
    if(!csvFile.isWritable()){
      mdtError e(tr("Cannot export data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
      e.setInformativeText(tr("No write access to given file."));
      MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
      e.commit();
      pvController->setLastError(e);
      return false;
    }
  }
  // Open CSV file
  file.setFileName(csvFile.absoluteFilePath());
  if(!file.open(QIODevice::WriteOnly)){
    mdtError e(tr("Cannot export data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
    e.commit();
    pvController->setLastError(e);
    return false;
  }
  // We call sort, so all data will be fetched (and sorted)
  sort();
  // Write header if requested
  if(includeHeader){
    lineData = pvController->headerRowDataStr(columns);
    if(!file.writeLine(lineData, csvSettings)){
      mdtError e(tr("Error occured while exporting data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
      e.commit();
      pvController->setLastError(e);
      return false;
    }
  }
  // Write data part
  for(row = 0; row < pvController->rowCount(false); ++row){
    lineData = pvController->rowDataStr(row, columns);
    if(!file.writeLine(lineData, csvSettings)){
      mdtError e(tr("Error occured while exporting data to CSV file") + "'" + csvFile.absoluteFilePath() + "'" , mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSqlTableWidget");
      e.commit();
      pvController->setLastError(e);
      return false;
    }
  }

  return true;
}

vector<int> mdtSqlTableWidget::visibleColumns() const
{
  vector<int> lst;
  int col;

  for(col = 0; col < pvController->columnCount(); ++col){
    if(!pvTableView->isColumnHidden(col)){
      lst.emplace_back(col);
    }
  }

  return lst;
}

vector< int > mdtSqlTableWidget::columnsOfFieldNames ( const QStringList& fieldNames ) const
{
  vector<int> lst;

  for(auto & fldName : fieldNames){
    Q_ASSERT(pvController->fieldIndex(fldName) > -1);
    lst.emplace_back(pvController->fieldIndex(fldName));
  }

  return lst;
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
  // Add to layout
  pvBottomHorizontalLayout->addWidget(pbInsert);
  pvBottomHorizontalLayout->addWidget(pbSubmit);
  pvBottomHorizontalLayout->addWidget(pbRevert);
  pvBottomHorizontalLayout->addWidget(pbRemove);
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
