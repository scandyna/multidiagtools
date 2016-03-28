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
#ifndef MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_H
#define MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_H

#include "mdtCsvData.h"
#include <QWidget>
#include <QString>
#include <QMetaType>
#include <vector>

class QHBoxLayout;
class QSignalMapper;

namespace mdt{ namespace csv{

  class RecordFormatSetupWidgetItem;

  /*! \brief Provide edition for CSV RecordFormat
   */
  class RecordFormatSetupWidget : public QWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    RecordFormatSetupWidget(QWidget *parent = nullptr);

    /*! \brief Set default field type
     */
    void setDefaultFieldType(QMetaType::Type type);

    /*! \brief Set header
     *
     * Create field type selection widgets
     *  and set its type to the one set with setDefaultFieldType()
     */
    void setHeader(const mdtCsvRecord & header);

   private slots:

    /*! \brief Called when a field type changed
     */
    void onFieldTypeChanged(int index);

   private:

    /*! \brief Set items count
     */
    void setItemsCount(int n);

    /*! \brief Add n items
     */
    void addItems(int n);

    /*! \brief Remove n items
     */
    void removeItems(int n);

    /*! \brief Set field name
     */
    void setFieldNameAt(int index, const QString & name);

    /*! \brief Set field type
     */
    void setFieldTypeAt(int index, QMetaType::Type type);

    QMetaType::Type pvDefaultFieldType;
    QHBoxLayout *pvLayout;
    std::vector<RecordFormatSetupWidgetItem*> pvItems;
    QSignalMapper *pvSignalMapper;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_H
