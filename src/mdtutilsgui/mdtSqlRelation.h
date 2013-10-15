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
#ifndef MDT_SQL_RELATION_H
#define MDT_SQL_RELATION_H

#include <QObject>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QString>
#include <QList>
#include <QSqlRecord>

class mdtSqlRelationItem;

/*! \brief Handle parent/child relations between 2 QSqlTableModels
 *
 * This is usefull to create data/details forms.
 *
 * Typical usage:
 *  - Set the parent and child models, using setParentModel() and setChildModel()
 *  - Add parent/child tables fields that must match with addRelation()
 *  - Connect the parent view's currentXyChanged() signal to setParentCurrentIndex() slot.
 *     If you use a QTableView, this signal is in its selectionModel(). For QDataWidgetMapper,
 *     the currentIndexChanged() signal is directly available.
 *  - If a QDataWidgetMapper is used directly on a child model, connect childModelFilterApplied()
 *     signal to QDataWidgetMapper's toFirst() slot.
 */
class mdtSqlRelation : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Create a empty relation
   */
  mdtSqlRelation(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlRelation();

  /*! \brief Set the parent model
   *
   * Will be used internally to get some informations about
   *  fields and get values to generate relation filters.
   *
   * \pre model must be a valid pointer.
   */
  void setParentModel(QSqlTableModel *model);

  /*! \brief Set the child model
   *
   * Will be used internally to get some informations about
   *  fields and get values to generate relation filters.
   *
   * \pre model must be a valid pointer.
   */
  void setChildModel(QSqlTableModel *model);

  /*! \brief Add a relation
   *
   * The parentFieldName is a field name from the parent table (typically a field from primary key).
   *  The childFieldName is a field from child table (typically a field from foreing keys).
   *
   * Note that this method will fetch some information about fields in parent and
   *  child model (data type, field index, ...), so parent and child models must be set before.
   *
   * \param parentFieldName Field of parent model to use in relation (usually PK)
   * \param childFieldName Field of child model to use in relation (usually FK)
   * \param copyParentToChildOnInsertion If true, data from parent model will be copied
   *                                      to child model for this fields.
   *                                      (See mdtSqlRelationItem for details).
   * \param operatorWithPreviousItem Most of cases, the operator between multiple relation items is AND.
   *                     If another is needed (f.ex. OR), specify it here.
   * \return True on succes, false if a field could not be found.
   * \pre Parent model must be set with setParentModel() before using this method.
   * \pre Child model must be set with setChildModel() before using this method.
   */
  bool addRelation(const QString &parentFieldName, const QString &childFieldName, bool copyParentToChildOnInsertion, const QString &operatorWithPreviousItem = "AND");

  /*! \brief Clear relations
   *
   * Will clear all added relations and references to parent and child models.
   *  (will not delete models).
   */
  void clear();

 signals:

  /*! \brief This signal is emitted once setFilter() was called on child model
   *
   * This can be usefull when update must be done after child model was updated.
   *  For example, when using a QDataWidgetMapper, connect this signal to
   *  QDataWidgetMapper's toFirst() slot.
   */
  void childModelFilterApplied();

  /*! \brief This signal is emitted after childModelFilterApplied() if child model becomes empty
   *
   * Connect this signal to QDataWidgetMapper's revert() slot.
   *  QDataWidgetMapper will not clear widgets if model becomes empty,
   *  so using this signal is a workaround..
   */
  void childModelIsEmpty();

 public slots:

  /*! \brief Update child model
   *
   * \pre Parent model must be set with setParentModel() before using this method.
   * \pre Child model must be set with setChildModel() before using this method.
   */
  void setParentCurrentIndex(int index);

  /*! \brief Update child model
   *
   * \pre Parent model must be set with setParentModel() before using this method.
   * \pre Child model must be set with setChildModel() before using this method.
   */
  void setParentCurrentIndex(const QModelIndex &index);

  /*! \brief Update child model
   *
   * \pre Parent model must be set with setParentModel() before using this method.
   * \pre Child model must be set with setChildModel() before using this method.
   */
  void setParentCurrentIndex(const QModelIndex &current, const QModelIndex &previous);

 private slots:

  /*! \brief Tasks to be done before inserting child record into database
   *
   * This will call setChildForeingKeyValues().
   *
   * \pre Parent model must be set with setParentModel() and child model with setChildModel() before using this method.
   */
  void onChildBeforeInsert(QSqlRecord &childRecord);

 private:

  /*! \brief Copy the parent primary key's values to related foreing key in child model
   *
   * \pre Parent model must be set with setParentModel() and child model with setChildModel() before using this method.
   */
  void setChildForeingKeyValues(QSqlRecord &parentRecord, QSqlRecord &childRecord);

  /*! \brief Generate filter for relation
   */
  void generateChildModelRelationFilter(int row);

  /*! \brief Generate filter with relation and user filetr
   */
  void generateChildModelFilter();

  Q_DISABLE_COPY(mdtSqlRelation);

  QList<mdtSqlRelationItem*> pvRelations;
  QSqlTableModel *pvParentModel;
  QSqlTableModel *pvChildModel;
  QString pvChildModelUserFilter;
  QString pvChildModelRelationFilter;
  QString pvChildModelFilter;
  int pvCurrentRow;
};

#endif  // #ifndef MDT_SQL_RELATION_H
