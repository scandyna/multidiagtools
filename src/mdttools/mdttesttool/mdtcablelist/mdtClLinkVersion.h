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
#ifndef MDT_CL_LINK_VERSION_H
#define MDT_CL_LINK_VERSION_H

#include "mdtTtBase.h"
#include "mdtClLinkVersionData.h"
#include <QSqlDatabase>

class QObject;
class QWidget;

/*! \brief Helper class for link version manipluation
 *
 * Refers to LinkVersion_tbl
 */
class mdtClLinkVersion : public mdtTtBase
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkVersion(QObject* parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClLinkVersion(QSqlDatabase db);

  /*! \brief Set current version
   *
   * Will first check if requested version exists in LinkVersion_tbl
   *  and set it if ok.
   *  If requested version does not exist, or a error occured,
   *  this function returns false, updates lastError and clears currentVersion.
   *
   * \pre version must be > 0
   */
  bool setCurrentVersion(double version);

  /*! \brief Get current version
   *
   * \sa setCurrentVersion()
   */
  static mdtClLinkVersionData currentVersion()
  {
    return pvCurrentVersionData;
  }

  /*! \brief Add a version into LinkVersion_tbl
   */
  bool addVersion(const mdtClLinkVersionData & data);

  /*! \brief Get link version data from database
   *
   * \param key Data that contains link version PK (Version_PK).
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClLinkVersionData getVersionData(const mdtClLinkVersionPkData & key, bool & ok);

  /*! \brief Get last link version data
   *
   * \return data for last link version in database.
   *       A null data is returned if no version exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClLinkVersionData getLastVersionData(bool & ok);

  /*! \brief Remove a version from LinkVersion_tbl
   */
  bool removeVersion(const mdtClLinkVersionPkData & key);

 public slots:

  /*! \brief Create a new link version
   *
   * This function is interactive (will display dialog to the user, etc..).
   *
   * \param parentWidget Widget to use as parent for dialogs
   */
  void createVersion(QWidget *parentWidget = nullptr);

 private:

  Q_DISABLE_COPY(mdtClLinkVersion);

  static mdtClLinkVersionData pvCurrentVersionData;
};

#endif // MDT_CL_LINK_VERSION_H
