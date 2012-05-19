#ifndef _MDTDATAWIDGETMAPPER_H
#define _MDTDATAWIDGETMAPPER_H


#include "mdtSqlTableModel.h"

class mdtDataWidgetMapper : public QDataWidgetMapper {
  public:
    mdtDataWidgetMapper(const QObject & parent);

    ~mdtDataWidgetMapper();

    void setModel(const mdtSqlTableModel & model);

    mdtSqlTableModel model();

    void addMapping(const QWidget & widget, int section);

    void addMapping(const QWidget & widget, int section, const QByteArray & propertyName);


  private:
    mdtSqlTableModel * pvSqlTableModel;

    void setModel(const QAbstractItemModel & model);

};
#endif
