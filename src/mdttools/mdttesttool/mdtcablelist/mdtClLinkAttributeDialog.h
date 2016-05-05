/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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

#include "ui_mdtClLinkAttributeDialog.h"
#include "mdtClLinkVersionData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtError.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>
#include <QList>

class QWidget;
class mdtClModificationModel;
class mdtClLinkVersionModel;
class mdtClVehicleTypeLinkAssignationWidget;

/*! \brief Let user select versionning and vehicle assignations
 */
class mdtClLinkAttributeDialog : public QDialog, Ui::mdtClLinkAttributeDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkAttributeDialog(QWidget *parent, QSqlDatabase db, bool enableVehicleTypeAssignations);

  /*! \brief Set message of dialog
   */
  void setMessage(const QString & msg);

  /*! \brief Build list of vehicle types related to given unit ID
   *
   * \pre Use this function only if vehicle type assignatins was enabled
   */
  bool buildVehicleTypeList(const QVariant & unitId);

  /*! \brief Get list of selected vehicle types
   *
   * \pre Use this function only if vehicle type assignatins was enabled
   */
  QList<mdtClVehicleTypeStartEndKeyData> selectedVehicleTypeList() const;

  /*! \brief Set link version
   */
  void setLinkVersion(const mdtClLinkVersionData & v);

  /*! \brief Get selected link version
   */
  LinkVersionPk linkVersionPkData() const;

  /*! \brief Set link modification
   */
  void setModification(ModificationPk m);

  /*! \brief Set link modification
   */
  void setModification(ModificationType m);

  /*! \brief Get selected link modification
   */
  ModificationPk modificationKeyData() const;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClLinkAttributeDialog)

  QSqlDatabase pvDatabase;
  mdtClModificationModel *pvModificationModel;
  mdtClLinkVersionModel *pvLinkVersionModel;
  mdtClVehicleTypeLinkAssignationWidget *pvVehicleTypeAssignationWidget;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_UNIT_LINK_MODIFICATION_DIALOG_H
