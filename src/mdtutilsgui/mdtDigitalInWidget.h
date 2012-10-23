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
#ifndef MDT_DIGITAL_IN_WIDGET_H
#define MDT_DIGITAL_IN_WIDGET_H

#include "mdtAbstractIoWidget.h"

class mdtLed;
class QLabel;

/*! \brief Representation of a digital input
 *
 * On wide range of automation application,
 *  it is usefull to display a input to the user.
 */
class mdtDigitalInWidget : public mdtAbstractIoWidget
{
 Q_OBJECT

 public:

  mdtDigitalInWidget(QWidget *parent = 0);

  ~mdtDigitalInWidget();

  /*! \brief Get the input state
   */
  bool isOn();

 public slots:

  /*! \brief Set the input state
   */
  void setOn(bool on);

 private:

  Q_DISABLE_COPY(mdtDigitalInWidget);

  mdtLed *ldState;
  QLabel *lbState;
};

#endif  // #ifndef MDT_DIGITAL_IN_WIDGET_H
