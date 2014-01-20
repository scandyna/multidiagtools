#ifndef _MDTCSVFILE_H
#define _MDTCSVFILE_H


class mdtCsvFile : public QFile {
  public:
    mdtCsvFile(const QObject & parent);

    ~mdtCsvFile();


  private:
    QList   [Qt4]<QStringList> pvLines;


  public:
    bool readLines(const QByteArray & separator, const QByteArray & protection);

    void clear();

    QString valueAt(int line, int column);

    QList<QStringList> lines();

};
#endif
