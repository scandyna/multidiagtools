#ifndef _MDTTTTESTEDITOR_H
#define _MDTTTTESTEDITOR_H


#include "mdtClEditor.h"

class mdtTtTestEditor : public mdtClEditor
{
  public:
  mdtTtTestEditor(const QObject & parent, const QSqlDatabase & db);

  void addTestItem();

  void removeTestItem();


  private:
  virtual bool setupTables();

  bool setupTestTable();

  bool setupTestLinkTable();

  bool setupTestNodeUnitSetupTable();

};

#endif // #ifndef MDTTTTESTEDITOR_H
