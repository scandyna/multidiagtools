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
#include <QList>
#include <QVector>

class QSqlDriver;

namespace mdtSqlViewSchema
{
  /*! \brief Field of a SQL SELECT statement of a view schema
   */
  struct SelectField
  {
    /*! \brief Default constructor
     */
    SelectField()
    {
    }

    /*! \brief Constructor
     */
    explicit SelectField(const QString & name, const QString & alias = QString())
     : pvName(name), pvAlias(alias)
    {
    }

    /*! \brief Set technical field name
     *
     * \note It's also possible to specify * as field name
     */
    void setFieldName(const QString & name)
    {
      pvName = name;
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return pvName;
    }

    /*! \brief Set alias
     */
    void setAlias(const QString & alias)
    {
      pvAlias = alias;
    }

    /*! \brief Get alias
     */
    QString alias() const
    {
      return pvAlias;
    }

    /*! \brief Set table name or alias
     *
     * \note This attribute is defined by Schema
     */
    void setTable(const QString & table)
    {
      pvTable = table;
    }

    /*! \brief Get table name or alias
     */
    QString table() const
    {
      return pvTable;
    }

    /*! \brief Clear
     */
    void clear();

     /*! \brief Get SQL statement
     *
     * \pre driver must not be a null pointer
     */
    QString getSql(const QSqlDriver * const driver) const;

  private:

    QString pvName;
    QString pvAlias;
    QString pvTable;
  };

  /*! \brief Table of a SQL view schema
   */
  class Table
  {
   public:

    /*! \brief Default constructor
     */
    Table()
    {
    }

    /*! \brief Constructor
     */
    explicit Table(const QString & name, const QString & alias = QString())
     : pvTableName(name), pvAlias(alias)
    {
    }

    /*! \brief Set technical table name
     */
    void setTableName(const QString & name)
    {
      pvTableName = name;
    }

    /*! \brief Get technical table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Set aliased table name
     */
    void setAlias(const QString & alias)
    {
      pvAlias = alias;
    }

    /*! \brief Get aliased table name
     */
    QString alias() const
    {
      return pvAlias;
    }

    /*! \brief Get alias if set, or table name else
     */
    QString aliasOrTableName() const
    {
      if(pvAlias.isEmpty()){
        return pvTableName;
      }else{
        return pvAlias;
      }
    }

    /*! \brief Clear
     */
    void clear();

   private:

    QString pvTableName;
    QString pvAlias;
  };

  /*! \brief SQL JOIN constraint
   */
  class JoinKey
  {
   public:

    /*! \brief JOIN field constraint operator
     */
    enum FieldOperator
    {
      Equal,              /*!< Maint table field = table to join field */
      MtfLessThanTdjf,    /*!< Maint table field < table to join field */
      MtfGreaterThanTdjf  /*!< Maint table field > table to join field */
    };

    /*! \brief JOIN constraint operator
     */
    enum ConstraintOperator
    {
      On,   /*!< ON operator */
      And,  /*!< AND operator */
      Or    /*!< OR operator */
    };

    /*! \brief Constructor
     */
    JoinKey()
     : pvConstraintOperator(On), pvFieldOperator(Equal)
    {
    }

    /*! \brief Set main table field
     */
    void setMainTableField(const QString & fieldName)
    {
      pvMainTableField = fieldName;
    }

    /*! \brief Get main table field
     */
    QString mainTableField() const
    {
      return pvMainTableField;
    }

    /*! \brief Set table to join field
     */
    void setTableToJoinField(const QString & fieldName)
    {
      pvTableToJoinField = fieldName;
    }

    /*! \brief Get table to join field
     */
    QString tableToJoinField() const
    {
      return pvTableToJoinField;
    }

    /*! \brief Set JOIN constraint operator
     *
     * Default is ON
     */
    void setConstraintOperator(ConstraintOperator op)
    {
      pvConstraintOperator = op;
    }

    /*! \brief Get JOIN constraint operator
     */
    ConstraintOperator constraintOperator() const
    {
      return pvConstraintOperator;
    }

    /*! \brief Get string representation of JOIN constraint operator
     */
    QString constraintOperatorStr() const;

    /*! \brief Set JOIN field constraint operator
     *
     * Default operator is Equal (=)
     */
    void setFieldOperator(FieldOperator op)
    {
      pvFieldOperator = op;
    }

    /*! \brief Get JOIN field constraint operator
     */
    FieldOperator fieldOperator() const
    {
      return pvFieldOperator;
    }

    /*! \brief Get string representation of JOIN field constraint operator
     */
    QString fieldOperatorStr() const;

    /*! \brief Clear
     */
    void clear();

   private:

    ConstraintOperator pvConstraintOperator;
    FieldOperator pvFieldOperator;
    QString pvMainTableField;
    QString pvTableToJoinField;

  };

  /*! \brief SQL JOIN clause
   */
  class JoinClause
  {
   public:

    /*! \brief JOIN operator
     */
    enum Operator
    {
      Join,     /*!< JOIN operator */
      LeftJoin  /*!< LEFT JOIN operator */
    };

    /*! \brief Constructor
     */
    JoinClause()
     : pvOperator(Join)
    {
    }

    /*! \brief Set main table
     */
    void setMainTable(const Table & table)
    {
      pvMainTable = table;
    }

    /*! \brief Set table to join
     */
    void setTableToJoin(const Table & table)
    {
      pvTableToJoin = table;
    }

    /*! \brief Add a key (i.e. a JOIN constraint)
     */
    void addKey(const JoinKey & key)
    {
      pvKeyList.append(key);
    }

    /*! \brief Set JOIN operator
     *
     * Default is JOIN
     */
    void setOperator(Operator op)
    {
      pvOperator = op;
    }

    /*! \brief Check if clause is null
     *
     * Join clause is Null if main table
     *  or table to join is not defined.
     */
    bool isNull() const
    {
      return (pvMainTable.tableName().isEmpty() || pvTableToJoin.tableName().isEmpty());
    }

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get SQL statement
     *
     * \pre driver must not be a null pointer
     */
    QString getSql(const QSqlDriver * const driver) const;

   private:

    /*! \brief Get string representation of pvOperator
     */
    QString operatorStr() const;

    Operator pvOperator;
    Table pvMainTable;
    Table pvTableToJoin;
    QVector<JoinKey> pvKeyList;
  };

  /*! \brief Helper class for SQL database schema creation
  *
  * This is a basic class that is mainly used
  *  in mdtSqlDatabaseSchema as container for views to create.
  *
  * Typical usage:
  * \code
  *  using namespace mdtSqlViewSchema;
  *  Table client("Client_tbl", "CLI");
  *  Table address("Address_tbl", "ADR");
  *  JoinClause join;
  *  JoinKey key;
  *  Schema view;
  *
  *  view.setName("Client_address_view");
  *  view.setTable(client);
  *  view.addSelectField(client, SelectField("Id_PK", "Client_Id"));
  *  view.addSelectField(client, SelectField("Name"));
  *  view.addSelectField(address, SelectField("Id_PK", "Address_Id"));
  *  view.addSelectField(address, SelectField("Street"));
  *  join.clear();
  *  join.setMainTable(client);
  *  join.setTableToJoin(address);
  *  key.clear();
  *  key.setMainTableField("Id_PK");
  *  key.setTableToJoinField("Client_Id_FK");
  *  join.addKey(key);
  * 
  * \endcode
  */
  class Schema
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
    Schema(const QString & name = QString())
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

    /*! \brief Set table
     *
     * Will be used for the FROM clause
     */
    void setTable(const Table & table);

    /*! \brief Set table name
    */
    void setTableName(const QString & name, const QString & alias = QString());

    /*! \brief Set select suffix
    */
    void setSelectSuffix(SelectSuffix s);

    /*! \brief Add a field to select
     *
     * \note field's table will be set with table's alias if defined, name else
     */
    void addSelectField(const Table & table, SelectField field);

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
    Table pvTable;
    QVector<SelectField> pvSelectFieldList;
    
    QString pvTableName;
    QString pvTableAlias;
    SelectSuffix pvSelectSuffix;
    QStringList pvSelectList;
    QList<mdtSqlViewSchemaJoinClause> pvJoinClauseList;
  //   QString pvAfterSelectStatement;
  };

};  // namespace mdtSqlViewSchema

#endif // #ifndef MDT_SQL_VIEW_SCHEMA_H
