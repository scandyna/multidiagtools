#ifndef _MDTSQLRELATION_H
#define _MDTSQLRELATION_H


class mdtSqlRelationItem;

class mdtSqlRelation : public QObject {
  public:
    mdtSqlRelation(const QObject & parent);

    ~mdtSqlRelation();


  private:
    <mdtSqlRelationItem> pvRelations;


  public:
    void setParentModel(const QSqlTableModel & model);

    void setChildModel(const QSqlTableModel & model);

    void addRelation(const QString & parentFieldName, const QString & childFieldName);

    void clear();


  private:
    QSqlTableModel pvParentModel;

    QSqlTableModel pvChildModel;


  public:
    void setParentCurrentIndex(int index);

    void setParentCurrentIndex(const QModelIndex & index);

    void setParentCurrentIndex(const QModelIndex & current, const QModelIndex & previous);


  private:
    QString pvChildModelUserFilter;

    QString pvChildModelRelationFilter;

    QString pvChildModelFilter;

    void generateChildModelRelationFilter();

    void generateChildModelFilter();

};
#endif
