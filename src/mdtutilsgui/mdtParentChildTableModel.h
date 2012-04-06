#ifndef MDT_PARENT_CHILD_TABLE_MODEL_H
#define MDT_PARENT_CHILD_TABLE_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
//#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QVariant>
#include "mdtParentChildTableItem.h"

class mdtParentChildTableModel : public QAbstractItemModel
{
 Q_OBJECT
 
 public:
  
  mdtParentChildTableModel(QObject *parent = 0);
  ~mdtParentChildTableModel();
  
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role) const;

  void setHeaderData(QList<QVariant> &headerData);
  
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  
  Qt::ItemFlags flags(const QModelIndex &index) const;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  
  QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
  
 private:
  
  void setupModelData();
   
  mdtParentChildTableItem *pvRootItem;
  QList<QVariant> pvHeaderData;
};

#endif  // #ifndef MDT_PARENT_CHILD_TABLE_MODEL_H
