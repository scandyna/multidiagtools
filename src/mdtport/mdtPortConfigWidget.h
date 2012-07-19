/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_PORT_CONFIG_WIDGET_H
#define MDT_PORT_CONFIG_WIDGET_H

#include "ui_mdtPortConfigWidget.h"
#include "mdtPortConfig.h"
#include <QWidget>

class mdtPortConfigWidget : public QWidget, public Ui::mdtPortConfigWidget
{
 Q_OBJECT

 public:

  mdtPortConfigWidget(QWidget *parent = 0);

  /*! \brief Read the config instance and update GUI
   */
  void displayConfig(mdtPortConfig &config);

  /*! \brief Get GUI's selected options and update config
   * 
   * Once done, the config is read again and displayed,
   * so the user can see what was really applied.
   */
  void updateConfig(mdtPortConfig &config);

 private slots:

  void on_cbFrameType_currentIndexChanged(int index);

  ///void on_cbReadOnly_toggled(bool checked);
};

#endif  // #ifndef MDT_PORT_CONFIG_WIDGET_H
