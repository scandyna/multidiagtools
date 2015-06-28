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
#ifndef MDT_CL_LINK_VERSION_DATA_H
#define MDT_CL_LINK_VERSION_DATA_H

#include "mdtClLinkVersionKeyData.h"
#include <QVariant>

/*! \brief Link version data
 *
 * Refers to LinkVersion_tbl
 *
 * Link version is composed of a
 *  version in floating value (f.ex. 1.2)
 *  and a integral value, witch is the primary key.
 *  The integral value is the floating version value x 1000.
 *  For example, for version 1.2 , the integral version will be represented as 1200.
 */
struct mdtClLinkVersionData
{
 private:

  /*! \brief Primary key data
   */
  mdtClLinkVersionPkData pvPk;

  /*! \brief Version
   */
  double pvVersion;

 public:

  /*! \brief Constructor
   */
  mdtClLinkVersionData()
   : pvVersion(0.0)
  {
  }

  /*! \brief Set primary key data
   *
   * \pre key mus not be null
   */
  void setPk(const mdtClLinkVersionPkData & key)
  {
    Q_ASSERT(!key.isNull());
    pvPk = key;
    pvVersion = static_cast<double>(key.versionPk.value()) / 1000.0;
  }

  /*! \brief Get primary key data
   */
  mdtClLinkVersionPkData pk() const
  {
    return pvPk;
  }

  /*! \brief Set version (Version_PK)
   *
   * \pre v must be > 0
   */
  void setVersionPk(int v)
  {
    Q_ASSERT(v > 0);
    pvPk.versionPk.setValue(v);
    pvVersion = static_cast<double>(v) / 1000.0;
  }

  /*! \brief Set version (Version_PK)
   *
   * If v is null, data will be cleared.
   *
   * \pre v must be a signed integral type
   * \pre v must be null or > 0
   */
  void setVersionPk(const QVariant & v)
  {
    Q_ASSERT( (v.type() == QVariant::Invalid) || (v.type() == QVariant::Int) || (v.type() == QVariant::LongLong) );
    Q_ASSERT( (v.isNull()) || (v.toInt() > 0) );

    if(v.isNull()){
      clear();
    }else{
      setVersionPk(v.toInt());
    }
  }

  /*! \brief Get version (Version_PK)
   */
  inline int versionPk() const
  {
    return pvPk.versionPk.value();
  }

  /*! \brief Set version (Version)
   *
   * \pre v must be > 0
   */
  void setVersion(double v)
  {
    Q_ASSERT(v > 0.0);
    pvPk.versionPk.setValue(v * 1000);
    pvVersion = v;
  }

  /*! \brief Get version (Version)
   */
  inline double version() const
  {
    return pvVersion;
  }

  /*! \brief Check if version data is null
   *
   * Version data is null when its primary key is null
   */
  inline bool isNull() const
  {
    return pvPk.isNull();
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvPk.clear();
    pvVersion = 0.0;
  }
};

#endif // #ifndef MDT_CL_LINK_VERSION_DATA_H
