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
#ifndef MDT_DIGITAL_IO_H
#define MDT_DIGITAL_IO_H

#include "mdtAbstractIo.h"

/*! \brief Representation of a digital I/O
 *
 * On wide range of automation applications,
 *  it is usefull to display a digital I/O to the user.
 *  For example, a digital I/O can be displayed by
 *  using this class together with mdtDigitalInWidget or mdtDigitalOutWidget
 */
class mdtDigitalIo : public mdtAbstractIo
{
 Q_OBJECT

 public:

  mdtDigitalIo(QObject *parent = 0);

  ~mdtDigitalIo();

 public slots:

  /*! \brief Set the value to update display
   *
   * \sa mdtAbstractIo::setValue(const mdtValue&, bool)
   */
  void setValue(const mdtValue &value, bool emitValueChanged = false);

 private slots:

  // Used from UI to update internal state.
  //  The stateChangedForUi() signal will not be emitted with this call.
  void setStateFromUi(bool on);

 private:

  Q_DISABLE_COPY(mdtDigitalIo);
};

#endif  // #ifndef MDT_DIGITAL_IO_H
