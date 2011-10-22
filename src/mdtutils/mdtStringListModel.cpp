#include "mdtStringListModel.h"

#include <QVariant>
#include <QString>
#include <iostream>
#include <QDebug>
#include <QStandardItemModel>
 
using namespace std;

mdtStringListModel::mdtStringListModel(const QStringList &strings, QObject *parent)
 : QAbstractItemModel(parent)
{
  // Root item
  QList<QVariant> headerData;
  headerData << "A" << "B" << "C";
  //QStandardItemModel *tbl1 = new QStandardItemModel(2, 3);
  
  pvRootItem = new mdtTreeItem;
  // Set data ...
  setupModelData();
}

mdtStringListModel::~mdtStringListModel()
{
  delete pvRootItem;
}

int mdtStringListModel::rowCount(const QModelIndex &parent) const
{
  mdtTreeItem *parentItem;

  // We don't recalc all for a call with column other than first
  if(parent.column() > 0){
    return 0;
  }
  // Get correct parent item
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtTreeItem*>(parent.internalPointer());
  }
  return parentItem->childCount();
}

int mdtStringListModel::columnCount(const QModelIndex &parent) const
{
  mdtTreeItem *parentItem;
  mdtTreeItem *childItem;
  
  // Set the correct parent
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtTreeItem*>(parent.internalPointer());
  }
  
  return 3;
  // Find the child corresponding to the request row
  childItem = parentItem->child(parentItem->row());
  if(childItem == 0){
    return 0;
  }
  
  return childItem->columnCount();
}

QVariant mdtStringListModel::data(const QModelIndex &index, int role) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return QVariant();
  }
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  //cout << "Req data, row: " << index.row() << " , col: " << index.column() << endl;
  mdtTreeItem *item = static_cast<mdtTreeItem*>(index.internalPointer());
  
  
  return item->data(index.column());
}
 
QVariant mdtStringListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  // Return the section with correct title
  if(orientation == Qt::Horizontal){
    return pvRootItem->data(section);
  }else{
    return QString("Row %1").arg(section);
  }
}

Qt::ItemFlags mdtStringListModel::flags(const QModelIndex &index) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable;
}

QModelIndex mdtStringListModel::index(int row, int column, const QModelIndex &parent) const
{
  if(!hasIndex(row, column, parent)){
    return QModelIndex();
  }
  
  // Get the parent item
  mdtTreeItem *parentItem;
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = static_cast<mdtTreeItem*>(parent.internalPointer());
  }
  // Get child of this parent item for request row
  mdtTreeItem *childItem = parentItem->child(row);
  if(childItem == 0){
    return QModelIndex();
  }
  return createIndex(row, column, childItem);
}

QModelIndex mdtStringListModel::parent(const QModelIndex &index) const
{
  if(!index.isValid()){
    return QModelIndex();
  }
  
  mdtTreeItem *childItem = static_cast<mdtTreeItem*>(index.internalPointer());
  mdtTreeItem *parentItem = childItem->parent();

  if(parentItem == pvRootItem){
    return QModelIndex();
  }
  return createIndex(parentItem->row(), 0, parentItem);
}

void mdtStringListModel::setupModelData()
{
/*
  mdtTreeItem *item0, *item1, *item2;
  QList<QVariant> line1, line2, line3;
  
  line1 << "01" << "02" << "03";
  line2 << "11" << "12" << "13";
  line3 << "21" << "22" << "23";

  item0 = new mdtTreeItem(line1, pvRootItem);
  pvRootItem->appendChild(item0);
  
  item1 = new mdtTreeItem(line2, pvRootItem);
  pvRootItem->appendChild(item1);

  item2 = new mdtTreeItem(line3, item1);
  item1->appendChild(item2);
*/

  QStandardItemModel *tbl1 = new QStandardItemModel(2, 3);
  for (int row = 0; row < 2; ++row) {
    for (int column = 0; column < 3; ++column) {
      QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
        tbl1->setItem(row, column, item);
    }
  }
  mdtTreeItem *item, *lastItem;
  // Create a item for each row in data model
  for(int i=0; i<tbl1->rowCount(); i++){
    item = new mdtTreeItem(tbl1, i, pvRootItem);
    pvRootItem->appendChild(item);
  }
  lastItem = item;
  for(int i=0; i<tbl1->rowCount(); i++){
    item = new mdtTreeItem(tbl1, i, lastItem);
    lastItem->appendChild(item);
  }

  
}
