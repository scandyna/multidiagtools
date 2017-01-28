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
#include "RowRange.h"
#include <QObject>
#include <QPointer>
#include <QFlags>
#include <QModelIndex>

class QAbstractItemModel;
// class QModelIndex;

namespace Mdt{ namespace ItemModel{

  class PrimaryKey;
  class ForeignKey;

  /*! \brief Copies data from parent model to child model on some events regarding relation key
   *
   * \par Keep models and relation key coherent
   *  There is no good solution for RelationKeyCopier to check
   *   if relation key is valid, regarding models, in a reliable way.
   *   The user of this class should take care of this.
   *   When setting parent model, child model, and relation key,
   *   no check will be done about columns that are to big regarding models.
   *   Then processing the copy, out of bound check is done,
   *   and nothing happens for a pair of columns when at least one is in invalid range.
   *   This also alows to change models and relation key at any time.
   */
  class RelationKeyCopier : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Copy trigger
     */
    enum CopyTrigger
    {
      NoTrigger              = 0x00,  /*!< No copy will be made */
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
     * Default is NoTrigger
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

    /*! \brief Set ChildModelRowsInserted trigger enabled/disabled
     */
    void setChildModelRowsInsertedTriggerEnabled(bool enable);

    /*! \brief Check if ChildModelRowsInserted trigger is enabled
     */
    bool isChildModelRowsInsertedTriggerEnabled() const
    {
      return (mCopyTriggers & ChildModelRowsInserted);
    }

    /*! \brief Set parent model
     *
     * Will also set parent model current row to -1.
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setParentModel(QAbstractItemModel *model);

    /*! \brief Set child model
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setChildModel(QAbstractItemModel *model);

    /*! \brief Add a pair of columns
     *
     * \pre \a parentModelColumn must be >= 0
     * \pre \a childModelColumn must be >= 0
     */
    void addColumnPair(int parentModelColumn, int childModelColumn);

    /*! \brief Set relation key from parent model primary key and child model foreign key
     *
     * \pre parentModelPk must not be null
     * \pre childModelFk must not be null
     * \pre Both parentModelPk and childModelFk must have the same count of column indexes
     */
    void setKey(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk);

    /*! \brief Set relation key
     *
     * \pre \a key must not be null
     */
    void setKey(const RelationKey & key);

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

    /*! \brief Copy all key data from parent model to child model for given row range for child model
     */
    bool copyAllKeyData(RowRange childModelRowRange, const QModelIndex & parent = QModelIndex());

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
    bool copyKeyDataForRow(int childModelRow);

    /*! \brief Copy key data from parent model to child model
     */
    void copyKeyData(int childModelRow);

    /*! \brief Proceed to signal/slot connections regarding mCopyTriggers
     *
     * If a model is not valid when calling this method, it does nothing.
     */
//     void reconnectSignalSlotsIfOk();

    /*! \brief Clear key
     */
    void clearKey();

    int mParentModelCurrentRow = -1;
    RelationKey mKey;
    QPointer<QAbstractItemModel> mParentModel;
    QPointer<QAbstractItemModel> mChildModel;
    QMetaObject::Connection mChildModelRowsInsertedConnection;
    QMetaObject::Connection mParentModelDataChangedConnection;
    CopyTriggers mCopyTriggers = NoTrigger;
  };

}} // namespace Mdt{ namespace ItemModel{

Q_DECLARE_OPERATORS_FOR_FLAGS(Mdt::ItemModel::RelationKeyCopier::CopyTriggers)

#endif // #ifndef MDT_ITEM_MODEL_RELATION_KEY_COPIER_H
