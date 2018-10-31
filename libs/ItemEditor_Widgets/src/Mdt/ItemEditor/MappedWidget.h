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
#ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_H
#define MDT_ITEM_EDITOR_MAPPED_WIDGET_H

#include "MdtItemEditor_WidgetsExport.h"
#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QVariant>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Uniform interface for DataWidgetMapper
   */
  class MDT_ITEMEDITOR_WIDGETS_EXPORT MappedWidget : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Contructor
     *
     * \pre \a widget must be a valid pointer
     * \pre \a column must be >= 0
     */
    explicit MappedWidget(QWidget *widget, int column);

    /*! \brief Get widget
     */
    QWidget *widget() const
    {
      return mWidget;
    }

    /*! \brief Get column
     */
    int column() const
    {
      return mColumn;
    }

    /*! \brief Set a value to the widget
     */
    virtual void setWidgetValue(const QVariant & value) = 0;

    /*! \brief Get widget's value
     */
    virtual QVariant widgetValue() const = 0;

    /*! \brief Set the widget editable
     */
    virtual void setWidgetEditable(bool editable) = 0;

    /*! \brief Set the widget enabled
     */
    virtual void setWidgetEnabled(bool enable);

   signals:

    /*! \brief Emitted when the widget notified edition started
     */
    void editionStarted();

   private:

    QPointer<QWidget> mWidget;
    int mColumn;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_H
