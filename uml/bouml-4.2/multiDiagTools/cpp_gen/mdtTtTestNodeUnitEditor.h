#ifndef _MDTTTTESTNODEUNITEDITOR_H
#define _MDTTTTESTNODEUNITEDITOR_H


class mdtTtTestNodeUnitEditor : public mdtSqlForm, public Ui::mdtTtTestNodeUnitEditor
{
  public:
  mdtTtTestNodeUnitEditor(const QWidget & parent, const QSqlDatabase & db);

  virtual bool setupTables();


  private:
  bool setupTestNodeUnitTable();

};

#endif // #ifndef MDTTTTESTNODEUNITEDITOR_H
