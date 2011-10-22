#ifndef MDT_STRING_LIST_MODEL_H
#define MDT_STRING_LIST_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QStringList>
#include "mdtTreeItem.h"

class mdtStringListModel : public QAbstractItemModel
{
 Q_OBJECT
 
 public:
  
  mdtStringListModel(const QStringList &strings, QObject *parent = 0);
  ~mdtStringListModel();
  
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  
  Qt::ItemFlags flags(const QModelIndex &index) const;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  
  QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
  
 private:
  
  void setupModelData();
   
  mdtTreeItem *pvRootItem;
};

#endif  // #ifndef MDT_STRING_LIST_MODEL_H
