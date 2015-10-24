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
#ifndef MDT_SQL_DATABASE_BASIC_INFO_WIDGET_H
#define MDT_SQL_DATABASE_BASIC_INFO_WIDGET_H

#include "mdtSqlDriverType.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QString>

class QLabel;
class QFormLayout;

/*! \brief Display common basic informations on a database
 */
class mdtSqlDatabaseBasicInfoWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseBasicInfoWidget(QWidget *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseBasicInfoWidget(const mdtSqlDatabaseBasicInfoWidget & other) = delete;

  /*! \brief Set driver type
   *
   * Display labels regarding driver type and clear informations.
   *
   * \note This function is automatically called by displayInfo()
   */
  void setDriverType(mdtSqlDriverType::Type driverType);

  /*! \brief Display basic information of given database
   */
  void displayInfo(const QSqlDatabase & db);

 private:

  /*! \brief Update widgets regarding db
   */
  void updateWidgets(const QSqlDatabase & db);

  /*! \brief Update database name widgets
   */
  void updateDatabaseNameWidgets(bool on);

  /*! \brief Build database name widgets
   */
  void buildDatabaseNameWidgets();

  /*! \brief Remove database name widgets
   */
  void removeDatabaseNameWidgets();

  /*! \brief Update directory widgets
   */
  void updateDirectoryWidgets(bool on);

  /*! \brief Build directory widgets
   */
  void buildDirectoryWidgets();

  /*! \brief Remove directory widgets
   */
  void removeDirectoryWidgets();

  /*! \brief Update host and port widgets
   */
  void updateHostAndPortWidgets(bool on);

  /*! \brief Build host and port widgets
   */
  void buildHostAndPortWidgets();

  /*! \brief Remove host and port widgets
   */
  void removeHostAndPortWidgets();

  /*! \brief Update user widgets
   */
  void updateUserWidgets(bool on);

  /*! \brief Build user widgets
   */
  void buildUserWidgets();

  /*! \brief Remove user widgets
   */
  void removeUserWidgets();

  /*! \brief Clear database informations
   */
  void clearDatabaseInfo();

  /*! \brief Display database infromations for SQLite
   */
  void displayInfoSqlite(const QSqlDatabase & db);

  QFormLayout *pvLayout;
  QLabel *lbDatabaseNamelabel;
  QLabel *lbDatabaseName;
  QLabel *lbDirectoryLabel;
  QLabel *lbDirectory;
  QLabel *lbHostLabel;
  QLabel *lbHost;
  QLabel *lbPortLabel;
  QLabel *lbPort;
  QLabel *lbUserLabel;
  QLabel *lbUser;
};

#endif // #ifndef MDT_SQL_DATABASE_BASIC_INFO_WIDGET_H
