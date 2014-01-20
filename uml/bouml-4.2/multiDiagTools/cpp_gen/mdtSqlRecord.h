#ifndef _MDTSQLRECORD_H
#define _MDTSQLRECORD_H


class mdtSqlRecord : public QSqlRecord
{
  public:
  mdtSqlRecord();

  ~mdtSqlRecord();

  bool addField(const QString & fieldName, const QString & tableName, const QSqlDatabase & db);


  private:
    mdtError pvLastError;


  public:
  mdtError lastError();


  protected:
  mdtError lastErrorW();

};

#endif // #ifndef MDTSQLRECORD_H
