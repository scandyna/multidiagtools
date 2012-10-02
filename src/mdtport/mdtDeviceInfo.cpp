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
#include "mdtDeviceInfo.h"

mdtDeviceInfo::mdtDeviceInfo()
{
}

mdtDeviceInfo::~mdtDeviceInfo()
{
}

void mdtDeviceInfo::setVendorId(int vid)
{
  pvVendorId = vid;
  pvVendorName = "Unknown (not implemented yet)";
}

int mdtDeviceInfo::vendorId() const
{
  return pvVendorId;
}

QString mdtDeviceInfo::vendorName() const
{
  return pvVendorName;
}

void mdtDeviceInfo::setProductId(int pid)
{
  pvProductId = pid;
  pvProductName = "Unknown (not implemented yet)";
}

int mdtDeviceInfo::productId() const
{
  return pvProductId;
}

QString mdtDeviceInfo::productName() const
{
  return pvProductName;
}

bool mdtDeviceInfo::operator==(const mdtDeviceInfo &other)
{
  if(pvVendorId != other.pvVendorId){
    return false;
  }
  if(pvVendorName != other.pvVendorName){
    return false;
  }
  if(pvProductId != other.pvProductId){
    return false;
  }
  if(pvProductName != other.pvProductName){
    return false;
  }

  return true;
}

bool mdtDeviceInfo::operator!=(const mdtDeviceInfo &other)
{
  return !(*this == other);
}
