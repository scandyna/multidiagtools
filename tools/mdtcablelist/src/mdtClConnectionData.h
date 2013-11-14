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
#ifndef MDT_CL_CONNECTION_DATA_H
#define MDT_CL_CONNECTION_DATA_H

#include <QVariant>

/*! \brief Container for connection data exchange
 */
class mdtClConnectionData
{
 public:

  /*! \brief Set connector ID
   *
   * Note: do not confound with unit or article connector ID .
   */
  void setConnectorId(const QVariant & id);

  /*! \brief Get connector ID
   *
   * Note: do not confound with unit or article connector ID .
   */
  QVariant connectorId() const;

  /*! \brief Set connection ID
   */
  void setConnectionId(const QVariant & id);

  /*! \brief Get connection ID
   */
  QVariant connectionId() const;

  /*! \brief Set connector name
   */
  void setConnectorName(const QVariant & name);

  /*! \brief Get connector name
   */
  QVariant connectorName() const;

  /*! \brief Set contact name
   */
  void setContactName(const QVariant & name);

  /*! \brief Get contact name
   */
  QVariant contactName() const;

  /*! \brief Set function EN
   */
  void setFunctionEN(const QVariant & function);

  /*! \brief Get function EN
   */
  QVariant functionEN() const;

  /*! \brief Set function FR
   */
  void setFunctionFR(const QVariant & function);

  /*! \brief Get function FR
   */
  QVariant functionFR() const;

  /*! \brief Set function DE
   */
  void setFunctionDE(const QVariant & function);

  /*! \brief Get function DE
   */
  QVariant functionDE() const;

  /*! \brief Set function IT
   */
  void setFunctionIT(const QVariant & function);

  /*! \brief Get function IT
   */
  QVariant functionIT() const;

 private:

  QVariant pvConnectorId;
  QVariant pvConnectionId;
  QVariant pvConnectorName;
  QVariant pvContactName;
  QVariant pvFunctionEN;
  QVariant pvFunctionFR;
  QVariant pvFunctionDE;
  QVariant pvFunctionIT;
};


#endif // #ifndef MDT_CL_CONNECTION_DATA_H
