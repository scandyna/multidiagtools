#ifndef _MDTCLLINKBEAMEDITOR_H
#define _MDTCLLINKBEAMEDITOR_H


class mdtClLinkBeamEditor : public mdtSqlForm
{
  public:
  mdtClLinkBeamEditor(const QWidget & parent, const QSqlDatabase & db);

  bool setupTables();


  private:
  bool setupLinkBeamTable();

  bool setupLinkTable();

  bool setupStartUnitTable();

  bool setupEndUnitTable();

};

#endif // #ifndef MDTCLLINKBEAMEDITOR_H
