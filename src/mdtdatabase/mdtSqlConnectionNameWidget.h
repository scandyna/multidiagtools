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
#ifndef MDT_SQL_CONNECTION_NAME_WIDGET_H
#define MDT_SQL_CONNECTION_NAME_WIDGET_H

#include "ui_mdtSqlConnectionNameWidget.h"
#include "mdtSqlDriverType.h"
#include <QSqlDatabase>
#include <QWidget>
#include <QStringList>

/*! \brief Manage QSqlDatabase connections
 *
 * mdtSqlConnectionNameWidget will list existing connections and
 *  let the user select, add and remove one.
 */
class mdtSqlConnectionNameWidget : public QWidget, Ui::mdtSqlConnectionNameWidget
{
 friend class mdtDatabaseTest;

 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlConnectionNameWidget(QWidget *parent = nullptr);

  /*! \brief Set driver type
   *
   * Driver type is used to list only connections for a specific driver.
   */
  void setDriverType(mdtSqlDriverType::Type type);

  /*! \brief Update connections list
   *
   * Update the connections combo box with currently available
   *  connections that uses the driver set by setDriverType().
   */
  void updateConnectionsList();

  /*! \brief Set current connection to given connection name
   */
  void setCurrentConnection(const QString & name);

  /*! \brief Get a list containing the name of existing connections
   *
   * This is the same as QSqlDatabase::connectionNames() ,
   *  but only contains connections that uses given driver type.
   *
   * Note: each call of this function will build the list.
   */
  static QStringList getConnectionNames(mdtSqlDriverType::Type type, bool sort);

 signals:

  /*! \brief Emitted when a connection was selected
   */
  void currentDatabaseChanged(const QSqlDatabase & db);

  /*! \brief Emitted just before current database connection is removed
   */
  void aboutToRemoveDatabaseConnection(const QString & connectionName);

 private slots:

  /*! \brief Add a database connection
   */
  void addConnection();

  /*! \brief Remove selected connection
   */
  void removeSelectedConnection();

  /*! \brief Update widget state with currently selected connection
   *
   * will also emit currentConnectionNameChanged()
   */
  void onCbConnectionNamesIndexChanged(int index);

 private:

  Q_DISABLE_COPY(mdtSqlConnectionNameWidget)

  mdtSqlDriverType::Type pvDriverType;
};

#endif // #ifndef MDT_SQL_CONNECTION_NAME_WIDGET_H
