/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SQL_RELATION_INFO_H
#define MDT_SQL_RELATION_INFO_H

#include <QString>
#include <QList>

/*! \brief Container for mdtSqlRelationInfo
 */
struct mdtSqlRelationInfoItem
{
  QString parentFieldName;
  QString childFieldName;
  bool copyParentToChildOnInsertion;
  QString relationOperatorWithPreviousItem;
};

/*! \brief Container for SQL relation informations
 */
class mdtSqlRelationInfo
{
 public:

  /*! \brief Constructor
   */
  mdtSqlRelationInfo() : pvCopyParentToChildOnInsertion(false) {}

  /*! \brief Set parent table name
   */
  void setParentTableName(const QString & tableName) { pvParentTableName = tableName; }

  /*! \brief Get parent table name
   */
  inline QString parentTableName() const { return pvParentTableName; }

  /*! \brief Set child table name
   */
  void setChildTableName(const QString & tableName) { pvParentTableName = tableName; }

  /*! \brief Get child table name
   */
  inline QString childTableName() const { return pvChildTableName; }

  /*! \brief Set copyParentToChildOnInsertion flag. 
   *
   * Usually, a parent/child relation is based on a primary key in parent table and foreing key (that refers to parent PK) in child model.
   *  When inserting a new record in child table, parent table's primary key must be copied into foreing key fields in child table.
   *  Set this flag to true to enable this behaviour (see mdtSqlRelation for more details).
   *
   * By default, this flag is false. 
   */
  void setCopyParentToChildOnInsertion(bool enable) { pvCopyParentToChildOnInsertion = enable; }

  /*! \brief Get copyParentToChildOnInsertion flag. 
   */
  inline bool copyParentToChildOnInsertion() const { return pvCopyParentToChildOnInsertion; }

  /*! \brief Add a relation
   *
   * The parentFieldName is a field name from the parent table (typically a field from primary key).
   *  The childFieldName is a field from child table (typically a field from foreing keys).
   *
   * \param parentFieldName Field of parent model to use in relation (usually PK)
   * \param childFieldName Field of child model to use in relation (usually FK)
   * \param copyParentToChildOnInsertion If true, data from parent model will be copied
   *                                      to child model for this fields.
   *                                      (See mdtSqlRelationItem for details).
   * \param operatorWithPreviousItem Most of cases, the operator between multiple relation items is AND.
   *                     If another is needed (f.ex. OR), specify it here.
   */
  void addRelation(const QString &parentFieldName, const QString &childFieldName, bool copyParentToChildOnInsertion, const QString &operatorWithPreviousItem = "AND");

  /*! \brief Get relation items list
   */
  inline const QList<mdtSqlRelationInfoItem> & items() const { return pvItems; }

 private:

  QString pvParentTableName;
  QString pvChildTableName;
  bool pvCopyParentToChildOnInsertion;
  QList<mdtSqlRelationInfoItem> pvItems;
};

#endif // #ifndef MDT_SQL_RELATION_INFO_H
