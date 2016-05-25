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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_MODEL_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_INFO_MODEL_H

#include "FieldTypeInfoList.h"
#include <QAbstractListModel>

namespace Mdt{ namespace Sql{ namespace Schema{

  class Driver;

  /*! \brief FieldTypeInfo list model
   *
   * The main goal of FieldTypeInfoModel is to let the user select
   *  a field type in a user interface.
   *
   * Typical usage with a QComboBox:
   * \code
   * QSqlDatabase db = ...
   * Mdt::Sql::Schema::Driver driver(db);
   * Mdt::Sql::Schema::FieldTypeInfoModel model;
   * QComboBox comboBox;
   *
   * // Set driver to model (will populate model with available field types)
   * model.setDriver(driver);
   * // Set comboBox model.
   * comboBox.setModel(&model);
   * // Some more code...
   * ...
   * // Get field type that the user selected
   * auto fieldType = model.fieldType(comboBox.currentIndex());
   * \endcode
   */
  class FieldTypeInfoModel : public QAbstractListModel
  {
   Q_OBJECT

   public:

    /*! \brief Create a empty FieldTypeInfoModel
     */
    FieldTypeInfoModel(QObject* parent = nullptr);

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override
    {
      if(parent.isValid()){
        return 0;
      }
      return pvList.size();
    }

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set driver
     *
     * Will populate model with driver's available field types
     *
     * \pre driver must be valid
     */
    void setDriver(const Driver & driver);

   private:

    FieldTypeInfoList pvList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_MODEL_H
