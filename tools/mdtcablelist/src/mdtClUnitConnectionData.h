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
#ifndef MDT_CL_UNIT_CONNECTION_DATA_H
#define MDT_CL_UNIT_CONNECTION_DATA_H

#include "mdtClConnectionData.h"
#include "mdtClArticleConnectionData.h"
#include <QVariant>

/*! \brief Container class that is used to exchange unit connection data
 */
class mdtClUnitConnectionData : public mdtClConnectionData
{
 public:

  /*! \brief Check data validity
   *
   * Data object is valid if:
   *  - id is set.
   *  - unitId is set.
   */
  bool isValid();

  /*! \brief Set unit ID
   */
  void setUnitId(const QVariant & id);

  /*! \brief Get unit ID
   */
  QVariant unitId() const;

  /*! \brief Set unit connector ID
   */
  void setUnitConnectorId(const QVariant & id);

  /*! \brief Get unit connector ID
   */
  QVariant unitConnectorId() const;

  /*! \brief Set schema page
   */
  void setSchemaPage(const QVariant & schemaPage);

  /*! \brief Get schema page
   */
  QVariant schemaPage() const;

  /*! \brief Set signal name
   */
  void setSignalName(const QVariant & name);

  /*! \brief Get signal name
   */
  QVariant signalName() const;

  /*! \brief Set SW address
   */
  void setSwAddress(const QVariant & address);

  /*! \brief Get SW address
   */
  QVariant swAddress() const;

  /*! \brief Set article connection data
   */
  void setArticleConnectionData(const mdtClArticleConnectionData & data);

  /*! \brief Copy article connection attributes to unit connection attributes
   *
   * Will copy some attributes from article connection data (set with setArticleConnectionData() )
   *  to corresponding unit connection attributes .
   */
  void copyArticleConnectionAttributes();

  /*! \brief Get article connection data
   */
  const mdtClArticleConnectionData & articleConnectionData() const;

  /*! \brief Get article connection data (for modifications)
   */
  mdtClArticleConnectionData & articleConnectionData();

 private:

  /*! \brief clear
   */
  void clearSub();

  QVariant pvUintId;
  QVariant pvUnitConnectorId;
  QVariant pvSchemaPage;
  QVariant pvSignalName;
  QVariant pvSwAddress;
  mdtClArticleConnectionData pvArticleConnectionData;
};

#endif  // #ifndef MDT_CL_UNIT_CONNECTION_DATA_H
