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
#ifndef MDT_CL_UNIT_LINK_DIALOG_H
#define MDT_CL_UNIT_LINK_DIALOG_H

#include "ui_mdtClUnitLinkDialog.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClLinkVersionData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClLinkData.h"
#include "mdtError.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>
#include <QList>

class QWidget;
class QSqlQueryModel;
class QComboBox;
class mdtClModificationModel;
class mdtClLinkVersionModel;
class mdtClLinkTypeModel;
class mdtClLinkDirectionModel;
class mdtClVehicleTypeLinkAssignationWidget;

/// \todo Update once migrated
namespace Mdt{ namespace CableList{
  class LinkTypeModel;
}}

/*! \brief Dialog that let the user edit a unit link
 */
class mdtClUnitLinkDialog : public QDialog, Ui::mdtClUnitLinkDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClUnitLinkDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnitLinkDialog();

  /*! \brief Set start unit
   *
   * This will also clear start connection and set start vehicle types.
   */
  void setStartUnit(const QVariant &unitId);

  /*! \brief Limit start unit selection to given list
   */
  void setStartUnitSelectionList(const QList<QVariant> & idList);

  /*! \brief Clear start unit selection list
   */
  void clearStartUnitSelectionList();

  /*! \brief Limit start connection selection to given connectors
   */
  ///void setStartConnectorLimitIdList(const QList<QVariant> & unitConnectorIdList);

  /*! \brief Change start connection label
   */
  void setStartConnectionLabel(const QString & labelText);

  /*! \brief Set end unit
   *
   * This will also clear end connection and set end vehicle types.
   */
  void setEndUnit(const QVariant &unitId);

  /*! \brief Limit end unit selection to given list
   */
  void setEndUnitSelectionList(const QList<QVariant> & idList);

  /*! \brief Limit end connection selection to given connectors
   */
  ///void setEndConnectorLimitIdList(const QList<QVariant> & unitConnectorIdList);

  /*! \brief Clear end unit selection list
   */
  void clearEndUnitSelectionList();

  /*! \brief Change end connection label
   */
  void setEndConnectionLabel(const QString & labelText);

  /*! \brief Set show only unused start connections
   */
  void setShowOnlyUnusedStartConnection(bool onlyUnused);

  /*! \brief Set show only unused end connections
   */
  void setShowOnlyUnusedEndConnection(bool onlyUnused);

  /*! \brief Set the vehicle type ID list on witch we are working on
   *
   * This will limit the unit selections to those that exists for given vehicle types.
   */
  void setWorkingOnVehicleTypeIdList(const QList<QVariant> & vtIdList);

  /*! \brief Set the vehicle type ID on witch we are working on
   *
   * This will limit the unit selections to those that exists for given vehicle type.
   */
  void setWorkingOnVehicleTypeId(const QVariant & vtId)
  {
    QList<QVariant> lst;
    lst.append(vtId);
    setWorkingOnVehicleTypeIdList(lst);
  }

  /*! \brief Clear the working on vehicle type list
   *
   * \sa setWorkingOnVehicleTypeIdList()
   */
  void clearWorkingOnVehicleTypeList();

  /*! \brief Get selected vehicle type assignations
   *
   * \pre Only call this function if vehicle type assignation is enabled
   */
  QList<mdtClVehicleTypeStartEndKeyData> selectedVehicleTypeList() const;

  /*! \brief Set link type
   */
  void setLinkType(LinkType t);

  /*! \brief Get link type key data
   */
  LinkTypePk linkTypePk() const;

  /*! \brief Set link direction
   */
  void setLinkDirection(mdtClLinkDirection_t d);

  /*! \brief Get selected link direction
   */
  mdtClLinkDirectionKeyData linkDirectionKeyData() const;

  /*! \brief Set link version
   */
  void setLinkVersion(const mdtClLinkVersionData & v);

  /*! \brief Set link modification
   */
  void setLinkModification(ModificationPk m);

  /*! \brief Set link modification
   */
  void setLinkModification(ModificationType m);

  /*! \brief Get selected link modification
   *
   * Will also include selected link version
   */
//   mdtClLinkModificationKeyData linkModificationKeyData() const;

  /*! \brief Set link data
   */
  void setLinkData(mdtClLinkData &data);

  /*! \brief Get link data
   */
  mdtClLinkData linkData();

 private slots:

  /*! \brief Update some things when link type combobox index changed
   */
  void onCbLinkTypeCurrentIndexChanged(int row);

  /*! \brief Update some things when link direction combobox index changed
   */
  void onCbLinkDirectionCurrentIndexChanged(int row);

  /*! \brief Select wire
   */
  void selectWire();

  /*! \brief Set wire to Null
   */
  void clearWire();

  /*! \brief Update link resistance (called when link length changed)
   */
  void setLinkResistance(double linkLength, bool linkLengthIsValid);

  /*! \brief Select the start unit
   */
  void selectStartUnit();

  /*! \brief Select the end unit
   */
  void selectEndUnit();

  /*! \brief Select the start connection
   */
  void selectStartConnection();

  /*! \brief Select the end connection
   */
  void selectEndConnection();

  /*! \brief Do some check, store values and accept dialog if Ok
   */
  void accept();

 private:

  /*! \brief Update wire data
   */
  void updateWire(const QVariant & wireId);

  /*! \brief Calculate and update link reistance
   */
  void updateLinkResistance(const QVariant & linkLength, const QVariant & lineicResistance);

  /*! \brief Update start unit
   */
  void updateStartUnit();

  /*! \brief Update end unit
   */
  void updateEndUnit();

  /*! \brief Update start connection
   */
  void updateStartConnection();

  /*! \brief Update end connection
   */
  void updateEndConnection();

  /*! \brief Update vehicle type assignations
   */
  void updateVehicleTypeAssignations(bool rebuildVehicleTypeList);

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtClUnitLinkDialog)

  QSqlDatabase pvDatabase;
  QVariant pvStartUnitId;
  QList<QVariant> pvStartUnitSelectionIdList;
  QList<mdtClUnitConnectorPkData> pvStartConnectorLimitIdList;  // If not empty, start connection selection will be limited to these connectors
  QVariant pvEndUnitId;
  QList<QVariant> pvEndUnitSelectionIdList;
  QList<mdtClUnitConnectorPkData> pvEndConnectorLimitIdList;    // If not empty, end connection selection will be limited to these connectors
  mdtClLinkData pvLinkData;
  QList<QVariant> pvWorkingOnVehicleTypeIdList;
  mdtClModificationModel *pvModificationModel;
  mdtClLinkVersionModel *pvLinkVersionModel;
  Mdt::CableList::LinkTypeModel *pvLinkTypeModel; /// \todo update once migrated
  mdtClLinkDirectionModel *pvLinkDirectionModel;
  mdtClVehicleTypeLinkAssignationWidget *pvVehicleTypeAssignationWidget;
};

#endif // #ifndef MDT_CL_UNIT_LINK_DIALOG_H
