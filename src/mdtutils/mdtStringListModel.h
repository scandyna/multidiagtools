#ifndef MDT_STRING_LIST_MODEL_H
#define MDT_STRING_LIST_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QStringList>

class mdtStringListModel : public QAbstractListModel
{
 Q_OBJECT
 
 public:
  
  mdtStringListModel(const QStringList &strings, QObject *parent = 0);
  
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role) const;
  
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  
  Qt::ItemFlags flags(const QModelIndex &index) const;
  
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  
  bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
  
  bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
  
 private:
  
  QStringList pvStringList;
};

#endif  // #ifndef MDT_STRING_LIST_MODEL_H
