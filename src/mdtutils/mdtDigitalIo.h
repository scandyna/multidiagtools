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

  /*! \brief Get current I/O state
   */
  bool isOn();

 public slots:

  /*! \brief Set I/O state (On or OFF)
   *
   * Store the state and emit stateChanged() if
   *  new value state different from current.
   *
   * \param on The state to store
   * \param isValid The validity flag. This flag is later avaliable with mdtAbstractIo::hasValidData()
   *
   * Note for UI developpers:
   *  - The signal stateChangedForUi() is emited
   */
  void setOn(bool on, bool isValid);

  /*! \brief Set I/O state (On or OFF)
   *
   * Overloaded method that calls setOn(bool, bool)
   *  with isValid = true.
   */
  void setOn(bool on);

 signals:

  /*! \brief This signal is emitted whenever the state is changed
   */
  void stateChanged(bool on);
  void stateChanged(int address, bool on);

  /*
   * This signal is emited every time
   *  a member is set with a setter method.
   * Usefull to update the UI (should not be used for other purpose)
   */
  void stateChangedForUi(bool on);

 private slots:

  // Used from UI to update internal state.
  //  The stateChangedForUi() signal will not be emitted with this call.
  void setStateFromUi(bool on);

 private:

  Q_DISABLE_COPY(mdtDigitalIo);

  bool pvIsOn;
};

#endif  // #ifndef MDT_DIGITAL_IO_H
