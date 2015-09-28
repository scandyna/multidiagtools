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
#ifndef MDT_SQL_VIEW_SCHEMA_H
#define MDT_SQL_VIEW_SCHEMA_H

#include "mdtSqlViewSchemaJoinClause.h"
#include <QString>
#include <QStringList>
#include <QSqlDriver>
#include <QList>

/*! \brief Helper class for SQL database schema creation
 *
 * This is a basic class that is mainly used
 *  in mdtSqlDatabaseSchema as container for views to create.
 */
class mdtSqlViewSchema
{
 public:

  /*! \brief SELECT suffix
   */
  enum SelectSuffix
  {
    SelectSuffixNone,     /*!< Select statement is simply SELECT */
    SelectSuffixAll,      /*!< Select statmenet becomes SELECT ALL */
    SelectSuffixDistinct  /*!< Select statmenet becomes SELECT DISTINCT */
  };

  /*! \brief Constructor
   */
  mdtSqlViewSchema(const QString & name = QString())
   : pvName(name),
     pvSelectSuffix(SelectSuffixNone)
  {
  }

  /*! \brief Set the name of the view
   */
  void setName(const QString & name);

  /*! \brief Get the name of the view
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Set table name
   */
  void setTableName(const QString & name, const QString & alias = QString());

  /*! \brief Set select suffix
   */
  void setSelectSuffix(SelectSuffix s);

  /*! \brief Add a SELECT item
   *
   * Given item will be added as is to the list of fields
   *  that must appear in the select statement.
   *
   * For example, if we call
   *  addSelectItem("Id_PK"), 
   *  addSelectItem("Name AS AliasedName"),
   *  addSelectItem("CLI.PhoneNumber"),
   *  the generated part of the SELECT statement will be:
   *  SELECT Id_PK, Name AS AliasedName, CLI.PhoneNumber
   */
  void addSelectItem(const QString & item);

  /*! \brief Add a join clause
   */
  void addJoinClause(const mdtSqlViewSchemaJoinClause & jc);

  /*! \brief Clear
   */
  void clear();

  /*! \brief Get the SQL statement to drop the view
   *
   * Note that each call of this function will rebuild the SQL statement.
   *
   * \pre driver must not be a null pointer
   */
  QString getSqlForDrop(const QSqlDriver *driver) const;

  /*! \brief Get the SQL statement to create the view
   *
   * Note that each call of this function will rebuild the SQL statement.
   *
   * \pre driver must not be a null pointer
   */
  QString getSqlForCreate(const QSqlDriver *driver) const;

 private:

  /*! \brief Get select key word regarding select suffix
   */
  QString selectKeyWord() const
  {
    switch(pvSelectSuffix){
      case SelectSuffixNone:
        return "SELECT";
      case SelectSuffixAll:
        return "SELECT ALL";
      case SelectSuffixDistinct:
        return "SELECT DISTINCT";
    }
    return QString();
  }

  QString pvName;
  QString pvTableName;
  QString pvTableAlias;
  SelectSuffix pvSelectSuffix;
  QStringList pvSelectList;
  QList<mdtSqlViewSchemaJoinClause> pvJoinClauseList;
//   QString pvAfterSelectStatement;
};

#endif // #ifndef MDT_SQL_VIEW_SCHEMA_H
