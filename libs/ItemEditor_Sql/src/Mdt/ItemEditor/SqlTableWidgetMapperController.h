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
#ifndef MDT_ITEM_EDITOR_SQL_TABLE_WIDGET_MAPPER_CONTROLLER_H
#define MDT_ITEM_EDITOR_SQL_TABLE_WIDGET_MAPPER_CONTROLLER_H

#include "AbstractSqlTableModelController.h"

class QWidget;

namespace Mdt{ namespace ItemEditor{

  class DataWidgetMapper;

  /*! \brief Controller for DataWidgetMapper to act on a QSqlTableModel
   */
  class SqlTableWidgetMapperController : public AbstractSqlTableModelController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SqlTableWidgetMapperController(QObject* parent = nullptr);

    // Copy disabled
    SqlTableWidgetMapperController(const SqlTableWidgetMapperController &) = delete;
    SqlTableWidgetMapperController & operator=(const SqlTableWidgetMapperController &) = delete;
    // Move disabled
    SqlTableWidgetMapperController(SqlTableWidgetMapperController &&) = delete;
    SqlTableWidgetMapperController & operator=(SqlTableWidgetMapperController &&) = delete;

    /*! \brief Get widget mapper
     */
    DataWidgetMapper *widgetMapper() const
    {
      return mWidgetMapper;
    }

    /*! \brief Adds a mapping between a widget and a column from the model
     *
     * For more informations, see DataWidgetMapper documentation.
     *
     * \pre widget pointer must be valid
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

    /*! \brief Set model to the view
     */
    void setModelToView(QAbstractItemModel *model) override;

    /*! \brief Submit data to model
     *
     * Tell widget mapper to submit data to model.
     */
    bool setDataToModel() override;

    /*! \brief Revert data from model
     */
    void revertDataFromModel() override;

    DataWidgetMapper *mWidgetMapper;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_SQL_TABLE_WIDGET_MAPPER_CONTROLLER_H
