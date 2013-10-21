/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_PATH_GRAPH_DIALOG_H
#define MDT_CL_PATH_GRAPH_DIALOG_H

#include "ui_mdtClPathGraphDialog.h"
#include <QDialog>
#include <QWidget>
#include <QSqlDatabase>

class mdtClPathGraph;

/*! \brief Display a path from link list starting at a specified connection
 */
class mdtClPathGraphDialog : public QDialog, Ui::mdtClPathGraphDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClPathGraphDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClPathGraphDialog();

  /*! \brief Load the link list
   */
  bool loadLinkList();

  /*! \brief Draw the graphic representation starting from a connection
   */
  bool drawPath(const QVariant & fromConnectionId);

 private:

  Q_DISABLE_COPY(mdtClPathGraphDialog);

  mdtClPathGraph *pvPathGraph;
};

#endif // #ifndef MDT_CL_PATH_GRAPH_DIALOG_H
