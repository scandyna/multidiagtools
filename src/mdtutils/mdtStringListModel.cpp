#include "mdtStringListModel.h"

#include <QVariant>
#include <QString>

mdtStringListModel::mdtStringListModel(const QStringList &strings, QObject *parent)
 : QAbstractListModel(parent)
{
  pvStringList = strings;
}
  
int mdtStringListModel::rowCount(const QModelIndex &parent) const
{
  return pvStringList.count();
}

QVariant mdtStringListModel::data(const QModelIndex &index, int role) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return QVariant();
  }
  if(index.row() >= pvStringList.size()){
    return QVariant();
  }
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  return pvStringList.at(index.row());  
}
 
QVariant mdtStringListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  // Display role
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  // Return the section with correct title
  if(orientation == Qt::Horizontal){
    return QString("Column %1").arg(section);
  }else{
    return QString("Row %1").arg(section);
  }
}

Qt::ItemFlags mdtStringListModel::flags(const QModelIndex &index) const
{
  // Case of invalid indexes
  if(!index.isValid()){
    return Qt::ItemIsEnabled;
  }
  // Tell the delegate that the item is editable (+ standard flags..)
  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool mdtStringListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Case of invalid indexes
  if(!index.isValid()){
    return false;
  }
  // Role
  if(role != Qt::EditRole){
    return false;
  }
  // Store new value
  pvStringList.replace(index.row(), value.toString());
  // Rfresh all views
  emit dataChanged(index, index);
  return true;
}

bool mdtStringListModel::insertRows(int position, int rows, const QModelIndex &index)
{
  beginInsertRows(QModelIndex(), position, position+rows-1);
  
  for(int row=0; row < rows; ++row){
    pvStringList.insert(position, "");
  }
  
  endInsertRows();
  return true;
}

bool mdtStringListModel::removeRows(int position, int rows, const QModelIndex &index)
{
  beginRemoveRows(QModelIndex(), position, position+rows-1);

  for(int row=0; row < rows; ++row){
    pvStringList.removeAt(position);
  }

  endRemoveRows();
  return true;
}
