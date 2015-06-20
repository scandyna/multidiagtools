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
#ifndef MDT_CL_UNIT_CONNECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTION_DIALOG_H

#include "mdtError.h"
#include "mdtClUnitConnectionData.h"
#include "ui_mdtClUnitConnectionDialog.h"
#include "mdtClConnectionTypeData.h"
#include <QDialog>
#include <QSqlDatabase>

class QWidget;
class mdtClConnectionTypeModel;

/*! \brief Dialog for edition of a unit connection
 */
class mdtClUnitConnectionDialog : public QDialog, Ui::mdtClUnitConnectionDialog
{
 Q_OBJECT

 public:

  /*! \brief Mode
   */
  enum mode_t {
                Add,  /*!< In this mode, user can select connection from connector */
                Edit  /*!< In this mode, user cannot change connection, only edit other data */
              };

  /*! \brief Constructor
   */
  mdtClUnitConnectionDialog(QWidget *parent, QSqlDatabase db, mode_t mode);

  /*! \brief Destructor
   */
  ~mdtClUnitConnectionDialog();

  /*! \brief Set data
   *
   * Fields that are realated to article connection,
   *  and thoses that are not null in data, will be enabled.
   *
   * \pre data's key must not be totally null (its unitId must at least be set)
   */
  void setData(const mdtClUnitConnectionData & data, const QVariant & baseArticleId);

  /*! \brief Get data
   */
  mdtClUnitConnectionData data() const
  {
    return pvData;
  }

 private slots:

  /*! \brief Copy article connection function EN to unit connection function EN
   */
  void copyFunctionEN();

  /*! \brief Copy article connection function FR to unit connection function FR
   */
  void copyFunctionFR();

  /*! \brief Copy article connection function DE to unit connection function DE
   */
  void copyFunctionDE();

  /*! \brief Copy article connection function IT to unit connection function IT
   */
  void copyFunctionIT();

  /*! \brief Select unit connector
   */
  void selectUnitConnector();

  /*! \brief Set unit connection based on no connector
   */
  void setNoConnector();

  /*! \brief Select article connection or base connector connection
   *
   * Depending on what is based unit connector, this method will do:
   *  - If no unit connector was set:
   *     List article connections related to current unit, and based on no article connector,
   *     let the user choose one and copy its contact name.
   *  - If unit connector is based on a article connector:
   *     List article connections based on article connector,
   *     let the user choose one and copy its contact name.
   *  - If unit connector is based on a connector (but not on a article connector):
   *     Let the user select a contact (from ConnectorContact_tbl) to use, and copy its contact name
   */
  void selectConnection();

  /*! \brief Copy article contact name to unit contact name
   */
  void copyContactName();

  /*! \brief Copy article resistance to unit resistance
   */
  void copyResistance();

  /*! \brief Do some check, store values and accept dialog if Ok
   */
  void accept();

  /*! \brief Clear values and reject dialog
   */
  void reject();

 private:

  /*! \brief Set connection type combobox current value
   */
  void setCurrentConnectionType(mdtClConnectionType_t t);

  /*! \brief Set unit connection from a article connection
   */
  void setConnectionFromArticleConnection();

  /*! \brief Set unit connection based on a connector contact
   *
   * Will list connector contacts based on actual connector,
   *  let the user select one and set unit connection based on it.
   */
  void setConnectionFromConnectorContact();

  /*! \brief Get connector data from database and update widgets
   */
  void updateConnectorData();

  /*! \brief Update dialog's widgets from data
   */
  void updateDialog();

  /*! \brief Update data from dialog's widgets
   */
  void updateData();

  /*! \brief Check if all required data are set
   */
  bool checkData();

  /*! \brief Hide all widgets related to article connection
   */
  void hideArticleConnectionWidgets();

  /*! \brief Show all widgets related to article connection
   */
  void showArticleConnectionWidgets();

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  /*! \brief Check if its possible to select a connection
   */
  bool canSelectConnection() const;

  /*! \brief Check if its possible to select connection type
   */
  bool canSelectConnectionType() const;

  Q_DISABLE_COPY(mdtClUnitConnectionDialog);

  QSqlDatabase pvDatabase;
  mdtClUnitConnectionData pvData;
  QVariant pvArticleConnectionResistance;
  QVariant pvBaseArticleId;       // Used to list article related connections
  mode_t pvMode;
  mdtClConnectionTypeModel *pvConnectionTypeModel;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_DIALOG_H
