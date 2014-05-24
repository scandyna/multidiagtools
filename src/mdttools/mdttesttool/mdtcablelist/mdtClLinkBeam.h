/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_CL_LINK_BEAM_H
#define MDT_CL_LINK_BEAM_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QList>

class QObject;

/*! \brief Helper class for link beam edition
 */
class mdtClLinkBeam : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClLinkBeam(QObject *parent, QSqlDatabase db);

  /*! \brief Get SQL statement for start unit selection
   *
   * List all units that are not currently affected to given link beam.
   */
  QString sqlForStartUnitSelection(const QVariant & linkBeamId);

  /*! \brief Get SQL statement for end unit selection
   *
   * List all units that are not currently affected to given link beam.
   */
  QString sqlForEndUnitSelection(const QVariant & linkBeamId);

  /*! \brief Get SQL statement for link add selection
   *
   * List all links that starts from one of given start units,
   *  and ends from one of given end units,
   *  and that are not currently affected to a link beam.
   */
  QString sqlForLinkSelection(const QList<QVariant> & unitStartIdList, const QList<QVariant> & unitEndIdList) const;

  /*! \brief Add a start unit
   */
  bool addStartUnit(const QVariant & unitId, const QVariant & linkBeamId);

  /*! \brief Remove a start unit
   */
  bool removeStartUnit(const QVariant & unitId, const QVariant & linkBeamId);

  /*! \brief Remove start units from selection
   */
  bool removeStartUnits(const mdtSqlTableSelection & s);

  /*! \brief Add a end unit
   */
  bool addEndUnit(const QVariant & unitId, const QVariant & linkBeamId);

  /*! \brief Remove a end unit
   */
  bool removeEndUnit(const QVariant & unitId, const QVariant & linkBeamId);

  /*! \brief Remove end units from selection
   */
  bool removeEndUnits(const mdtSqlTableSelection & s);

  /*! \brief Add a existing link to given link beam
   */
  bool addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, const QVariant & linkBeamId);

  /*! \brief Remove links from beam (will not delete links)
   */
  bool removeLinks(const mdtSqlTableSelection & s);

  /*! \brief Remove a link from beam (will not delete links)
   */
  bool removeLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);

 private:

  Q_DISABLE_COPY(mdtClLinkBeam);
};

#endif // #ifndef MDT_CL_LINK_BEAM_H
