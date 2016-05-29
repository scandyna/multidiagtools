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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_LIST_MODEL_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_LIST_MODEL_H

#include "FieldTypeList.h"
#include <QAbstractListModel>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief FieldType list model
   *
   * The main goal of FieldTypeListModel is to let the user select
   *  a field type in a user interface.
   *
   * Typical usage with a QComboBox:
   * \code
   * using Mdt::Sql::Schema::FieldType;
   *
   * Mdt::Sql::Schema::FieldTypeList fieldTypeList;
   * Mdt::Sql::Schema::FieldTypeListModel model;
   * QComboBox comboBox;
   *
   * // Populate model
   * fieldTypeList.append(FieldType::Boolean);
   * fieldTypeList.append(FieldType::Integer);
   * model.setFieldTypeList(fieldTypeList);
   * // Set comboBox model.
   * comboBox.setModel(&model);
   * // Some more code...
   * ...
   * // Get field type that the user selected
   * auto fieldType = model.fieldType(comboBox.currentIndex());
   * \endcode
   */
  class FieldTypeListModel : public QAbstractListModel
  {
   Q_OBJECT

   public:

    /*! \brief Create a empty FieldTypeListModel
     */
    FieldTypeListModel(QObject* parent = nullptr);

    /*! \brief Set field type list
     *
     * A list of available field types can be retrieved from a Driver
     *
     * \sa Driver::getAvailableFieldTypeList()
     */
    void setFieldTypeList(const FieldTypeList & ftl);

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override
    {
      if(parent.isValid()){
        return 0;
      }
      return pvFieldTypeList.size();
    }

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Get field type at given row
     *
     * If row is out of bound,
     *  UnknownType is returned.
     */
    FieldType fieldType(int row);
    
   private:

    FieldTypeList pvFieldTypeList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_LIST_MODEL_H
