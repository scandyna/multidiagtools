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
#ifndef MDT_ANALOG_OUT_WIDGET_H
#define MDT_ANALOG_OUT_WIDGET_H

#include "mdtAbstractIoWidget.h"
#include "mdtValue.h"
#include <QString>

class QDoubleSpinBox;
class QwtSlider;
class mdtAnalogIo;

/*! \brief Representation of a analog output
 *
 * On wide range of automation application,
 *  it is usefull to display a output to the user.
 *  For example, a analog output of a I/O module, with a 8 bit
 *  encoding could be displayed with this class.
 *
 * This class is used to display a analog output as GUI.
 * \sa mdtAnalogIo
 */
class mdtAnalogOutWidget : public mdtAbstractIoWidget
{
 Q_OBJECT

 public:

  mdtAnalogOutWidget(QWidget *parent = 0);

  ~mdtAnalogOutWidget();

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
  ///void setValue(double value);
  void setEnabled(bool enabled);

  /*! \brief Update displayed value
   *
   * \pre If value is valid, it must contain double value.
   */
  void setValue(const mdtValue &value);

 private:

  Q_DISABLE_COPY(mdtAnalogOutWidget);

  QwtSlider *slValue;
  QDoubleSpinBox *sbValue;
  QString pvUnit;
};

#endif  // #ifndef MDT_ANALOG_OUT_WIDGET_H
