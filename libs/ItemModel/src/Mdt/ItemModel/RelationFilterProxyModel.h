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
#ifndef MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_H
#define MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_H

#include "RelationFilterExpression.h"
#include "ParentModelColumn.h"
#include "FilterColumn.h"
#include "LikeExpression.h"

#include <QSortFilterProxyModel>

#include <QPointer>
#include <memory>

namespace Mdt{ namespace ItemModel{

  class RelationKeyCopier;

  /*! \brief Provides support for filtering data between another model and a view
   *
   * Example of usage:
   * \code
   * #include <Mdt/ItemModel/RelationFilterProxyModel.h>
   * #include <QTableView>
   * #include "ClientTableModel.h"
   * #include "AddressTableModel.h"
   *
   * namepsace ItemModel = Mdt::ItemModel;
   * using ItemModel::RelationFilterProxyModel;
   * using ItemModel::ParentModelColumn;
   *
   * auto *view = new QTableView;
   * auto *clientModel = new ClientTableModel;
   * auto *addressModel = new AddressTableModel(view);
   * auto *proxyModel = new RelationFilterProxyModel(view);
   *
   * // Setup view act on addressModel
   * proxyModel->setParentModel(clientModel);
   * proxyModel->setSourceModel(addressModel);
   * view->setModel(proxyModel);
   *
   * // Setup a filter and apply it
   * ParentModelColumn clientId(0);
   * proxyModel->setFilter( cliendId == addressClientId );
   * proxyModel->setParentModelMatchRow(0);
   * \endcode
   */
  class RelationFilterProxyModel : public QSortFilterProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Construct a model
     */
    explicit RelationFilterProxyModel(QObject* parent = nullptr);

    // Destructor - unique_ptr needs to know how to destruct RelationKeyCopier
    ~RelationFilterProxyModel();
    // Disable copy
    RelationFilterProxyModel(const RelationFilterProxyModel &) = delete;
    RelationFilterProxyModel & operator=(const RelationFilterProxyModel &) = delete;
    // Disable move
    RelationFilterProxyModel(RelationFilterProxyModel &&) = delete;
    RelationFilterProxyModel & operator=(RelationFilterProxyModel &&) = delete;

    /*! \brief Set parent model
     *
     * \pre \a model must be a valid pointer
     */
    void setParentModel(QAbstractItemModel *model);

    /*! \brief Get parent model
     */
    QAbstractItemModel *parentModel() const
    {
      return mParentModel;
    }

    /*! \brief Set relation filter
     *
     * \param expression Expression to apply as filter.
     * \pre \a expression must not be null
     *
     * \sa setParentModelMatchRow()
     */
    void setFilter(const RelationFilterExpression & expression);

    /*! \brief Get row of parent model for which filter must match
     */
    int parentModelMatchRow() const
    {
      return mParentModelRow;
    }

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
    void setDynamicSortFilter(bool enable);

    /*! \brief Reimplemented from QSortFilterProxyModel
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

   public slots:

    /*! \brief Set row of parent model for which filter must match
     *
     * If row is out of bound, filter will allways evaluate false,
     *  and this proxy model will present no data.
     *
     * \sa setFilter()
     */
    void setParentModelMatchRow(int row);

   private slots:

    /*! \brief Actions to perform when rows have been inserted into this proxy model
     */
    void onRowsInserted(const QModelIndex & parent, int first, int last);

   private:

    /*! \brief Return true if filter expression was set and evaluates true
     */
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    int mParentModelRow = -1;
    bool mInserting = false;
    QPointer<QAbstractItemModel> mParentModel;
    RelationFilterExpression mFilterExpression;
    std::unique_ptr<RelationKeyCopier> mKeyCopier;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_H
