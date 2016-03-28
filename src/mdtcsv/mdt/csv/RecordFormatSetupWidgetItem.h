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
#ifndef MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_ITEM_H
#define MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_ITEM_H

#include <QWidget>
#include <QString>
#include <QMetaType>

class QLabel;
class QComboBox;

namespace mdt{ namespace csv{

  class RecordFormatSetupWidgetItem : public QWidget
  {
   Q_OBJECT

   public:

    RecordFormatSetupWidgetItem(QWidget *parent = nullptr);

    void setFieldName(const QString & name);

    void setFieldType(QMetaType::Type type);

    QMetaType::Type fieldType() const;

   signals:

    /*! \brief Emitted when field type changed
     */
    void fieldTypeChanged();

   private slots:

    /*! \brief Called when cbFieldType index changed
     */
    void onFieldTypeChanged();

   private:

    QLabel *lbFieldName;
    QComboBox *cbFieldType;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_RECORD_FORMAT_SETUP_WIDGET_ITEM_H
