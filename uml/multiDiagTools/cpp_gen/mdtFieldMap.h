#ifndef _MDTFIELDMAP_H
#define _MDTFIELDMAP_H


#include "mdtFieldMapItem.h"

class mdtFieldMap {
  public:
    mdtFieldMap();

     ~mdtFieldMap();


  private:
    <mdtFieldMapItem> pvItems;


  public:
    void addItem(const mdtFieldMapItem & item);

    void clear();

    mdtFieldMapItem itemAtFieldIndex(int index);

    mdtFieldMapItem itemAtFieldName(const QString & name);

    mdtFieldMapItem itemAtDisplayText(const QString & text);

    mdtFieldMapItem itemAtSourceFieldIndex(int index);

    mdtFieldMapItem itemAtSourceFieldName(const QString & name);

    mdtFieldMapItem itemAtSourceFieldDisplayText(const QString & text);

    QVariant dataForFieldIndex(const QStringList & sourceData, const  & );

    QVariant dataForFieldName(const QStringList & sourceData, const  & );

    QVariant dataForDisplayText(const QStringList & sourceData, const  & );

    QString dataForSourceFieldIndex(const QList<QVariant> & data);

    QString dataForSourceFieldName(const QList<QVariant> & data);

    QString dataForSourceFieldDisplayText(const QList<QVariant> & data);

};
#endif
