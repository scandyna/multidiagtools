#ifndef _MDTSQLTABLESELECTIONITEM_H
#define _MDTSQLTABLESELECTIONITEM_H


class mdtSqlTableSelectionItem
{
  private:
    QModelIndex pvIndex;

    QString pvFieldName;


  public:
  mdtSqlTableSelectionItem(const QModelIndex & index, const QString & fieldName);

 inline  QModelIndex index() const;

  inline QString fieldName() const;

};
inline QModelIndex mdtSqlTableSelectionItem::index() const 
{
}

inline QString mdtSqlTableSelectionItem::fieldName() const 
{
}


#endif // #ifndef MDTSQLTABLESELECTIONITEM_H
