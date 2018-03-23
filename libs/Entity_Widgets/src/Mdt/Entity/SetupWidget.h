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
#ifndef MDT_ENTITY_SETUP_WIDGET_H
#define MDT_ENTITY_SETUP_WIDGET_H

#include "MdtEntity_WidgetsExport.h"

class QLineEdit;
class QComboBox;
class QWidget;
class QVariant;

namespace Mdt{ namespace Entity{

  class FieldAttributes;

  /*! \brief Setup \a lineEdit regarding \a fieldAttributes
   */
  void MDT_ENTITY_WIDGETS_EXPORT setupWidget(QLineEdit * const lineEdit, const FieldAttributes & fieldAttributes);

  /*! \brief Setup \a comboBox regarding \a fieldAttributes
   *
   * If \a comboBox is not editable, not setup will be done.
   */
  void MDT_ENTITY_WIDGETS_EXPORT setupWidget(QComboBox * const comboBox, const FieldAttributes & fieldAttributes);

  /*! \brief Setup \a widget regarding \a fieldAttributes
   *
   * This implementation can be used when the exact editor is not known.
   *  It will use setWidgetProperty().
   */
  void MDT_ENTITY_WIDGETS_EXPORT setupWidget(QWidget * const widget, const FieldAttributes & fieldAttributes);

  /*! \brief Set the value of a property to a widget
   *
   * To set the value of a property to a widget,
   *  QObject::setProperty() is typically used.
   *  If a property does not exist for a widget,
   *  QObject::setProperty() will create a dynamic property,
   *  which is not the expected behaviour here.
   *
   * If a property is defined using Q_PROPERTY,
   *  it will be set and true is returned on success and false otherwise.
   */
  bool MDT_ENTITY_WIDGETS_EXPORT setWidgetProperty(QWidget * const widget, const char * const propertyName, const QVariant & propertyValue);

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SETUP_WIDGET_H
