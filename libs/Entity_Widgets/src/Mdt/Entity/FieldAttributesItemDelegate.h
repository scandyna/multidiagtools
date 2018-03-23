/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_FIELD_ATTRIBUTES_ITEM_DELEGATE_H
#define MDT_ENTITY_FIELD_ATTRIBUTES_ITEM_DELEGATE_H

#include "SetupWidget.h"
#include "Mdt/Entity/TypeTraits/IsEntity.h"
#include "Mdt/Entity/FieldAttributes.h"
#include "Mdt/Entity/FieldAt.h"
#include <QStyledItemDelegate>

namespace Mdt{ namespace Entity{

  /*! \brief Item delegate to handle some field attributes
   *
   * If a entity was defined using MDT_ENTITY_DEF(),
   *  setup a view using FieldAttributesItemDelegate will look like:
   * \code
   * QTableView view;
   * MyEntityTableModel model;
   * Mdt::Entity:FieldAttributesItemDelegate<MyEntity> delegate;
   *
   * view.setItemDelegate(&delegate);
   * view.setModel(&model);
   * \endcode
   */
  template<typename Entity>
  class FieldAttributesItemDelegate : public QStyledItemDelegate
  {
    static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

   public:

    using entity_def_type = typename Entity::def_type;

    /*! \brief Constructor
     */
    explicit FieldAttributesItemDelegate(QObject *parent = nullptr)
     : QStyledItemDelegate(parent)
    {
    }

    /*! \brief Reimplemented from QStyledItemDelegate::createEditor()
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {
      auto *editor = QStyledItemDelegate::createEditor(parent, option, index);

      if(editor == nullptr){
        return editor;
      }
      if(!index.isValid()){
        return editor;
      }
      setupWidget(editor, fieldAttributesAtIndex(index) );

      return editor;
    }

    /*! \brief Get field attributes for \a column
     *
     * \pre \a column must be in valid range ( 0 <= \a column < fieldCount<Entity>() ).
     */
    static const FieldAttributes fieldAttributesAtColumn(int column) noexcept
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < fieldCount<entity_def_type>());

      return fieldAttributesAt(entity_def_type{}, column);
    }

    /*! \brief Get field attributes for \a index
     */
    static const FieldAttributes fieldAttributesAtIndex(const QModelIndex & index) noexcept
    {
      Q_ASSERT(index.isValid());

      return fieldAttributesAtColumn(index.column());
    }

  };

}}

#endif // #ifndef MDT_ENTITY_FIELD_ATTRIBUTES_ITEM_DELEGATE_H
