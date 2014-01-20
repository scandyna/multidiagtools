#ifndef _MDTCLCONNECTOREDITOR_H
#define _MDTCLCONNECTOREDITOR_H


#include "mdtClEditor.h"

class mdtClConnectorEditor : public mdtClEditor
{
  public:
  mdtClConnectorEditor(const QObject & parent, const QSqlDatabase & db);

  ~mdtClConnectorEditor();


  protected:
  virtual bool setupTables();

};

#endif // #ifndef MDTCLCONNECTOREDITOR_H
