/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_WIRE_EDITOR_H
#define MDT_CL_WIRE_EDITOR_H

#include <QSqlDatabase>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include "mdtSqlForm.h"

class QWidget;

/*! \brief Wire database editor
 */
class mdtClWireEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Contruct a wire editor
   */
  mdtClWireEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private:

  /*! \brief Setup Wire_tbl
   */
  bool setupWireTable();

  Q_DISABLE_COPY(mdtClWireEditor);
};

#endif // #ifndel MDT_CL_WIRE_EDITOR_H
