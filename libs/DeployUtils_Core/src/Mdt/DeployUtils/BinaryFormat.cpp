/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "BinaryFormat.h"
#include "ObjdumpWrapper.h"
#include "ObjdumpBinaryFormatParser.h"

namespace Mdt{ namespace DeployUtils{

BinaryFormat::BinaryFormat(QObject* parent)
 : QObject(parent)
{
}

bool BinaryFormat::readFormat(const QString& binaryFilePath)
{
  ObjdumpWrapper objdump;
  if(!objdump.execReadFormat(binaryFilePath)){
    setLastError(objdump.lastError());
    return false;
  }

  ObjdumpBinaryFormatParser parser;
  if(!parser.parse(objdump.readAllStandardOutputString())){
    const QString msg = tr("Could not parse the binary format of file '%1'")
                        .arg(binaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  mOperatingSystem = parser.operatindSystem();
  mProcessor = parser.processor();

  return true;
}

void BinaryFormat::setLastError(const Error & error)
{
  mLastError = error;
  mLastError.commit();
}


}} // namespace Mdt{ namespace DeployUtils{
