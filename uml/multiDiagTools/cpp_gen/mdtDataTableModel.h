#ifndef _MDTDATATABLEMODEL_H
#define _MDTDATATABLEMODEL_H


class mdtDataTableModel : public QSqlTableModel {
  public:
    mdtDataTableModel(const QObject & parent, const QSqlDatabase & db);

    ~mdtDataTableModel();


  private:
    QString pvDataSetName;

    QDir pvDataSetDirectory;


  public:
    bool createDataSet(const QString & name, const QStringList & fields, const create_mode_t & mode);

    bool addRow(const QMap<QString,QVariant> & data,  );

    bool setData(int row, int column, const QVariant & value, const QT::EditRole & role);

    bool setData(int row, const QString & field, const QVariant & value, const QT::EditRole & role);

    bool exportToCsvFile(const QString & filePath);

    bool importFromCsvFile(const QString & filePath);

};
#endif
