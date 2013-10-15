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

#include <QVariant>

/*! \brief Container class that is used to exchange unit connection data
 */
class mdtClUnitConnectionData
{
 public:

  /*! \brief Constructor
   */
  mdtClUnitConnectionData();

  /*! \brief Destructor
   */
  ~mdtClUnitConnectionData();

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Check data validity
   *
   * Data object is valid if:
   *  - id is set.
   *  - unitId is set.
   */
  bool isValid();

  /*! \brief Set unit connection ID
   */
  void setId(const QVariant & id);

  /*! \brief Get unit connection ID
   */
  QVariant id();

  /*! \brief Set unit ID
   */
  void setUnitId(const QVariant & id);

  /*! \brief Get unit ID
   */
  QVariant unitId() const;

  /*! \brief Set article connection ID
   */
  void setArticleConnectionId(const QVariant & id);

  /*! \brief Get article connection ID
   */
  QVariant articleConnectionId() const;

  /*! \brief Set article connector name
   */
  void setArticleConnectorName(const QVariant &name);

  /*! \brief Get article connector name
   */
  QVariant articleConnectorName() const;

  /*! \brief Set article contact name
   */
  void setArticleContactName(const QVariant &name);

  /*! \brief Get article contact name
   */
  QVariant articleContactName() const;

  /*! \brief Set article I/O type
   */
  void setArticleIoType(const QVariant &ioType);

  /*! \brief Get article I/O type
   */
  QVariant articleIoType() const;

  /*! \brief Set article function EN
   */
  void setArticleFunctionEN(const QVariant &function);

  /*! \brief Get article function EN
   */
  QVariant articleFunctionEN() const;

  /*! \brief Set article function FR
   */
  void setArticleFunctionFR(const QVariant &function);

  /*! \brief Get article function FR
   */
  QVariant articleFunctionFR() const;

  /*! \brief Set article function DE
   */
  void setArticleFunctionDE(const QVariant &function);

  /*! \brief Get article function DE
   */
  QVariant articleFunctionDE() const;

  /*! \brief Set article function IT
   */
  void setArticleFunctionIT(const QVariant &function);

  /*! \brief Get article function IT
   */
  QVariant articleFunctionIT() const;

  /*! \brief Set schema page
   */
  void setSchemaPage(const QVariant & schemaPage);

  /*! \brief Get schema page
   */
  QVariant schemaPage() const;

  /*! \brief Set function (English)
   */
  void setFunctionEN(const QVariant & function);

  /*! \brief Get function (English)
   */
  QVariant functionEN() const;

  /*! \brief Set function (Frensh)
   */
  void setFunctionFR(const QVariant & function);

  /*! \brief Get function (Frensh)
   */
  QVariant functionFR() const;

  /*! \brief Set function (German)
   */
  void setFunctionDE(const QVariant & function);

  /*! \brief Get function (German)
   */
  QVariant functionDE() const;

  /*! \brief Set function (Italian)
   */
  void setFunctionIT(const QVariant & function);

  /*! \brief Get function (Italian)
   */
  QVariant functionIT() const;

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

  /*! \brief Set unit connector name
   */
  void setUnitConnectorName(const QVariant & name);

  /*! \brief Get unit connector name
   */
  QVariant unitConnectorName() const;

  /*! \brief Set unit contact name
   */
  void setUnitContactName(const QVariant & name);

  /*! \brief Get unit contact name
   */
  QVariant unitContactName() const;

 private:

  QVariant pvId;
  QVariant pvUintId;
  QVariant pvArticleConnectionId;
  QVariant pvArticleConnectorName;
  QVariant pvArticleContactName;
  QVariant pvArticleIoType;
  QVariant pvArticleFunctionEN;
  QVariant pvArticleFunctionFR;
  QVariant pvArticleFunctionDE;
  QVariant pvArticleFunctionIT;
  QVariant pvSchemaPage;
  QVariant pvFunctionEN;
  QVariant pvFunctionFR;
  QVariant pvFunctionDE;
  QVariant pvFunctionIT;
  QVariant pvSignalName;
  QVariant pvSwAddress;
  QVariant pvUnitConnectorName;
  QVariant pvUnitContactName;
};

#endif  // #ifndef MDT_CL_UNIT_CONNECTION_DATA_H
