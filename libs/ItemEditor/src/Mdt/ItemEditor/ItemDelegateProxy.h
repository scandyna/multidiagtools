/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ITEM_DELEGATE_PROXY_H
#define MDT_ITEM_EDITOR_ITEM_DELEGATE_PROXY_H

#include <QAbstractItemDelegate>
#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Item delegate proxy
   *
   * For some cases, it can be required to create a item delegate
   *  to catch some event, or do some very common processing.
   *
   * The possible way is to implement a delegate based on, for.ex.
   *  QStyledItemDelegate.
   *  The problem here is that each user delegate must also
   *  inherit from this class.
   *
   * By using a delegate proxy, it is possible to use
   *  delegates based on QAbstractItemDelegate .
   *
   * Typical usage:
   * \code
   * QTableView view;
   * auto *delegate = new MyDelegate(&view);
   * auto *delegateProxy = new ItemDelegateProxy(&view);
   *
   * delegateProxy->setItemDelegate(delegate);
   * view.setItemDelegate(delegateProxy);
   * \endcode
   */
  class ItemDelegateProxy : public QAbstractItemDelegate
  {
   public:

    /*! \brief Constructor
     */
    explicit ItemDelegateProxy(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~ItemDelegateProxy();

    // Copy disabled
    ItemDelegateProxy(const ItemDelegateProxy &) = delete;
    ItemDelegateProxy & operator=(const ItemDelegateProxy &) = delete;
    // Move disabeld
    ItemDelegateProxy(ItemDelegateProxy &&) = delete;
    ItemDelegateProxy & operator=(ItemDelegateProxy &&) = delete;

    /*! \brief Set item delegate
     *
     * Previously set delegate is replaced with delegate,
     *  but not deleted.
     *  ItemDelegateProxy does not take ownership of delegate.
     */
    void setItemDelegate(QAbstractItemDelegate * delegate);

    /*! \brief Get item delegate
     */
    QAbstractItemDelegate * itemDelegate() const;

    /*! \brief Create editor
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

    /*! \brief Destroy editor
     */
    void destroyEditor(QWidget *editor, const QModelIndex & index) const override;

    /*! \brief Process editor events
     */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem & option, const QModelIndex & index) override;

    /*! \brief Process help event
     */
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem & option, const QModelIndex & index) override;

    /*! \brief Paint
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /*! \brief Set editor data
     */
    void setEditorData(QWidget *editor, const QModelIndex & index) const override;

    /*! \brief Set model data
     */
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const override;

    /*! \brief Get size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /*! \brief Update editor geometry
     */
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

   private:

    QPointer<QAbstractItemDelegate> pvDelegate;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_DELEGATE_PROXY_H
