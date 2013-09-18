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
#ifndef MDT_DEVICE_IOS_SEGMENT_H
#define MDT_DEVICE_IOS_SEGMENT_H

#include "mdtValue.h"
#include <QList>
#include <QVariant>

class mdtAbstractIo;
class mdtAnalogIo;
class mdtDigitalIo;

/*! \brief Store a segment of device I/O list
 *
 * A physical device, such as a modular I/O controller
 *  (f.ex. Wago 750) can contain severa modules.
 *  Some of them can be analog I/O, digital I/O or more complex module.
 *
 * This class is a container that helps mdtDevice and mdtDeviceIos to deal with
 *  I/O mapping for multiple I/O read/write.
 *  Typically, some I/O can be present in mdtDeviceIos, but are not
 *  data I/O (but, f.ex. configuration registers).
 *  A other problem is that it can happen that I/O are not present in contigous range,
 *   but in several segments (f.ex. analog inputs from 0x200 -> 0x500, then 0x3000 -> 0x3500, ...)
 */
class mdtDeviceIosSegment
{
 public:

  /*! \brief Constructor
   *
   * Will set default values:
   *  - startAddress : 0
   *  - endAddress   : 0
   *  - ioCount      : 0
   */
  mdtDeviceIosSegment();

  /*! \brief Destructor
   */
  ~mdtDeviceIosSegment();

  /*! \brief Set the I/O's values and address range for this segment
   *
   * \param isInput If true, the I/O's readAddress will be used, else the writeAddress
   * \pre List of I/O's must be sorted by address and not contain holes (i.e. contigous address range)
   */
  void setIos(const QList<mdtAnalogIo*> & ios);
  void setIos(const QList<mdtDigitalIo*> & ios);

  /*! \brief Get start address for read access
   */
  int startAddressRead() const;

  /*! \brief Get start address for write access
   */
  int startAddressWrite() const;

  /*! \brief Get end address for read access
   */
  int endAddressRead() const;

  /*! \brief Get end address for write access
   */
  int endAddressWrite() const;

  /*! \brief Get number of I/O's
   */
  int ioCount() const;

  /*! \brief Check if segment contains a address for read access
   */
  bool containsAddressRead(int address) const;

  /*! \brief Get a list of contigous addresses for read access
   */
  QList<int> addressesRead() const;

  /*! \brief Check if segment contains a address for write access
   */
  bool containsAddressWrite(int address) const;

  /*! \brief Get a list of contigous addresses for write access
   */
  QList<int> addressesWrite() const;

  /*! \brief Set values
   *
   * This method assumes that values list is sorted
   *  exactly the same way than ios list set with setIos()
   *
   * If values list contains less items than stored I/O's values,
   *  only the first ones will be updated.
   * If values list contains more items than stored I/O's values,
   *  all I/O's values are updated, and rest of values are ignored.
   * If max > -1, same rules but not more than max items are stored.
   *
   * \returns Number of I/O's updated
   */
  int setValues(const QList<mdtValue> & values, int max = -1);

  /*! \brief Set values
   *
   * \overload setValues(const QList<mdtValue> &)
   */
  int setValues(const QList<QVariant> & values, int max = -1);

  /*! \brief Update some values starting at address read
   *
   * Note: some checking are made:
   *  - If startAddress was not found, 0 values will be updated
   *  - If values contains to much items, only items that can be stored are updated
   *  - If max > -1 , the minimum between values size, internall items count and max will be updated
   *
   * \returns Number of updated values
   */
  int updateValuesFromAddressRead(int startAddress, const QList<mdtValue> & values, int max = -1);

  /*! \brief Update some values starting at address read
   *
   * \overload updateValuesFromAddressRead(int , const QList<mdtValue> &)
   */
  int updateValuesFromAddressRead(int startAddress, const QList<QVariant> & values, int max = -1);

  /*! \brief Update some values starting at address write
   *
   * Note: some checking are made:
   *  - If startAddress was not found, 0 values will be updated
   *  - If values contains to much items, only items that can be stored are updated
   * - If max > -1 , the minimum between values size, internall items count and max will be updated
   *
   * \returns Number of updated values
   */
  int updateValuesFromAddressWrite(int startAddress, const QList<mdtValue> & values, int max = -1);

  /*! \brief Update some values starting at address write
   *
   * \overload updateValuesFromAddressRead(int , const QList<mdtValue> &)
   */
  int updateValuesFromAddressWrite(int startAddress, const QList<QVariant> & values, int max = -1);

  /*! \brief Get list of stored I/O's values
   */
  QList<mdtValue> values() const;

  /*! \brief Get list of stored I/O's values
   */
  QList<int> valuesInt() const;

  /*! \brief Get list of stored I/O's values
   */
  QList<double> valuesDouble() const;

  /*! \brief Get list of stored I/O's values
   */
  QList<bool> valuesBool() const;

 private:

  Q_DISABLE_COPY(mdtDeviceIosSegment);

  QList<mdtAbstractIo*> pvIos;
};

#endif  // #ifndef MDT_DEVICE_IOS_SEGMENT_H
