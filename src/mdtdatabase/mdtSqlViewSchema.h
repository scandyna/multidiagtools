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

#include <QString>
#include <QStringList>

/*! \brief Helper class for SQL database schema creation
 *
 * This is a basic class that is mainly used
 *  in mdtSqlDatabaseSchema as container for views to create.
 */
class mdtSqlViewSchema
{
 public:

  /*! \brief Constructor
   */
  mdtSqlViewSchema(const QString & name = QString())
   : pvName(name)
  {
  }

  /*! \brief Set the name of the view
   */
  void setName(const QString & name)
  {
    pvName = name;
  }

  /*! \brief Get the name of the view
   */
  QString name() const
  {
    return pvName;
  }

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
  void addSelectItem(const QString & item)
  {
    pvSelectList.append(item);
  }

  /*! \brief Set the SQL statement that must be appended to SELECT statement
   */
  void setAfterSelectStatement(const QString & sql)
  {
    pvAfterSelectStatement = sql;
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvName.clear();
    pvSelectList.clear();
    pvAfterSelectStatement.clear();
  }

  /*! \brief Get the SQL statement to drop the view
   *
   * Note that each call of this function will rebuild the SQL statement.
   */
  QString getSqlForDrop(const QString & tableNameDelimiter) const
  {
    if(pvName.isEmpty()){
      return "";
    }
    return "DROP VIEW IF EXISTS " + tableNameDelimiter + pvName + tableNameDelimiter;
  }

  /*! \brief Get the SQL statement to create the view
   *
   * Note that each call of this function will rebuild the SQL statement.
   */
  QString getSqlForCreate(const QString tableNameDelimiter, const QString fieldNameDelimiter, const QString endOfLine = "\n") const
  {
    QString sql;
    QString space;

    // Check if we need space
    if(endOfLine.isEmpty()){
      space = " ";
    }
    // Check if all is complete
    if( (pvName.isEmpty()) || (pvSelectList.isEmpty()) || (pvAfterSelectStatement.isEmpty()) ){
      return sql;
    }
    // Build header
    sql = "CREATE VIEW " + tableNameDelimiter + pvName + tableNameDelimiter + " AS" + space + endOfLine;
    // Build SELECT statement
    sql += "SELECT" + endOfLine;
    int lastIndex = pvSelectList.size() - 1;
    Q_ASSERT(lastIndex >= 0);
    for(int i = 0; i < lastIndex; ++i){
      sql += " " + fieldNameDelimiter + pvSelectList.at(i) + fieldNameDelimiter + "," + endOfLine;
    }
    sql += " " + fieldNameDelimiter + pvSelectList.at(lastIndex) + fieldNameDelimiter + endOfLine;
    // Add the rest of SQL
    sql += space + pvAfterSelectStatement + endOfLine;

    return sql;
  }

 private:

  QString pvName;
  QStringList pvSelectList;
  QString pvAfterSelectStatement;
};

#endif // #ifndef MDT_SQL_VIEW_SCHEMA_H
