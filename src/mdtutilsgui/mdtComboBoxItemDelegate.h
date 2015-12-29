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
#ifndef MDT_COMBO_BOX_ITEM_DELEGATE_H
#define MDT_COMBO_BOX_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPointer>

class QComboBox;
class QIcon;
class QStringList;
class QAbstractItemModel;

/*! \brief Item delegate that displays item as a combobox
 *
 * Typical usage:
 * \code
 *   QTableView *tableView = new QTableView;
 *   mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(tableView); // Give a parent to handle deletion of delegate
 *   // Populate the delegate with items that its internal combobox must display
 *   delegate->addItem("A");
 *   delegate->addItem("B");
 *   tableView->setModel(someUsefullModel);
 *   tableView->setItemDelegateForColumn(1, delegate); // Will use the combobox delegate only for column 1
 * \endcode
 */
class mdtComboBoxItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtComboBoxItemDelegate(QObject *parent = nullptr);

  /*! \brief Destructor
   */
  ~mdtComboBoxItemDelegate();

  /*! \brief Disable copy
   */
  mdtComboBoxItemDelegate(const mdtComboBoxItemDelegate & other) = delete;

  /*! \brief Add a item to the internal combobox
   *
   * See QComboBox::addItem() for details.
   */
  void addItem(const QString & text, const QVariant & userData = QVariant());

  /*! \brief Add a item to the internal combobox
   *
   * See QComboBox::addItem() for details.
   */
  void addItem(const QIcon & icon, const QString & text, const QVariant & userData = QVariant());

  /*! \brief Add items to the internal combobox
   *
   * See QComboBox::addItems() for details.
   */
  void addItems(const QStringList & texts);

  /*! \brief Set the model of the internal combobox
   *
   * \param model Set model that internal combobox must use to display its items.
   *           See QComboBox::setModel() for details.
   * \param modelColumn Column in model that internal combobox will display.
   *           See QComboBox::setModelColumn() for details.
   * \pre model must not be a nullptr
   */
  void setModel(QAbstractItemModel *model, int modelColumn = 0);

  /*! \brief Clear items of internal combobox
   *
   * See QComboBox::clear() for details.
   */
  void clear();

  /*! \brief Set combobox current index
   */
  void setCurrentIndex(int index);

  /*! \brief Get combobox current index
   */
  int currentIndex() const;

  /*! \brief Reimplemented to display a combobox like item
   */
//   void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

  /*! \brief Create editor
   */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;

  /*! \internal Overloaded to keep internal combo box
   */
  void destroyEditor(QWidget *editor, const QModelIndex & index) const;

  /*! \brief Set editor data
   */
  void setEditorData(QWidget *editor, const QModelIndex & index) const;

  /*! \brief Set model data
   */
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const;

  /*! \brief Update editor geometry
   */
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

//  private slots:
 
  /*! \internal Prevent double free if pvComboBox was parented
   */
//   void resetPvComboBoxPointer(QObject *obj);

 private:

  ///mutable QComboBox *pvComboBox;
  mutable QPointer<QComboBox> pvComboBox;
};

#endif // #ifndef MDT_COMBO_BOX_ITEM_DELEGATE_H
