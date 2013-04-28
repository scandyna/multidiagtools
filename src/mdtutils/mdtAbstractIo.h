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
#ifndef MDT_ABSTRACT_IO_H
#define MDT_ABSTRACT_IO_H

#include "mdtValue.h"
#include <QObject>
#include <QString>

/*! \brief Base class for I/O handling on (G)UI
 *
 * Can be used with mdtAbstractIoWidget and subclasses.
 * This class offers the functionnal part of I/O representation
 *  on a UI, or GUI when used with mdtAbstractIoWidget.
 */
class mdtAbstractIo : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Construct a empty I/O
   *
   * A empty I/O has address 0 and no valid data.
   */
  mdtAbstractIo(QObject *parent = 0);

  virtual  ~mdtAbstractIo();

  /*! \brief Set the I/O address
   *
   * Note: will set both addressRead and addressWrite
   *  to given address.
   */
  void setAddress(int address);

  /*! \brief Get the I/O address
   *
   * Note: if addressRead and addressWrite are not the same,
   *   addressRead is returned.
   */
  int address() const;

  /*! \brief Set the I/O address for read
   *
   * For some devices, accessing a I/O to set (write)
   *  or get (read) its value can be done by another
   *  address. Typical examples are some MODBUS fieldbuses
   *  (like Wago 750 or Beckhoff BK 90xx).
   *  For such case, you can use this method to set the read access address.
   */
  void setAddressRead(int address);

  /*! \brief Get the I/O address for read
   *
   * See setAddressRead() for details.
   */
  int addressRead() const;

  /*! \brief Set the I/O address for write
   *
   * For some devices, accessing a I/O to set (write)
   *  or get (read) its value can be done by another
   *  address. Typical examples are some MODBUS fieldbuses
   *  (like Wago 750 or Beckhoff BK 90xx).
   *  For such case, you can use this method to set the read access address.
   */
  void setAddressWrite(int address);

  /*! \brief Get the I/O address for write
   *
   * See setAddressWrite() for details.
   */
  int addressWrite() const;

  /*! \brief Set the short I/O label
   *
   * When used with mdtAbstractIoWidget,
   *  the short I/O label is allways displayed
   */
  void setLabelShort(const QString &text);

  /*! \brief Get the short I/O label
   */
  QString labelShort() const;

  /*! \brief Set the complete I/O label
   *
   * When used with mdtAbstractIoWidget,
   *  this label is displayed as tool tip
   */
  void setLabel(const QString &text);

  /*! \brief Get the complete I/O label
   */
  QString label() const;

  /*! \brief Set details about the I/O
   */
  void setDetails(const QString & text);

  /*! \brief Get details about the I/O
   */
  QString details() const;

  /*! \brief Returns true if a valid value or state was set
   */
  bool hasValidData() const;

  /*! \brief Set enabled state
   */
  void setEnabled(bool enabled);

 signals:

  /*
   * These signals are emited every time
   *  a member is set with a setter method.
   * Usefull to update the UI (should not be used for other purpose)
   */
  void addressChangedForUi(int addressRead, int addressWrite);
  void labelShortChangedForUi(const QString &text);
  void labelChangedForUi(const QString &text);
  void detailsChangedForUi(const QString &text);
  void enabledStateChangedForUi(bool enabled);
  /// NEW
  /*! \brief This signal is emitted whenever the value is changed
   */
  void valueChanged(const mdtValue &value);

  /*! \brief This signal is emitted whenever the value is changed
   *
   * Use it only for (G-)UI update
   */
  void valueChangedForUi(const mdtValue &value);

 protected:

  bool pvHasValidData;  /// \todo Obeslete this flag (use mdtValue's flag )
  mdtValue pvValue;

 private:

  Q_DISABLE_COPY(mdtAbstractIo);

  int pvAddressRead;
  int pvAddressWrite;
  QString pvLabelShort;
  QString pvLabel;
  QString pvDetails;
};

#endif  // #ifndef MDT_ABSTRACT_IO_H
