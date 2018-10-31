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
#ifndef MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_MAP_H
#define MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_MAP_H

#include "WidgetEditableProperty.h"
#include <QWidget>
#include <QByteArray>
#include <QMetaType>
#include <memory>

namespace Mdt{ namespace ItemEditor{

  class WidgetEditablePropertyMapImpl;

  /*! \brief Provides unified access to editable property of widgets
   */
  class WidgetEditablePropertyMap
  {
   public:

    /*! \brief Constructor
     *
     * Will also set editable property name for Qt's known widgets
     *  that have edition capabality.
     */
    explicit WidgetEditablePropertyMap();

    /*! \brief Destructor
     */
    ~WidgetEditablePropertyMap();

    /*! \brief Set editable property for a widget of type T
     *
     * If a editable property name was allaready set for type T ,
     *  it will be replaced with \a editablePropertyName .
     *
     * If the widget is enabled while its property name is false,
     *  set \a reversed to true.
     *  This is for example the case for QLineEdit,
     *  which has readOnly property (which must be false so that editing is possible).
     *
     * \pre \a T be a subclass of QWidget
     * \pre \a editablePropertyName must be a non empty null terminated string
     */
    template<typename T>
    void setEditableProperty(const char * const editablePropertyName, bool reversed)
    {
      Q_ASSERT(editablePropertyName != nullptr);
      setEditableProperty( classNameForT<T>(), editablePropertyName, reversed );
    }

    /*! \brief Get editable property for a widget of type T
     *
     * Returns the editable property for a widget of type T if it was set,
     *  otherwise a null one.
     *
     * \note This method requires that type T exactly matches the type that was used when editable property was set.
     *        For example, QAbstractSpinBox has a readOnly property.
     *        QSpinBox inherits QAbstractSpinBox, but the readOnly property will not be found here.
     *        For such case, considere using getEditableProperty(const QWidget * const) const
     *
     * \pre \a T be a subclass of QWidget
     */
    template<typename T>
    WidgetEditableProperty getEditableProperty() const
    {
      return getEditableProperty( classNameForT<T>() );
    }

    /*! \brief Get editable property for a widget
     *
     * Returns the editable property for class name of \a widget if it was set,
     *  otherwise a empty one.
     *
     * \pre \a widget must be a valid pointer
     */
    WidgetEditableProperty getEditableProperty(const QWidget * const widget) const;

    /*! \brief Set a widget editable
     *
     * If a property name was defined for \a widget's class name,
     *  it will use it to set it editable or not regarding \a editable .
     *
     * Return true if a valid property name was set for \a widget ,
     *  otherwise false.
     *
     * \pre \a widget must be a valid pointer
     */
    bool setWidgetEditable(QWidget * const widget, bool editable) const;

   private:

    /*! \brief Set editable property for a widget class name
     *
     * If a editable property name was allaready set for \a className ,
     *  it will be replaced with \a editablePropertyName .
     *
     * If the widget is enabled while its property name is false,
     *  set \a reversed to true.
     *  This is for example the case for QLineEdit,
     *  which has readOnly property (which must be false so that editing is possible).
     *
     * \pre \a className must be a non empty null terminated string
     * \pre \a editablePropertyName must be a non empty null terminated string
     */
    void setEditableProperty(const char * const className, const char * const editablePropertyName, bool reversed);

    /*! \brief Add editable property for a widget class name without checking it exists
     */
    template<typename T>
    void addEditableProperty(const char * const editablePropertyName, bool reversed)
    {
      Q_ASSERT(editablePropertyName != nullptr);
      addEditableProperty( classNameForT<T>(), editablePropertyName, reversed );
    }

    /*! \brief Add editable property for a widget class name without checking it exists
     */
    void addEditableProperty(const char * const className, const char * const editablePropertyName, bool reversed);

    /*! \brief Get editable property for a widget class name
     *
     * Returns the editable property for \a className if it was set,
     *  otherwise a null one.
     *
     * \pre \a className must be a non empty null terminated string
     */
    WidgetEditableProperty getEditableProperty(const char * const className) const;

    template<typename T>
    const char *classNameForT() const
    {
      return classNameForMetaTypeId( qMetaTypeId<T*>() );
    }
    const char *classNameForMetaTypeId(int id) const;
    void populateWithQtWidgets();

    static std::unique_ptr<WidgetEditablePropertyMapImpl> mImpl;
    static int mInstanceCount;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_WIDGET_EDITABLE_PROPERTY_MAP_H
