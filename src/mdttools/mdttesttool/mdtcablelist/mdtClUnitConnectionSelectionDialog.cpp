/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtClUnitConnectionSelectionDialog.h"

mdtClUnitConnectionSelectionDialog::mdtClUnitConnectionSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(700, 400);
  setWindowTitle(tr("Unit connection selection"));
}

bool mdtClUnitConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & unitId, mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, bool allowMultiSelection)
{
  QString sql;

  return select(db, unitId, QList<mdtClUnitConnectorPkData>(), lu, allowMultiSelection);
}

bool mdtClUnitConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & unitId, QList<mdtClUnitConnectorPkData> ucnrPkList, mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, bool allowMultiSelection)
{
  QString sql;
  int lastIndex;
  int i;

  sql = baseSqlStatement();
  sql += " WHERE Unit_Id_FK = " + unitId.toString() +  " ";
  sql += linkUsageClause(lu, unitId);
  if(!ucnrPkList.isEmpty()){
    sql += " AND UnitConnector_Id_FK IN(";
    lastIndex = ucnrPkList.size() - 1;
    for(i = 0; i < lastIndex; ++i){
      sql += ucnrPkList.at(i).id.toString();
    }
    sql += ucnrPkList.at(lastIndex).id.toString() + ")";
  }

  return setQuery(sql, db, allowMultiSelection);
}

QString mdtClUnitConnectionSelectionDialog::baseSqlStatement() const
{
  return "SELECT * FROM UnitConnection_view";
}

QString mdtClUnitConnectionSelectionDialog::linkUsageClause(mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, const QVariant &unitId) const
{
  QString clause;

  switch(lu){
    case LinkUsage_t::All:
      return clause;
    case LinkUsage_t::NotUsedAsConnectionStart:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionStart_Id_FK FROM UnitLink_view WHERE StartUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)";
      break;
    case LinkUsage_t::NotUsedAsConnectionEnd:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionEnd_Id_FK FROM UnitLink_view WHERE EndUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionEnd_Id_FK IS NOT NULL)";
      break;
    case LinkUsage_t::NotUsed:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionStart_Id_FK FROM UnitLink_view WHERE StartUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)" \
             + "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionEnd_Id_FK FROM UnitLink_view WHERE EndUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)";
      break;
  }

  return clause;
}
