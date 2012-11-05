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
#ifndef MDT_DEVICE_IOS_WIDGET_H
#define MDT_DEVICE_IOS_WIDGET_H

#include <QWidget>
#include <QList>

class mdtDeviceIos;
class mdtAnalogInWidget;
class mdtDigitalInWidget;
class mdtAnalogOutWidget;
class mdtDigitalOutWidget;
class QGridLayout;

/*! \brief Display a set of I/O's
 */
class mdtDeviceIosWidget : public QWidget
{
 Q_OBJECT

 public:

  mdtDeviceIosWidget(QWidget *parent = 0);

  ~mdtDeviceIosWidget();

  /*! \brief Set the maximum columns on the layout
   *
   * \pre n must be > 0
   */
  void setMaxColumns(int n);

  /*! \brief Add all I/O's as corresponding GUI representation
   *
   * Will display a I/O corresponding to the type
   *  set for each item avaliable in ios.
   * If I/O's allready exists, they are remove first,
   *  so that this method can be called multiple times if needed.
   *
   * \pre ios must be a valid pointer
   */
  void setDeviceIos(mdtDeviceIos *ios);

 private:

  Q_DISABLE_COPY(mdtDeviceIosWidget);

  QList<mdtAnalogInWidget*> pvAnalogInputs;
  QList<mdtAnalogOutWidget*> pvAnalogOutputs;
  QList<mdtDigitalInWidget*> pvDigitalInputs;
  QList<mdtDigitalOutWidget*> pvDigitalOutputs;
  int pvMaxColumns;
  QGridLayout *pvLayout;
  mdtDeviceIos *pvIos;
};

#endif  // #ifndef MDT_DEVICE_IOS_WIDGET_H
