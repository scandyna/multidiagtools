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
#ifndef MDT_SQL_DRIVER_TYPE_H
#define MDT_SQL_DRIVER_TYPE_H

#include <QString>
#include <QLatin1String>

/*! \brief SQL driver type
 *
 * The SQL module of multiDiagTools library uses the Qt SQL module,
 *  witch supports many SQL engines via QSqlDriver plugin system.
 *
 * Additionally, multiDiagTools tries to provide some functionnalities
 *  for schema generation and edition.
 *  This part is currently very limited (partial support of Sqlite and MySql/MariaDB).
 *
 * mdtSqlDriverType provides a type for database engines that are supported by multiDiagTools.
 */
class mdtSqlDriverType
{
 public:

  /*! \brief Database engine type
   */
  enum Type
  {
    Unknown,  /*!< Database engine is not defined, or not supported by multiDiagTools library */
    SQLite,   /*!< SQLite database engine */
    MariaDB,  /*!< MariaDB database engine */
    MySQL     /*!< MySQL database engine */
  };

  /*! \brief Default constructor
   */
  mdtSqlDriverType()
   : pvType(mdtSqlDriverType::Unknown)
   {
   }

  /*! \brief Set type
   */
  void setType(Type type)
  {
    pvName = nameFromType(type);
    pvType = type;
  }

  /*! \brief Set type from driver name
   *
   * \param name Driver name used by QSqlDatabase
   * \return true if name represents a supported type, false else.
   */
  bool setType(const QString & name)
  {
    pvType = typeFromName(name);
    if(pvType == Unknown){
      pvName.clear();
      return false;
    }
    pvName = name;
    return true;
  }

  /*! \brief Get type
   */
  Type type() const
  {
    return pvType;
  }

  /*! \brief Get name that is used by QSqlDatabase
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvType = Unknown;
    pvName.clear();
  }

  /*! \brief Check if driver type is null
   */
  bool isNull() const
  {
    return (pvType == Unknown);
  }

  /*! \brief Returns the driver name used by QSqlDatabase
   *
   * If given type is not supported by multiDiagTools,
   *  a empty string is returned.
   */
  static QString nameFromType(Type type)
  {
    switch(type){
      case Unknown:
        return QLatin1String("");
      case SQLite:
        return QLatin1String("QSQLITE");
      case MariaDB:
      case MySQL:
        return QLatin1String("QMYSQL");
    }
    return QLatin1String("");
  }

  /*! \brief Returns database engine type
   *
   * \param name Driver name used by QSqlDatabase
   */
  static Type typeFromName(const QString & name)
  {
    if(name == QLatin1String("QSQLITE")){
      return SQLite;
    }else if(name == QLatin1String("QMYSQL")){
      return MySQL;
    }else{
      return Unknown;
    }
  }

 private:

  Type pvType;
  QString pvName;
};

#endif // #ifndef
