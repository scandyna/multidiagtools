#ifndef _MDTSQLRECORD_H
#define _MDTSQLRECORD_H


class mdtSqlRecord
{
  public:
  mdtSqlRecord();

  ~mdtSqlRecord();

  bool addField(const QString & fieldName, const QString & tableName, const QSqlDatabase & db);


  protected:
    mdtError pvLastError;


  public:
  mdtError lastError();


  private:
    QSqlRecord pvRecord;

};

#endif // #ifndef MDTSQLRECORD_H
