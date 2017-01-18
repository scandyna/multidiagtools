/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_RELATION_KEY_COPIER_H
#define MDT_ITEM_MODEL_RELATION_KEY_COPIER_H

#include "RelationKey.h"
#include <QObject>
#include <QPointer>
#include <QFlags>

class QAbstractItemModel;
class QModelIndex;

namespace Mdt{ namespace ItemModel{

  class PrimaryKey;
  class ForeignKey;

  /*! \brief Copies data from parent model to child model on some events regarding relation key
   */
  class RelationKeyCopier : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Copy trigger
     */
    enum CopyTrigger
    {
      ChildModelRowsInserted = 0x01,  /*!< Copy occurs when rows have been inserted into child model */
      ParentModelDataChanged = 0x02   /*!< Copy occurs when data changes in a column in parent model that is part of the key.
                                            When implementing a module that acts on a database (for example Mdt::ItemEditor::AbstractSqlController),
                                            this flag should not be used, but better set DBMS's foreign key ON UPDATE option.
                                            \todo This is currently not implemented */
    };
    Q_DECLARE_FLAGS(CopyTriggers, CopyTrigger)

    /*! \brief Constructor
     */
    explicit RelationKeyCopier(QObject* parent = nullptr);

    /*! \brief Set copy triggers
     *
     * Default copy triggers is ChildModelRowsInserted
     *
     * \sa CopyTriggers
     */
    void setCopyTriggers(CopyTriggers triggers);

    /*! \brief Get copy triggers
     */
    CopyTriggers copyTriggers() const
    {
      return mCopyTriggers;
    }

    /*! \brief Set parent model
     *
     * Will also clear the key and set parent model current row to -1.
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setParentModel(QAbstractItemModel *model);

    /*! \brief Set child model
     *
     * Will also clear the key.
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setChildModel(QAbstractItemModel *model);

    /*! \brief Add a pair of columns
     *
     * \note key will be cleared if parent or child model changes
     * \pre parent model and child model must be set
     * \pre parentModelColumn must be in range 0 <= parentModelColumn < parent model's column count
     * \pre childModelColumn must be in range 0 <= childModelColumn < child model's column count
     */
    void addColumnPair(int parentModelColumn, int childModelColumn);

    /*! \brief Set rekation key from parent model primary key and child model foreign key
     *
     * \note key will be cleared if parent or child model changes
     * \pre parent model and child model must be set
     * \pre parentModelPk must not be null
     * \pre childModelFk must not be null
     * \pre Both parentModelPk and childModelFk must have the same count of fields
     * \pre Count of fields must be <= both parent and child model column count
     */
    void setKey(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk);

    /*! \brief Get relation key
     */
    RelationKey key() const
    {
      return mKey;
    }

    /*! \brief Get parent model current row
     */
    int parentModelCurrentRow() const
    {
      return mParentModelCurrentRow;
    }

   public slots:

    /*! \brief Set current row in parent model
     *
     * If parent model was not set, this slot does nothing.
     *
     * \pre If parent model is valid, row must be in range
     *       -1 <= row < parentModel->rowCount()
     */
    void setParentModelCurrentRow(int row);

   private slots:

    /*! \brief Actions to perform when rows have been inserted into the child model
     *
     * \pre mParentModel must not be null
     * \pre mChildModel must not be null
     */
    void onChildModelRowsInserted(const QModelIndex & parent, int first, int last);

    /*! \brief Actions to perform when data has changed in parent model
     *
     * \pre mParentModel must not be null
     * \pre mChildModel must not be null
     */
    void onParentModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles);

   private:

    /*! \brief Copy key data from parent model to child model
     */
    void copyKeyData(int childModelRow);

    /*! \brief Proceed to signal/slot connections regarding mCopyTriggers
     *
     * If a model is not valid when calling this method, it does nothing.
     */
    void reconnectSignalSlotsIfOk();

    /*! \brief Clear key
     */
    void clearKey();

    int mParentModelCurrentRow = -1;
    RelationKey mKey;
    QPointer<QAbstractItemModel> mParentModel;
    QPointer<QAbstractItemModel> mChildModel;
    QMetaObject::Connection mChildModelRowsInsertedConnection;
    QMetaObject::Connection mParentModelDataChangedConnection;
    CopyTriggers mCopyTriggers = ChildModelRowsInserted;
  };

}} // namespace Mdt{ namespace ItemModel{

Q_DECLARE_OPERATORS_FOR_FLAGS(Mdt::ItemModel::RelationKeyCopier::CopyTriggers)

#endif // #ifndef MDT_ITEM_MODEL_RELATION_KEY_COPIER_H
