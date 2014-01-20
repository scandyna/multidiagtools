#ifndef _MDTCLBASE_H
#define _MDTCLBASE_H


class mdtClBase
{
  private:
    QSqlDatabase * pvDatabase;


  protected:
    mdtError pvLastError;


  public:
  mdtClBase(const QSqlDatabase & db);

  virtual ~mdtClBase();

  QSqlDatabase database();

  mdtError lastError();


  protected:
  bool beginTransaction();

  bool rollbackTransaction();

  bool commitTransaction();

};

#endif // #ifndef MDTCLBASE_H
