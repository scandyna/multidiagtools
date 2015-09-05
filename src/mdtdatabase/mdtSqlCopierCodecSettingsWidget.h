/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_COPIER_CODEC_SETTINGS_WIDGET_H
#define MDT_SQL_COPIER_CODEC_SETTINGS_WIDGET_H

#include "ui_mdtSqlCopierCodecSettingsWidget.h"
#include "mdtSqlCopierCodecSettings.h"
///#include "mdtSqlCopierSqliteDatabaseTableCodecSettings.h"
#include <QWidget>
/// #include <memory>

/*! \brief Settings widget for SQL copier codec
 */
class mdtSqlCopierCodecSettingsWidget : public QWidget, Ui::mdtSqlCopierCodecSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCopierCodecSettingsWidget(QWidget *parent = nullptr);

  /*! \brief Set codec type
   */
  void setCodecType(mdtSqlCopierCodecSettings::CodecType type);

  /*! \brief Get codec settings
   */
  mdtSqlCopierCodecSettings codecSettings() const
  {
    return pvSettings;
  }

 private slots:

  /*! \brief Update widget after type was selected
   */
  void updateCodecType(int cbTypeIndex);

  /*! \brief Let user select database or file or whatever (depending on pvSettings codec type)
   */
  void selectTarget();

  /*! \brief Update table name when it was selected
   */
  void updateTableName(const QString & name);

 private:

  /*! \brief Set widget to unknown codec type
   */
  void setCodecTypeUnknown();

  /*! \brief Set widget to Sqlite codec type
   */
  void setCodecTypeSqlite();

  /*! \brief Update widgets data
   */
  void updateWidgetsData();

  /*! \brief Let user select a SQLite database
   */
  void selectSqliteDatabase();

  Q_DISABLE_COPY(mdtSqlCopierCodecSettingsWidget)

//   std::unique_ptr<mdtSqlCopierCodecSettings> pvSettings;
  mdtSqlCopierCodecSettings pvSettings;
};

#endif // #ifndef MDT_SQL_COPIER_CODEC_SETTINGS_WIDGET_H
