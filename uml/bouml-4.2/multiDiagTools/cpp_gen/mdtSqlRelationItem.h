#ifndef _MDTSQLRELATIONITEM_H
#define _MDTSQLRELATIONITEM_H


class mdtSqlRelationItem {
  public:
    mdtSqlRelationItem();

    ~mdtSqlRelationItem();


  private:
    QSqlField pvParentField;

    int pvParentFieldIndex;

    QSqlField pvChildField;

    int pvChildFieldIndex;


  public:
    void setParentField(const QSqlField & field);

    void setParentFieldIndex(int index);

    void setChildField(const QSqlField & field);

    void setChildFieldIndex(int index);

    QString parentFieldName() const;

    int parentFieldIndex() const;

    QString childFieldName() const;

    int childFieldIndex() const;


  private:
    QString pvDataProtection;


  public:
    QString dataProtection() const;

};
#endif
