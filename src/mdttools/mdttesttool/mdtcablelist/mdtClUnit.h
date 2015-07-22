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
#ifndef MDT_CL_UNIT_H
#define MDT_CL_UNIT_H

#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"

#include "mdtClLinkData.h"

#include "mdtTtBase.h"
#include "mdtError.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QModelIndex>

/*! \brief Helper class to manage unit and related data
 */
class mdtClUnit : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClUnit(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClUnit(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnit();

  /*! \brief Get SQL statement for component selection
   *
   * List all units, expected those that are allready component of given unitId
   */
  QString sqlForComponentSelection(const QVariant & unitId) const;

  /*! \brief Get a list of unit connection ID that are part of given unit connector ID
   *
   * \deprecated
   */
  QList<QVariant> getConnectionIdListPartOfConnectorId(const QVariant & unitConnectorId, bool *ok);

  /*! \brief Get unit connector ID of given unit connection ID
   *
   * \deprecated
   */
  QVariant getConnectorIdOfConnectionId(const QVariant & unitConnectionId, bool *ok);

  /*! \brief Add a unit as component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addComponent(const QVariant &unitId, const QVariant &componentId);

  /*! \brief Remove a single component
   */
  bool removeComponent(const QVariant &componentId);

  /*! \brief Remove a list of components
   */
  bool removeComponents(const QList<QVariant> &componentIdList);

  /*! \brief Remove each component that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeComponents(const mdtSqlTableSelection & s);

  /*! \brief Return the number unit connections that are related to article connections
   *
   * \return A number >= 0 on success, a value < 0 on error.
   *         On error, use lastError() to check what goes wrong.
   * 
   * \todo Migrate to ???
   */
  int toUnitRelatedArticleConnectionCount(const QVariant & unitId);

  /*! \brief Get a list of links that are related to given unit ID
   *
   * This is a helper method to display a message to the user in a simple way.
   *  Do not use the result for technical processing.
   *
   * \todo Migrate to ???
   */
  QStringList toUnitRelatedLinksList(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList, bool *ok);

  /*! \brief Get a list of links that are related to given unit ID
   *
   * This is a helper method to display a message to the user in a simple way.
   *  Do not use the result for technical processing.
   *
   * \todo Migrate to ???
   */
  QString toUnitRelatedLinksListStr(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList, bool *ok);

  /*! \brief
   */
  bool addRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData);

 private:

  Q_DISABLE_COPY(mdtClUnit)
};

#endif  // #ifndef MDT_CL_UNIT_H
