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
#ifndef MDT_IO_WIDGET_TEST_H
#define MDT_IO_WIDGET_TEST_H

#include "mdtTest.h"

class mdtIoWidgetTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void analogInWidgetTest();

  void analogOutWidgetTest();

  /*
   * Some physical device can send
   *  a feedback after a output value was setted.
   * We must be able to display the real value
   *  that device confirm to the user.
   * The problem: this can produce a infinite loop
   *  when expected value (request) and feedback
   *  are not the same.
   */
  void analogOutWidgetRecursifTest();

  void digitalInWidgetTest();

  void digitalOutWidgetTest();
};

#endif  // #ifndef MDT_IO_WIDGET_TEST_H
