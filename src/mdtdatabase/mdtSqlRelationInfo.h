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

  /*! \brief Relation type
   */
  enum relationType_t {
                OneToMany = 0,  /*!< One to many relation */
                OneToOne        /*!< One to one relation.
                                      In 1-1 relation, inserting a record in parent model will also
                                       insert one in child model. When parent model is saved,
                                       child model is updated with parent model's primary key.
                                       When removing a record in parent model, related record
                                       (if exists) will be removed first in child model.
                                 */
  };

  /*! \brief Constructor
   *
   * Will set relationType to OneToMany.
   */
  mdtSqlRelationInfo() : pvRelationType(OneToMany) {}

  /*! \brief Set relation type
   */
  void setRelationType(relationType_t type);

  /*! \brief Get relation type
   */
  inline relationType_t relationType() const { return pvRelationType; }

  /*! \brief Set parent table name
   */
  void setParentTableName(const QString & tableName) { pvParentTableName = tableName; }

  /*! \brief Get parent table name
   */
  inline QString parentTableName() const { return pvParentTableName; }

  /*! \brief Set child table name
   */
  void setChildTableName(const QString & tableName) { pvChildTableName = tableName; }

  /*! \brief Get child table name
   */
  inline QString childTableName() const { return pvChildTableName; }

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

  /*! \brief Clear
   */
  void clear();

 private:

  QString pvParentTableName;
  QString pvChildTableName;
  QList<mdtSqlRelationInfoItem> pvItems;
  relationType_t pvRelationType;
};

#endif // #ifndef MDT_SQL_RELATION_INFO_H
