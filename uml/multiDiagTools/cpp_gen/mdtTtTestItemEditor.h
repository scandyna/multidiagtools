#ifndef _MDTTTTESTITEMEDITOR_H
#define _MDTTTTESTITEMEDITOR_H


#include "mdtClEditor.h"

class mdtTtTestItemEditor : public mdtClEditor
{
  public:
  mdtTtTestItemEditor(const QObject & parent, const QSqlDatabase & db);


  private:
  virtual bool setupTables();

  bool setupTestItemTable();

  bool setupTestLinkTable();

  bool setupTestNodeUnitSetupTable();

};

#endif // #ifndef MDTTTTESTITEMEDITOR_H
