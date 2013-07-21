#ifndef _MDTGRAPHEDGEDATA_H
#define _MDTGRAPHEDGEDATA_H


class mdtGraphEdgeData {
  private:
    QMap<QString, QVariant> pvData;


  public:
    mdtGraphEdgeData();

    ~mdtGraphEdgeData();

    void setData(const QVariant & data, const QString & field);

    QVariant data(const QString & field);

};
#endif
