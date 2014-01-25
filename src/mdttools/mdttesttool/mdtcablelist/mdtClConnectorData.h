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
#ifndef MDT_CL_CONNECTOR_DATA_H
#define MDT_CL_CONNECTOR_DATA_H

#include "mdtSqlRecord.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

/*! \brief Data container class for connector data
 *
 * Permit to echange data with Connector_tbl and ConnectorContact_tbl.
 */
class mdtClConnectorData : public mdtSqlRecord
{
 public:

  /*! \brief Setup fields from Connector_tbl
   */
  bool setup(const QSqlDatabase & db);

  /*! \brief Add a list of contact data
   *
   * By using this method, the caller is responsible to give
   *  valid records, that contains needed fields.
   *
   * \sa addContactData().
   */
  void setContactDataList(const QList<mdtSqlRecord> & dataList);

  /*! \brief Get list of contact data
   */
  QList<mdtSqlRecord> contactDataList() const;

  /*! \brief Add contact data
   *
   * \pre data must contains following fields:
   *  - Id_PK
   */
  void addContactData(const mdtSqlRecord & data);

  /*! \brief Update existing contact data
   *
   * Will update contact data for witch Id_PK matches given contactId.
   *
   * \return False if contactId was not found.
   */
  bool setContactData(const QVariant & contactId, const mdtSqlRecord & data);

  /*! \brief Get contact data for given contact ID
   *
   * Return the contact data for witch Id_PK matches given contactId.
   *  ok will be set to false if contactId was not found.
   */
  mdtSqlRecord contactData(const QVariant & contactId, bool *ok) const;

 private:

  QList<mdtSqlRecord> pvContactDataList;
};

#endif // #ifndef MDT_CL_CONNECTOR_DATA_H
