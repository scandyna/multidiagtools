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
#ifndef MDT_CL_UNIT_CONNECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTION_DIALOG_H

#include "mdtClUnitConnectionData.h"
#include "ui_mdtClUnitConnectionDialog.h"
#include <QDialog>
#include <QSqlDatabase>

class QWidget;

/*! \brief Dialog for edition of a unit connection
 */
class mdtClUnitConnectionDialog : public QDialog, Ui::mdtClUnitConnectionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClUnitConnectionDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnitConnectionDialog();

  /*! \brief Set unit ID
   */
  void setUnitId(const QVariant & id);

  /*! \brief Set article connection ID
   */
  void setArticleConnectionId(const QVariant & id);

  /*! \brief Set data
   *
   * Fields that are realated to article connection,
   *  and thoses that are not null in data, will be enabled.
   */
  void setData(const mdtClUnitConnectionData &data);

  /*! \brief Get data
   */
  const mdtClUnitConnectionData &data() const;

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

  /*! \brief Copy article connector name to unit connector name
   */
  void copyConnectorName();

  /*! \brief Copy article contact name to unit contact name
   */
  void copyContactName();

  /*! \brief Do some check, store values and accept dialog if Ok
   */
  void accept();

  /*! \brief Clear values and reject dialog
   */
  void reject();

 private:

  /*! \brief Hide all widgets related to article connection
   */
  void hideArticleConnectionWidgets();

  /*! \brief Show all widgets related to article connection
   */
  void showArticleConnectionWidgets();

  Q_DISABLE_COPY(mdtClUnitConnectionDialog);

  QSqlDatabase pvDatabase;
  mdtClUnitConnectionData pvData;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_DIALOG_H
