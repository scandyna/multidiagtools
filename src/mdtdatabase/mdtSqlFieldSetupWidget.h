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
#ifndef MDT_SQL_FIELD_SETUP_WIDGET_H
#define MDT_SQL_FIELD_SETUP_WIDGET_H

#include "ui_mdtSqlFieldSetupWidget.h"
#include "mdtSqlFieldSetupData.h"
#include "mdtError.h"
#include <QWidget>
#include <QSqlDatabase>

class mdtSqlFieldSetupTableModel;

/*! \brief Field setup widget
 */
class mdtSqlFieldSetupWidget : public QWidget, Ui::mdtSqlFieldSetupWidget
{
 public:

  /*! \brief Constructor
   */
  mdtSqlFieldSetupWidget(QWidget *parent);

  /*! \brief Set setup data
   */
  bool setData(const mdtSqlFieldSetupData & data, QSqlDatabase db);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Set edition mode
   */
  void setEditionMode(mdtSqlFieldSetupEditionMode_t mode);

  /*! \brief Set creation edition mode
   */
  void setEditionModeCreation();

  /*! \brief Set selection edition mode
   */
  void setEditionModeSelection();

  /*! \brief Set Edition edition mode
   */
  void setEditionModeEdition();

  Q_DISABLE_COPY(mdtSqlFieldSetupWidget)

  mdtSqlFieldSetupTableModel *pvFieldModel;
  mdtSqlFieldSetupData pvSetupData;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_FIELD_SETUP_WIDGET_H
