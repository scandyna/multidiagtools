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
#ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_GENERIC_H
#define MDT_ITEM_EDITOR_MAPPED_WIDGET_GENERIC_H

#include "MappedWidget.h"
#include "WidgetEditableProperty.h"
#include "MdtItemEditor_WidgetsExport.h"
#include <QMetaProperty>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Generic implementation of MappedWidget
   *
   * This implementation can be used if no one handle the concrete editor directly.
   *
   * If the widget has a user property defined (see QObject documentations about properties),
   *  it will be used to set and get its value.
   *  If this user property also has a notify signal, it will be used
   *  to emit editionStarted() .
   *
   * \note The editionStarted() signal will be emitted when the value changes
   *   using setWidgetValue() as well as when the user edits.
   */
  class MDT_ITEMEDITOR_WIDGETS_EXPORT MappedWidgetGeneric : public MappedWidget
  {
   Q_OBJECT

   public:

    /*! \brief Contructor
     *
     * \pre \a widget must be a valid pointer
     * \pre \a column must be >= 0
     */
    explicit MappedWidgetGeneric(QWidget *widget, int column);

    /*! \brief Get widget editable property
     */
    WidgetEditableProperty widgetEditableProperty() const
    {
      return mWidgetEditableProperty;
    }

    /*! \brief Set a value to the widget
     */
    void setWidgetValue(const QVariant & value) override;

    /*! \brief Get widget's value
     */
    QVariant widgetValue() const override;

    /*! \brief Set the widget editable
     *
     * If the widget has a writable property named editable, readOnly or checkable,
     *  it will be used.
     */
    void setWidgetEditable(bool editable) override;

    /*! \brief Set the widget enabled
     */
    void setWidgetEnabled(bool enable) override;

   private:

    bool hasWidgetUserProperty() const;
    void setupWidgetEditableProperty();
    void connectWidgetToEditionStartedSignal();
    QMetaProperty getWidgetProperty(const char * const name);
    bool canEnableWidget() const;

    WidgetEditableProperty mWidgetEditableProperty;
    bool mEnabled;
    bool mEditable = true;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_GENERIC_H
