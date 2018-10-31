/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_H
#define MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_H

#include "MdtItemEditor_WidgetsExport.h"
#include <QByteArray>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Unified editable property of a widget
   */
  class MDT_ITEMEDITOR_WIDGETS_EXPORT WidgetEditableProperty
  {
   public:

    /*! \brief Construct a null editable property
     */
    WidgetEditableProperty() = default;

    /*! \brief Construct a widget editable property
     *
     * \sa setEditableProperty()
     * \pre \a editablePropertyName must not be empty
     */
    WidgetEditableProperty(const QByteArray & editablePropertyName, bool reversed);

    /*! \brief Set editable property
     *
     * If the widget is enabled while its property name is false,
     *  set \a reversed to true.
     *  This is for example the case for QLineEdit,
     *  which has readOnly property (which must be false so that editing is possible).
     *
     * \pre \a editablePropertyName must not be empty
     */
    void setEditableProperty(const QByteArray & editablePropertyName, bool reversed);

    /*! \brief Get editable property name
     */
    QByteArray editablePropertyName() const
    {
      return mPropertyName;
    }

    /*! \brief Check if editable property is reversed
     */
    bool isReversed() const
    {
      return mIsReversed;
    }

    /*! \brief Check if this editable property is null
     */
    bool isNull() const
    {
      return mPropertyName.isEmpty();
    }

   private:

    QByteArray mPropertyName;
    bool mIsReversed = false;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_H
