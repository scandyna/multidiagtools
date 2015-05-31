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
#ifndef MDT_TT_TEST_SYSTEM_COMPONENT_H
#define MDT_TT_TEST_SYSTEM_COMPONENT_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include <QVariant>
#include <QString>

/*! \brief Test system assignation data
 */
struct mdtTtTestSystemAssignationData
{
  QVariant testSystemId;
  QVariant testSystemComponentId;
};

/*! \brief Helper class for test system and component
 */
class mdtTtTestSystemComponent : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestSystemComponent(QObject* parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtTtTestSystemComponent(QSqlDatabase db);

  /*! \brief Add a test system assignation
   *
   * Adds a entry in TestSystem_TestSystemComponent_tbl
   */
  bool addTestSystemAssignation(const mdtTtTestSystemAssignationData & data);

  /*! \brief Remove a test system assignation
   *
   * Removes a entry in TestSystem_TestSystemComponent_tbl
   */
  bool removeTestSystemAssignation(const mdtTtTestSystemAssignationData & data);

  /*! \brief Remove a test system assignation
   *
   * Removes a entry in TestSystem_TestSystemComponent_tbl
   */
  bool removeTestSystemAssignation(const mdtSqlTableSelection & s);

  /*! \brief Create a new test system unit
   *
   * Will handle creation in Unit_tbl and TestSystemUnit_tbl
   *
   * \return ID of created test system unit. If a error occured, a null ID is returned.
   */
  QVariant createUnit(const QVariant & componentId, const QString & typeCode, const QVariant & baseAticleId);

  /*! \brief Remove a test system unit
   *
   *  Will handle deletion in Unit_tbl and TestSystemUnit_tbl
   *
   * \param id ID of test system unit to remove (delete)
   * \param handleTransaction Because data must be removed in Unit_tbl and TestSystemUnit_tbl,
   *          a (explicit) transaction is used. If a transaction was started before calling this function,
   *          set this parameter false.
   */
  bool removeUnit(const QVariant & id, bool handleTransaction);

  /*! \brief Remove test system units contained in selection
   *
   * \sa removeUnit()
   */
  bool removeUnits(const mdtSqlTableSelection & s);

 private:

  Q_DISABLE_COPY(mdtTtTestSystemComponent);
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_COMPONENT_H
