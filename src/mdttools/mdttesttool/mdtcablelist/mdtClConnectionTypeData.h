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
#ifndef MDT_CL_CONNECTION_TYPE_DATA_H
#define MDT_CL_CONNECTION_TYPE_DATA_H

#include <QVariant>
#include <QString>
#include <QLocale>

/*! \brief Connection type
 */
enum class mdtClConnectionType_t
{
  Undefined,  /*!< Undefined type (is the case for null data) */
  Terminal,   /*!< Terminal */
  Pin,        /*!< Pin (male contact) */
  Socket      /*!< Socket (female contact) */
};

/*! \brief Connection type key data
 *
 * Refers to ConnectionType_tbl
 */
struct mdtClConnectionTypeKeyData
{
  /*! \brief code (Code_PK)
   */
  QVariant code;

  /*! \brief Get connection type
   */
  mdtClConnectionType_t type() const;

  /*! \brief Set connection type
   *
   * Will set code regarding given type.
   */
  void setType(mdtClConnectionType_t t);

  /*! \brief Check if key data is null
   */
  bool isNull() const
  {
    return code.isNull();
  }

  /*! \brief Clear
   */
  void clear()
  {
    code.clear();
  }
};

/*! \brief Connection type data
 *
 * Refers to ConnectionType_tbl
 */
struct mdtClConnectionTypeData
{
 private:

  /*! \brief Connection type key data
   */
  mdtClConnectionTypeKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  inline mdtClConnectionTypeKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set connection type code (ConnectionType_Code_FK)
   */
  void setCode(const QVariant & code)
  {
    pvKeyData.code = code;
  }

  /*! \brief Set key data
   */
  void setKeyData(const mdtClConnectionTypeKeyData & key)
  {
    pvKeyData = key;
  }

  /*! \brief Get connection type
   */
  mdtClConnectionType_t type() const
  {
    return pvKeyData.type();
  }

  /*! \brief Set connection type
   *
   * Will set keyData regarding given type.
   *  Note that names are cleared.
   *  To get them, data must be reloaded from database table.
   */
  void setType(mdtClConnectionType_t t);

  /*! \brief name EN (NameEN)
   */
  QVariant nameEN;

  /*! \brief name FR (NameFR)
   */
  QVariant nameFR;

  /*! \brief name DE (NameDE)
   */
  QVariant nameDE;

  /*! \brief name IT (NameIT)
   */
  QVariant nameIT;

  /*! \brief Get name regarding given locale
   */
  QString name(const QLocale & locale = QLocale()) const;

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
};

#endif
