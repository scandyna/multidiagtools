#include "mdtParentChildTableModel.h"

#include <QVariant>
#include <QString>
#include <iostream>
#include <QDebug>
#include <QStandardItemModel>
 
using namespace std;

mdtParentChildTableModel::mdtParentChildTableModel(QObject *parent)
 : QAbstractItemModel(parent)
{
  // Root item
  pvRootItem = new mdtParentChildTableItem;
  // Set data ...
  setupModelData();
}

mdtParentChildTableModel::~mdtParentChildTableModel()
{
  delete pvRootItem;
}

int mdtParentChildTableModel::rowCount(const QModelIndex &parent) const
{
  mdtParentChildTableItem *parentItem;

  // We don't recalc all for a call with column other than first
  if(parent.column() > 0){
    return 0;
  }
  // Get correct parent item
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtParentChildTableItem*>(parent.internalPointer());
  }
  return parentItem->childCount();
}

int mdtParentChildTableModel::columnCount(const QModelIndex &parent) const
{
  mdtParentChildTableItem *parentItem;
  mdtParentChildTableItem *childItem;

  // Set the correct parent
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtParentChildTableItem*>(parent.internalPointer());
  }
  // Find the child corresponding to the request row
  childItem = parentItem->child(parentItem->row());
  if(childItem == 0){
    return 0;
  }
  return childItem->columnCount();
}

QVariant mdtParentChildTableModel::data(const QModelIndex &index, int role) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return QVariant();
  }
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  mdtParentChildTableItem *item = static_cast<mdtParentChildTableItem*>(index.internalPointer());

  return item->data(index.column(), role);
}

void mdtParentChildTableModel::setHeaderData(QList<QVariant> &headerData)
{
  pvHeaderData = headerData;
}

QVariant mdtParentChildTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  // Return the section with correct title
  if(orientation == Qt::Horizontal){
    return pvHeaderData.value(section);
  }else{
    return QString("Row %1").arg(section);
  }
}

Qt::ItemFlags mdtParentChildTableModel::flags(const QModelIndex &index) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable;
}

QModelIndex mdtParentChildTableModel::index(int row, int column, const QModelIndex &parent) const
{
  if(!hasIndex(row, column, parent)){
    return QModelIndex();
  }

  // Get the parent item
  mdtParentChildTableItem *parentItem;
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtParentChildTableItem*>(parent.internalPointer());
  }
  // Get child of this parent item for request row
  mdtParentChildTableItem *childItem = parentItem->child(row);
  if(childItem == 0){
    return QModelIndex();
  }
  return createIndex(row, column, childItem);
}

QModelIndex mdtParentChildTableModel::parent(const QModelIndex &index) const
{
  if(!index.isValid()){
    return QModelIndex();
  }
  
  mdtParentChildTableItem *childItem = static_cast<mdtParentChildTableItem*>(index.internalPointer());
  mdtParentChildTableItem *parentItem = childItem->parent();

  if(parentItem == pvRootItem){
    return QModelIndex();
  }
  return createIndex(parentItem->row(), 0, parentItem);
}

void mdtParentChildTableModel::setupModelData()
{
  QList<QVariant> header;
  header << "A" << "B" << "C" << "D";
  
  QStandardItemModel *tbl1 = new QStandardItemModel(2, 3);
  for (int row = 0; row < 2; ++row) {
    for (int column = 0; column < 3; ++column) {
      QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
        tbl1->setItem(row, column, item);
    }
  }
  setHeaderData(header);
  mdtParentChildTableItem *item, *lastItem;
  // Create a item for each row in data model
  for(int i=0; i<tbl1->rowCount(); i++){
    item = new mdtParentChildTableItem(tbl1, i, pvRootItem);
    pvRootItem->appendChild(item);
  }
  lastItem = item;
  for(int i=0; i<tbl1->rowCount(); i++){
    item = new mdtParentChildTableItem(tbl1, i, lastItem);
    lastItem->appendChild(item);
  }
  for(int i=0; i<tbl1->rowCount(); i++){
    item = new mdtParentChildTableItem(tbl1, i, pvRootItem);
    pvRootItem->appendChild(item);
  }

  
}
