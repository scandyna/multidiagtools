#ifndef _MDTTTTESTNODEEDITOR_H
#define _MDTTTTESTNODEEDITOR_H


#include "mdtClEditor.h"

class mdtTtTestNodeEditor : public mdtClEditor
{
  public:
  mdtTtTestNodeEditor(const QObject & parent, const QSqlDatabase & db);


  private:
  virtual bool setupTables();

};

#endif // #ifndef MDTTTTESTNODEEDITOR_H
