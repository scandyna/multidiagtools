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
#ifndef MDT_CL_UNIT_LINK_DIALOG_H
#define MDT_CL_UNIT_LINK_DIALOG_H

#include "ui_mdtClUnitLinkDialog.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClLinkData.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>
#include <QList>

class QWidget;
class QSqlQueryModel;
class QComboBox;

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
   * This variant will get data from unit table and calls setStartUnit(const QVariant&, const QVariant&, const QVariant&) .
   */
  void setStartUnit(const QVariant &unitId);

  /*! \brief Set start unit
   *
   * This will also clear start connection and set start vehicle types.
   */
  void setStartUnit(const QVariant &unitId, const QVariant &schemaPosition, const QVariant &cabinet);

  /*! \brief Get selected start unit ID
   */
  QVariant startUnitId() const;

  /*! \brief Set end unit
   *
   * This variant will get data from unit table and calls setEndUnit(const QVariant&, const QVariant&, const QVariant&) .
   */
  void setEndUnit(const QVariant &unitId);

  /*! \brief Set end unit
   *
   * This will also clear end connection and set end vehicle types.
   */
  void setEndUnit(const QVariant &unitId, const QVariant &schemaPosition, const QVariant &cabinet);

  /*! \brief Get selected end unit ID
   */
  QVariant endUnitId() const;

  /*! \brief Set the start connection data
   */
  void setStartConnectionData(const mdtClUnitConnectionData &data);

  /*! \brief Get selected start connection data
   */
  mdtClUnitConnectionData startConnectionData() const;

  /*! \brief Set the end connection data
   */
  void setEndConnectionData(const mdtClUnitConnectionData &data);

  /*! \brief Get selected end connection data
   */
  mdtClUnitConnectionData endConnectionData() const;

  /*! \brief Set start vehicle types
   */
  void setStartVehicleTypes(const QVariant &unitId);

  /*! \brief Get selected start vehicle types
   */
  const QList<QVariant> startVehicleTypeIdList() const;

  /*! \brief Set end vehicle types
   */
  void setEndVehicleTypes(const QVariant &unitId);

  /*! \brief Get selected end vehicle types
   */
  const QList<QVariant> endVehicleTypeIdList() const;

  /*! \brief Display link type in combobox
   */
  void setLinkTypeCode(const QVariant & code);

  /*! \brief Get selected link type code
   */
  QVariant linkTypeCode() const;

  /*! \brief Display link direction in combobox
   */
  void setLinkDirectionCode(const QVariant & code);

  /*! \brief Get selected link direction code
   */
  QVariant linkDirectionCode() const;

  /*! \brief Get link data
   */
  mdtClLinkData &linkData();

 private slots:

  /*! \brief Update some things when link type combobox index changed
   */
  void onCbLinkTypeCurrentIndexChanged(int row);

  /*! \brief Update some things when link direction combobox index changed
   */
  void onCbLinkDirectionCurrentIndexChanged(int row);

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

  /*! \brief Select the start vehicle types
   */
  void selectStartVehicleTypes();

  /*! \brief Select the end vehicle types
   */
  void selectEndVehicleTypes();

 private:

  Q_DISABLE_COPY(mdtClUnitLinkDialog);

  QSqlDatabase pvDatabase;
  QSqlQueryModel *pvLinkTypeModel;
  QSqlQueryModel *pvLinkDirectionModel;
  mdtClLinkData pvLinkData;
  QVariant pvStartUnitId;
  QVariant pvEndUnitId;
  mdtClUnitConnectionData pvStartConnectionData;
  mdtClUnitConnectionData pvEndConnectionData;
  QSqlQueryModel *pvStartVehicleTypesModel;
  QSqlQueryModel *pvEndVehicleTypesModel;
};

#endif // #ifndef MDT_CL_UNIT_LINK_DIALOG_H
