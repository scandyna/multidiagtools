/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt. If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_H
#define MDT_ITEM_MODEL_HEADER_PROXY_MODEL_H

#include "HeaderProxyModelItemList.h"
#include "MdtItemModelExport.h"
#include <QIdentityProxyModel>
#include <QStringList>

namespace Mdt{ namespace ItemModel{

  /*! \brief HeaderProxyModel provides a simple way to set custom headers
   *
   * Common way to use Qt's item/view is to implement a item model,
   *  that let the user access relevant data.
   *  Such model also should implement header data access methodes.
   *  But, for some cases, it can be enouth to work with a existing model,
   *  such as VariantTableModel, that do not provide header data methods.
   *  In the latter case, HeaderProxyModel can be used.
   *
   * Typical usage:
   * \code
   * #include <Mdt/ItemModel/VariantTableModel.h>
   * #include <Mdt/ItemModel/HeaderProxyModel.h>
   * #include <QTableView>
   *
   * using namespace Mdt::ItemModel;
   *
   * // Setup model
   * VariantTableModel model;
   * model.populate(5, 2);
   * // Setup proxy model
   * HeaderProxyModel headerModel;
   * headerModel.setSourceModel(&model);
   * headerModel.setHorizontalHeaderLabels({"Id","Name"});
   * // Setup view
   * QTableView view;
   * view.setModel(&headerModel);
   * \endcode
   *
   * \note When you have to deal with several proxy models,
   *        ProxyModelContainer can be helpful.
   */
  class MDT_ITEMMODEL_EXPORT HeaderProxyModel : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit HeaderProxyModel(QObject* parent = nullptr);

    /*! \brief Get header data
     *
     * \sa horizontalHeaderData()
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get horizontal header data
     *
     * If some horizontal header data has been set,
     *  and \a column is in that bound,
     *  these header data will be returned.
     *  Else, if a source model has allready been set,
     *  the header data from this model will be returned.
     *  Else, a null QVariant is returned.
     *
     * \sa setHorizontalHeaderLabels()
     */
    QVariant horizontalHeaderData(int column, int role = Qt::DisplayRole) const;

    /*! \brief Set horizontal header labels
     */
    void setHorizontalHeaderLabels(const QStringList & labels);

   private:

    HeaderProxyModelItemList mHorizontalHeders;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_H
