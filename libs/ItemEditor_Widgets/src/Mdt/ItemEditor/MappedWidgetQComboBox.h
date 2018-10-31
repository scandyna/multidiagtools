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
#ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_Q_COMBO_BOX_H
#define MDT_ITEM_EDITOR_MAPPED_WIDGET_Q_COMBO_BOX_H

#include "MappedWidget.h"
#include "MdtItemEditor_WidgetsExport.h"
#include <QComboBox>

namespace Mdt{ namespace ItemEditor{

  class MDT_ITEMEDITOR_WIDGETS_EXPORT MappedWidgetQComboBox : public MappedWidget
  {
   public:

    /*! \brief Contructor
     *
     * \pre \a comboBox must be a valid pointer
     * \pre \a column must be >= 0
     */
    explicit MappedWidgetQComboBox(QComboBox *comboBox, int column);

    /*! \brief Set a value to the widget
     */
    void setWidgetValue(const QVariant & value) override;

    /*! \brief Get widget's value
     */
    QVariant widgetValue() const override;

    /*! \brief Set the widget editable
     */
    void setWidgetEditable(bool editable) override;

    /*! \brief Set the widget enabled
     */
    void setWidgetEnabled(bool enable) override;

   private:

    QComboBox *comboBox() const;
    bool canEnableWidget() const;

    bool mEnabled;
    bool mEditable = true;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_Q_COMBO_BOX_H
