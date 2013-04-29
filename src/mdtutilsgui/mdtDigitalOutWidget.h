/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_DIGITAL_OUT_WIDGET_H
#define MDT_DIGITAL_OUT_WIDGET_H

#include "mdtAbstractIoWidget.h"
#include "mdtValue.h"

class QPushButton;
class mdtDigitalIo;

class mdtDigitalOutWidget : public mdtAbstractIoWidget
{
 Q_OBJECT

 public:

  mdtDigitalOutWidget(QWidget *parent = 0);

  ~mdtDigitalOutWidget();

  /*! \brief Set the I/O object
   *
   * Make all needed connections with the I/O
   *  object, so that widget is allways updated.
   *
   * \pre io must be a valid pointer.
   */
  void setIo(mdtDigitalIo *io);

  /*! \brief Get the internal push button object
   *
   * Used by unit testing, should not be used directly
   */
  QPushButton *internalPushButton();

 private slots:

  /*! \brief Update displayed value
   *
   * \pre If value is valid, it must contain double value.
   */
  void setValue(const mdtValue &value);

  // if state is != from current state, text will be updated
  void updateText(bool state);
  void setEnabled(bool enabled);

 private:

  Q_DISABLE_COPY(mdtDigitalOutWidget);

  QPushButton *pbState;
  mdtDigitalIo *pvIo;
};

#endif  // #ifndef MDT_DIGITAL_OUT_WIDGET_H
