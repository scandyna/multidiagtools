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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_CONTROLLER_H

#include "AbstractController.h"
#include <QByteArray>

class QDataWidgetMapper;
class QWidget;

namespace Mdt{ namespace ItemEditor{

  class MappedWidgetList;

  /*! \brief Common base class to implement controllers for QDataWidgetMapper
   */
  class AbstractWidgetMapperController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractWidgetMapperController(QDataWidgetMapper *mapper, QObject* parent = nullptr);

    /*! \brief Get widget mapper
     */
    QDataWidgetMapper *widgetMapper() const
    {
      return pvWidgetMapper;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    void setModel(QAbstractItemModel *model) override;

    /*! \brief Adds a mapping between a widget and a section from the model
     *
     * For more informations, see QDataWidgetMapper documentation.
     */
    void addMapping(QWidget *widget, int section);

    /*! \brief Adds a mapping between a widget and a section from the model
     *
     * For more informations, see QDataWidgetMapper documentation.
     */
    void addMapping(QWidget *widget, int section, const QByteArray & propertyName);

   private:

    QDataWidgetMapper *pvWidgetMapper;
    MappedWidgetList *pvMappedWidgetList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_WIDGET_MAPPER_CONTROLLER_H
