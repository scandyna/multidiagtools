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
#ifndef MDT_ITEM_EDITOR_WIDGET_MAPPER_CONTROLLER_H
#define MDT_ITEM_EDITOR_WIDGET_MAPPER_CONTROLLER_H

// #include "AbstractWidgetMapperController.h"
#include "AbstractController.h"
#include <QByteArray>

class QWidget;

namespace Mdt{ namespace ItemEditor{

  class DataWidgetMapper;

  /*! \brief Controller for DataWidgetMapper
   */
  class WidgetMapperController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit WidgetMapperController(QObject* parent = nullptr);

    /*! \brief Get widget mapper
     */
    DataWidgetMapper *widgetMapper() const
    {
      return mWidgetMapper;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre No widget must be mapped when setting a model. If widget has been mapped, call clearMapping() first.
     *       Which widget must be mapped with which column is only known from application developper,
     *       this is the reason why no automatic remapping is done when changing the model.
     * \pre model must be a valid pointer
     */
    void setModel(QAbstractItemModel *model) override;

    /*! \brief Adds a mapping between a widget and a column from the model
     *
     * For more informations, see DataWidgetMapper documentation.
     *
     * \pre model must be set before mapping any widget
     * \pre widget pointer must be valid
     * \sa setModel()
     */
    void addMapping(QWidget *widget, int column);

    /*! \brief Clear mapping
     *
     * After that call, all widgets that were mapped will no longer
     *  been handled by this controller.
     *
     * \note Mapped widgets are not owned by this controller,
     *        so thei will not be deleted.
     */
    void clearMapping();

   private:

    /*! \brief Submit data to model
     *
     * Tell widget mapper to submit data to model.
     */
    bool submitDataToModel() override;

    /*! \brief Revert data from model
     */
    void revertDataFromModel() override;

    DataWidgetMapper *mWidgetMapper;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_WIDGET_MAPPER_CONTROLLER_H
