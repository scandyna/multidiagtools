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
#ifndef MDT_ANALOG_IN_WIDGET_H
#define MDT_ANALOG_IN_WIDGET_H

#include "mdtAbstractIoWidget.h"
#include <QString>

class QwtThermo;
class QLabel;
class mdtAnalogIo;

/*! \brief Representation of a analog input
 *
 * On wide range of automation application,
 *  it is usefull to display a state of a input to the user.
 *  For example, a analog input of a I/O module, with a 8 bit
 *  encoding could be displayed with this class.
 *
 * This class is used to display a analog input as GUI.
 * \sa mdtAnalogIo
 */
class mdtAnalogInWidget : public mdtAbstractIoWidget
{
 Q_OBJECT

 public:

  mdtAnalogInWidget(QWidget *parent = 0);

  ~mdtAnalogInWidget();

  /*! \brief Set the I/O object
   *
   * Make all needed connections with the I/O
   *  object, so that widget is allways updated.
   *
   * \pre io must be a valid pointer.
   */
  void setIo(mdtAnalogIo *io);

 private slots:

  // Used to update GUI from mdtAnalogIo object.
  void setUnit(const QString &unit);
  void setRange(double min, double max);
  void setValue(double value);

 private:

  Q_DISABLE_COPY(mdtAnalogInWidget);

  QwtThermo *thValue;
  QLabel *lbValue;
  QString pvUnit;
  mdtAnalogIo *pvIo;
};

#endif  // #ifndef MDT_ANALOG_IN_WIDGET_H
