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
#ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_ITEM_DELEGATE_H
#define MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVariant>
#include <QMetaType>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Item delegate used by DataWidgetMapper
   */
  class DataWidgetMapperItemDelegate : public QStyledItemDelegate
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit DataWidgetMapperItemDelegate(QObject* parent = nullptr);

    /*! \brief Set model data
     *
     * Get the value from \a editor using its user property.
     */
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

   private:

    static QVariant convertData(const QVariant & data);
    static QVariant convertFromString(const QVariant & data);

    static QVariant displayValueForNullVariant(int editorValueType);
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_ITEM_DELEGATE_H
