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
#ifndef MDT_DEVICE_INFO_H
#define MDT_DEVICE_INFO_H

#include <QString>
#include <QMetaType>

/*! \brief Contains informations about a device (attached to a port)
 *
 * Used by mdtPortManager in scan() method.
 */
class mdtDeviceInfo
{
 public:

  mdtDeviceInfo();

  ~mdtDeviceInfo();

  /*! \brief Set vendor ID
   */
  void setVendorId(int vid);

  /*! \brief Get vendor ID
   */
  int vendorId() const;

  /*! \brief Get vendor name
   */
  QString vendorName() const;

  /*! \brief Set product ID
   */
  void setProductId(int pid);

  /*! \brief Get product ID
   */
  int productId() const;

  /*! \brief Get product name
   */
  QString productName() const;

  /*! \brief Get the display text
   *
   * Returns a text that can be displayed to the user.
   *  (F.ex. in a combo box)
   */
  QString displayText() const;

  /*! \brief Comparaison operator
   */
  bool operator==(const mdtDeviceInfo &other);

  /*! \brief Comparaison operator
   */
  bool operator!=(const mdtDeviceInfo &other);

private:

  int pvVendorId;
  QString pvVendorName;
  int pvProductId;
  QString pvProductName;
};

Q_DECLARE_METATYPE(mdtDeviceInfo)

#endif  // #ifndef MDT_DEVICE_INFO_H
