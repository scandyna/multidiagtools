#ifndef _MDTITEMSSELECTORDIALOGITEM_H
#define _MDTITEMSSELECTORDIALOGITEM_H


class mdtItemsSelectorDialogItem
{
  private:
    QString pvText;

    int pvColumnIndex;

    QString pvFieldName;

    Qt::SortOrder pvSortOrder;


  public:
  inline const QString Text() const;

  void setText(QString value);

  inline const int ColumnIndex() const;

  void setColumnIndex(int value);

  inline const QString FieldName() const;

  void setFieldName(QString value);

  inline const Qt::SortOrder SortOrder() const;

  void setSortOrder(Qt::SortOrder value);

};
inline const QString mdtItemsSelectorDialogItem::Text() const 
{
  return pvText;
}

inline const int mdtItemsSelectorDialogItem::ColumnIndex() const 
{
  return pvColumnIndex;
}

inline const QString mdtItemsSelectorDialogItem::FieldName() const 
{
  return pvFieldName;
}

inline const Qt::SortOrder mdtItemsSelectorDialogItem::SortOrder() const 
{
  return pvSortOrder;
}


#endif // #ifndef MDTITEMSSELECTORDIALOGITEM_H
