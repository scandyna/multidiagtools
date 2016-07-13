/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_WIDGET_H
#define MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_WIDGET_H

#include "AbstractEditorWidget.h"

class QDataWidgetMapper;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Common base class for widget that use a QDataWidgetMapper
   *
   * This class only handles some common stuff,
   *  but not owns any controller.
   *
   * To create a editor that uses a QDataWidgetMapper,
   *  it is recommanded to subclass WidgetMapperWidget.
   */
  class AbstractWidgetMapperWidget : public AbstractEditorWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractWidgetMapperWidget(QWidget* parent = nullptr);

   protected slots:

    /*! \brief Update model
     *
     * This slot is called from controller.
     *
     * Subclass that owns a controller
     *  should connect AbstractController::modelChanged()
     *  to this slot.
     */
    void updateModel(QAbstractItemModel *model);

   private:

    QDataWidgetMapper *pvMapper;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_WIDGET_H
