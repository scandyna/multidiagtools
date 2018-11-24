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
#ifndef MDT_ITEMEDITOR_MAPPED_WIDGET_RELATIONAL_QCOMBOBOX_H
#define MDT_ITEMEDITOR_MAPPED_WIDGET_RELATIONAL_QCOMBOBOX_H

#include "MappedWidgetQComboBox.h"
#include "MdtItemEditor_WidgetsExport.h"

namespace Mdt{ namespace ItemEditor{

  /*!\brief
    */
  class MappedWidgetRelationalQComboBox : public MappedWidgetQComboBox
  {
   Q_OBJECT

   public:

    /*! \brief Contructor
     *
     * \pre \a comboBox must be a valid pointer
     * \pre \a comboBox must have a model
     * \pre \a column must be >= 0
     * \pre \a comboBoxModelValueColumn must be in range of the comboBox model ( 0 <= \a comboBoxModelValueColumn < comboBox->model()->columnCount() )
     */
    explicit MappedWidgetRelationalQComboBox(QComboBox *comboBox, int column, int comboBoxModelValueColumn);

    /*! \brief Set a value to the widget
     */
    void setWidgetValue(const QVariant & value) override;

    /*! \brief Get widget's value
     */
    QVariant widgetValue() const override;

   private:

    int findRowForValue(const QVariant & value) const;

    int mComboBoxModelValueColumn;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // MDT_ITEMEDITOR_MAPPED_WIDGET_RELATIONAL_QCOMBOBOX_H
