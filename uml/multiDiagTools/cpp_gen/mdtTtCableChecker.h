#ifndef _MDTTTCABLECHECKER_H
#define _MDTTTCABLECHECKER_H


class mdtTtTest;
class mdtTtTestResult;
class mdtDeviceModbusWago;

class mdtTtCableChecker : public mdtSqlForm
{
  public:
  mdtTtCableChecker(const QWidget & parent, const QSqlDatabase & db);

  virtual bool setupTables();


  private:
  bool setupTestResultTable();

  bool setupTestResultItemTable();

};

#endif // #ifndef MDTTTCABLECHECKER_H
