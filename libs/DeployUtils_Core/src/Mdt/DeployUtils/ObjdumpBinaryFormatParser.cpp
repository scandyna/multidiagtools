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
#include "ObjdumpBinaryFormatParser.h"
#include "Impl/Objdump/BinaryFormatParserImpl.h"
#include <QString>
#include <QLatin1String>
#include <array>
#include <algorithm>

using namespace Mdt::PlainText;

namespace Mdt{ namespace DeployUtils{

static const std::array<const char * const, 1> LinuxFormatNames =
{{
  "elf64-x86-64"
}};

static const std::array<const char * const, 1> WindowsFormatNames =
{{
  "pei-i386"
}};

ObjdumpBinaryFormatParser::ObjdumpBinaryFormatParser()
 : mParser( std::make_unique< Impl::Objdump::FormatParserTemplate<StringConstIterator> >() )
{
}

ObjdumpBinaryFormatParser::~ObjdumpBinaryFormatParser()
{
}

bool ObjdumpBinaryFormatParser::parse(const QString& data)
{
  StringConstIterator first = data.cbegin();
  StringConstIterator last = data.cend();

  if(!mParser->parseAll(first, last)){
    return false;
  }
  const auto record = mParser->rawFormatRecord();
  Q_ASSERT(record.columnCount() == 2);
  if(!setOperatingSystem(record.data(0))){
    return false;
  }
  if(!setProcessor(record.data(1))){
    return false;
  }

  return true;
}

bool ObjdumpBinaryFormatParser::setOperatingSystem(const QString& os)
{
  if(isOsLinux(os)){
    mOperatingSystem = OperatingSystem::Linux;
    return true;
  }
  if(isOsWindows(os)){
    mOperatingSystem = OperatingSystem::Windows;
    return true;
  }
  return false;
}

bool ObjdumpBinaryFormatParser::isOsLinux(const QString& os)
{
  const auto cmp = [os](const char * const s){
    return ( QString::compare( os, QLatin1String(s), Qt::CaseSensitive ) == 0 );
  };
  return ( std::find_if(LinuxFormatNames.cbegin(), LinuxFormatNames.cend(), cmp) != LinuxFormatNames.cend() );
}

bool ObjdumpBinaryFormatParser::isOsWindows(const QString& os)
{
  const auto cmp = [os](const char * const s){
    return ( QString::compare( os, QLatin1String(s), Qt::CaseSensitive ) == 0 );
  };
  return ( std::find_if(WindowsFormatNames.cbegin(), WindowsFormatNames.cend(), cmp) != WindowsFormatNames.cend() );
}

bool ObjdumpBinaryFormatParser::setProcessor(const QString& processor)
{
  if(compareEqual(processor, "i386:x86-64")){
    mProcessor = Processor::X86_64;
    return true;
  }
  if(compareEqual(processor, "i386")){
    mProcessor = Processor::X86_32;
    return true;
  }
  return false;
}

bool ObjdumpBinaryFormatParser::compareEqual(const QString& str, const char*const cs)
{
  return ( QString::compare(str, QLatin1String(cs), Qt::CaseSensitive) == 0 );
}

}} // namespace Mdt{ namespace DeployUtils{
