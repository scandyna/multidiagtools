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
#ifndef MDT_ITEM_MODEL_PROXY_MODEL_CONTAINER_H
#define MDT_ITEM_MODEL_PROXY_MODEL_CONTAINER_H

#include <QAbstractProxyModel>
#include <QAbstractItemModel>
#include <QPointer>
#include <vector>
#include <algorithm>
#include <typeinfo>

namespace Mdt{ namespace ItemModel{

  /*! \brief Container for a chain of proxy models based on QAbstractProxyModel
   *
   * The common way to use a proxy model between a source model and a view looks like:
   * \code
   * auto *view = new QTableView;
   * auto *sourceModel = new MyItemModel(this);
   * auto *filterModel = new FilterProxyModel(this);
   *
   * filterModel->setSourceModel(sourceModel);
   * view->setModel(filterModel);
   * \endcode
   * To insert a format model, we could do:
   * \code
   * auto *formatModel = new FormatProxyModel(this);
   *
   * filterModel->setSourceModel(sourceModel);
   * formatModel->setSourceModel(filterModel);
   * view->setModel(formatModel);
   * \endcode
   *
   * If we have to handle a chain of proxy models,
   *  and be able to insert or remove proxy models,
   *  ProxyModelContainer can be helpful.
   *
   * Above example could look like:
   * \code
   * ProxyModelContainer container;
   * auto *view = new QTableView;
   *
   * container.setSourceModel( new MyItemModel(this) );
   * container.appendProxyModel( new FilterProxyModel(this) );
   * container.appendProxyModel( new FormatProxyModel(this) );
   * view->setModel( container.lastProxyModel() );
   * \endcode
   *
   * ProxyModelContainer does not own any model,
   *  source model and all proxy models will not be deleted
   *  when the container is destroyed.
   *  It is recommanded to use the QObject mechanism to handle lifetime of models
   *  (passing a parent to each one).
   */
  class ProxyModelContainer
  {
   public:

    /*! \brief Constructor
     */
    ProxyModelContainer() = default;

    // Disable copy
    ProxyModelContainer(const ProxyModelContainer &) = delete;
    ProxyModelContainer & operator=(const ProxyModelContainer &) = delete;
    // Disable move
    ProxyModelContainer(ProxyModelContainer &&) = delete;
    ProxyModelContainer & operator=(ProxyModelContainer &&) = delete;

    /*! \brief Set source model
     *
     * \pre \a model must be a valid pointer
     */
    void setSourceModel(QAbstractItemModel *model);

    /*! \brief Get source model
     *
     * Returns a nullptr until a model was set.
     *
     * \sa setSourceModel()
     */
    QAbstractItemModel *sourceModel() const
    {
      return mSourceModel;
    }

    /*! \brief Add a proxy model to the end
     *
     * \pre \a model must be a valid pointer
     */
    void appendProxyModel(QAbstractProxyModel *model);

    /*! \brief Add a proxy model to the begin
     *
     * \pre \a model must be a valid pointer
     */
    void prependProxyModel(QAbstractProxyModel *model);

    /*! \brief Get first proxy model
     *
     * The first proxy model is the nearest of the source model.
     *
     * \pre Cannot be called unless at least 1 proxy model was added the this container.
     */
    QAbstractProxyModel *firstProxyModel() const
    {
      Q_ASSERT(!mList.empty());
      return mList.front();
    }

    /*! \brief Get last proxy model
     *
     * The last proxy model is the nearest of the view.
     *
     * \pre Cannot be called unless at least 1 proxy model was added the this container.
     */
    QAbstractProxyModel *lastProxyModel() const
    {
      Q_ASSERT(!mList.empty());
      return mList.back();
    }

    /*! \brief Get count of proxy models
     */
    int proxyModelCount() const
    {
      return mList.size();
    }

    /*! \brief Get proxy model at index
     *
     * \pre \a index must be in valid range ( 0 <= index < proxyModelCount() )
     */
    QAbstractProxyModel *proxyModelAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < proxyModelCount());
      return mList[index];
    }

    /*! \brief Check if a proxy model of type T exists in this container
     */
    template<typename T>
    bool containsProxyModelOfType() const
    {
      return ( indexOfFirstProxyModelOfType<T>() >= 0 );
    }

    /*! \brief Get the index of the first proxy model that is of type T
     *
     * \return The index of the proxy model if found, else -1
     */
    template<typename T>
    int indexOfFirstProxyModelOfType() const
    {
      auto pred = [](QAbstractProxyModel *model)
      {
        return (typeid(*model) == typeid(T));
      };
      const auto it = std::find_if(mList.cbegin(), mList.cend(), pred);
      if(it == mList.cend()){
        return -1;
      }
      return it - mList.cbegin();
    }

    /*! \brief Get the first proxy model that is of type T
     *
     * \return The proxy model if found, else a nullptr
     */
    template<typename T>
    QAbstractProxyModel *firstProxyModelOfType() const
    {
      int i = indexOfFirstProxyModelOfType<T>();
      if(i < 0){
        return nullptr;
      }
      Q_ASSERT(i < proxyModelCount());
      return mList[i];
    }

    /*! \brief Remove proxy model at index
     *
     * Note that the model is only removed from this container,
     *  but not deleted.
     *
     * \pre \a index must be in valid range ( 0 <= index < proxyModelCount() )
     */
    void removeProxyModelAt(int index);

    /*! \brief Delete proxy model at index
     *
     * Will remove and delete the proxy model at index.
     *
     * \pre \a index must be in valid range ( 0 <= index < proxyModelCount() )
     */
    void deleteProxyModelAt(int index);

    /*! \brief Remove the first proxy model of type T
     *
     * Note that the model is only removed from this container,
     *  but not deleted.
     *
     * Will do nothing if not proxy model of type T exists in this container.
     */
    template<typename T>
    void removeFirstProxyModelOfType()
    {
      int index = indexOfFirstProxyModelOfType<T>();
      if(index >= 0){
        removeProxyModelAt(index);
      }
    }

    /*! \brief Delete the first proxy model of type T
     *
     * Will remove and delete the proxy model.
     *
     * Will do nothing if not proxy model of type T exists in this container.
     */
    template<typename T>
    void deleteFirstProxyModelOfType()
    {
      int index = indexOfFirstProxyModelOfType<T>();
      if(index >= 0){
        deleteProxyModelAt(index);
      }
    }

   private:

    void updateSourceModelForProxyModelAt(int index);
    void updateSourceModel(QAbstractProxyModel *proxyModel, QAbstractItemModel *sourceModel);

    QPointer<QAbstractItemModel> mSourceModel;
    std::vector< QPointer<QAbstractProxyModel> > mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_PROXY_MODEL_CONTAINER_H
