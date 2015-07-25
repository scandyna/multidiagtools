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
#ifndef MDT_CL_UNIT_LINK_MODIFICATION_DIALOG_H
#define MDT_CL_UNIT_LINK_MODIFICATION_DIALOG_H

#include "ui_mdtClUnitLinkModificationDialog.h"
#include "mdtClLinkVersionData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClLinkModificationKeyData.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>
#include <QList>

class QWidget;
class mdtClModificationModel;
class mdtClLinkVersionModel;

/*! \brief Link modification edition dialog
 *
 * \deprecated
 */
class mdtClUnitLinkModificationDialog : public QDialog, Ui::mdtClUnitLinkModificationDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClUnitLinkModificationDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Set link version
   */
  void setLinkVersion(const mdtClLinkVersionData & v);

  /*! \brief Set link modification
   */
  void setLinkModification(const mdtClModificationPkData & m);

  /*! \brief Set link modification
   */
  void setLinkModification(mdtClModification_t m);

  /*! \brief Get selected link modification
   *
   * Will also include selected link version
   *
   * \todo link PK not included !
   */
  mdtClLinkModificationKeyData linkModificationKeyData() const;

 private:

  Q_DISABLE_COPY(mdtClUnitLinkModificationDialog)

  QSqlDatabase pvDatabase;
  mdtClModificationModel *pvLinkModificationModel;
  mdtClLinkVersionModel *pvLinkVersionModel;
};

#endif // #ifndef MDT_CL_UNIT_LINK_MODIFICATION_DIALOG_H
