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
#ifndef MDT_CL_CONNECTOR_DATA_H
#define MDT_CL_CONNECTOR_DATA_H

#include "mdtSqlRecord.h"
#include "mdtClConnectorKeyData.h"
#include "mdtClConnectorContactData.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Data container class for connector data
 *
 * Refers to Connector_tbl
 */
struct mdtClConnectorData : public mdtSqlRecord /// \todo When all is adapted, remove this inheritance
{
 private:

  /*! \brief Connector key data
   */
  mdtClConnectorKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  inline mdtClConnectorKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   */
  void setKeyData(const mdtClConnectorKeyData & key);

  /*! \brief Connector gender
   *
   * \todo Currently, gender is a simple string, wicth is not typed..
   */
  QVariant gender;

  /*! \brief Connector form
   *
   * \todo Currently, form is a simple string, wicth is not typed..
   */
  QVariant form;

  /*! \brief Manufacturer
   */
  QVariant manufacturer;

  /*! \brief Manufacturer config code
   */
  QVariant manufacturerConfigCode;

  /*! \brief Manufacturer article code
   */
  QVariant manufacturerArticleCode;

  /*! \brief Add contact to connector
   *
   * Note: in given contact key data,
   *  connectorFk is ignored and replaced by connector ID
   */
  void addContactData(mdtClConnectorContactData data);

  /*! \brief Set list of contacts
   *
   * \sa addContactData()
   */
  void setContactDataList(const QList<mdtClConnectorContactData> & dataList);

  /*! \brief Access list of contacts
   */
  const QList<mdtClConnectorContactData> & contactDataList() const
  {
    return pvContactDataList;
  }

  /*! \brief Check if data is null
   *
   * Data is considered null if keyData is null
   */
  inline bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear();

 private:

  QList<mdtClConnectorContactData> pvContactDataList;
};

#endif // #ifndef MDT_CL_CONNECTOR_DATA_H
