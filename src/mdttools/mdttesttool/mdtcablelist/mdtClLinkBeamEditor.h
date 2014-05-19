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
#ifndef MDT_CL_LINK_BEAM_EDITOR_H
#define MDT_CL_LINK_BEAM_EDITOR_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>

class mdtClLinkBeamEditor;

/*! \brief Provides link beam edition to the user
 */
class mdtClLinkBeamEditor : public mdtSqlForm
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkBeamEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Add a start unit
   */
  void addStartUnit();

  /*! \brief Remove start units
   */
  void removeStartUnits();

  /*! \brief Add a end unit
   */
  void addEndUnit();

  /*! \brief Remove end units
   */
  void removeEndUnits();

 private:

  /*! \brief Setup LinkBeam_tbl part
   */
  bool setupLinkBeamTable();

  /*! \brief Setup UnitLink_view part
   */
  bool setupLinkTable();

  /*! \brief Setup LinkBeam_UnitStart_view part
   */
  bool setupStartUnitTable();

  /*! \brief Setup LinkBeam_UnitEnd_view part
   */
  bool setupEndUnitTable();

  Q_DISABLE_COPY(mdtClLinkBeamEditor);
};

#endif // #ifndef MDT_CL_LINK_BEAM_EDITOR_H
