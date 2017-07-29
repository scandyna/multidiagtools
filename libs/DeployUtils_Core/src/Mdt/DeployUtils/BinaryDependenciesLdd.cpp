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
#include "BinaryDependenciesLdd.h"
#include "LddWrapper.h"
#include "LddDependenciesParser.h"
#include "LibraryName.h"
#include "LibraryInfo.h"
#include "Mdt/Algorithm.h"
#include "Mdt/PlainText/StringRecord.h"
#include <QString>
#include <QLatin1String>
#include <array>
#include <algorithm>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

/*! \internal List of library to not deploy
 *
 * This was grabbed from https://github.com/probonopd/linuxdeployqt
 */
static const std::array<const char * const, 41> LibrayExcludeListLinux =
{
  "asound", "com_err", "crypt", "c", "dl", "drm", "expat", "fontconfig", "gcc_s","gdk_pixbuf-2",
  "gdk-x11-2.0", "gio-2.0", "glib-2.0", "GL", "gobject-2.0", "gpg-error", "gssapi_krb5", "gtk-x11-2.0", "ICE", "idn",
  "k5crypto", "keyutils", "m", "nss3", "nssutil3", "p11-kit", "pangoft2-1", "pangocairo-1.0", "pango-1.0", "pthread",
  "resolv", "rt", "selinux", "SM", "stdc++", "usb-1.0", "uuid", "X11", "xcb", "z",
  "linux-vdso"
};

BinaryDependenciesLdd::BinaryDependenciesLdd(QObject* parent)
 : BinaryDependenciesImplementationInterface(parent)
{
}

bool BinaryDependenciesLdd::findDependencies(const QString& binaryFilePath)
{
  LddWrapper ldd;
  if(!ldd.execFindDependencies(binaryFilePath)){
    setLastError(ldd.lastError());
    return false;
  }

  LddDependenciesParser parser;
  if( !parser.parse( ldd.readAllStandardOutputString() ) ){
    const QString msg = tr("Parsing ldd output for file '%1' failed.").arg(binaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  auto list = parser.rawDependencies();
  qDebug() << "All:";
  for(const auto & rec : list){
    qDebug() << " Lib: " << rec.data(0);
  }
  fillAndSetDependencies(list);
  qDebug() << "Excluded:";
  for(const auto & rec : list){
    qDebug() << " Lib: " << rec.data(0);
  }
/*
  for(const auto & rec : list){
    qDebug() << "Lib: " << rec.data(0);
    if(rec.columnCount() > 1){
      qDebug() << " path: " << rec.data(1);
    }
  }*/

//   qDebug() << ldd.readAllStandardOutputString().split('\n');
  
  return false;
}

void BinaryDependenciesLdd::fillAndSetDependencies(PlainText::StringRecordList& data)
{
  PlainText::StringRecordList notFoundDependencies;
  PlainText::StringRecordList dependencies;

  // Move libraries that are not found to <notFoundDependencies> list
  Mdt::Algorithm::moveIf( data, notFoundDependencies, isLibraryNotFound );
  // Move libraries that are not in the exclude list to <dependencies>
  Mdt::Algorithm::moveIf( data, dependencies, isLibraryNotInExcludeList );
  // \todo Build and set the resulting dependencies
  setDependencies( stringRecordListToLibraryInfoList(dependencies) );
  
  qDebug() << "Result:";
  for(const auto & rec : dependencies){
    qDebug() << " Lib: " << rec.data(0);
  }

}

bool BinaryDependenciesLdd::isLibraryNotFound(const PlainText::StringRecord& record)
{
  if(record.columnCount() < 2){
    return false;
  }
  return (QString::compare( record.data(1), QLatin1String("not found"), Qt::CaseSensitive ) == 0);
}

bool BinaryDependenciesLdd::isLibraryNotInExcludeList(const PlainText::StringRecord & record)
{
  Q_ASSERT(record.columnCount() > 0);

  const auto cmp = [record](const char * const excludeName){
    LibraryName libName(record.data(0));
    return ( QString::compare( libName.name(), QLatin1String(excludeName), Qt::CaseSensitive ) == 0);
  };
  const auto it = std::find_if( LibrayExcludeListLinux.cbegin(), LibrayExcludeListLinux.cend(), cmp );
  return (it == LibrayExcludeListLinux.cend());
}

LibraryInfoList BinaryDependenciesLdd::stringRecordListToLibraryInfoList(const PlainText::StringRecordList& list)
{
  LibraryInfoList libraryInfoList;

  libraryInfoList.reserve(list.rowCount());
  for(const auto & record : list){
    Q_ASSERT(record.columnCount() > 0);
    LibraryInfo li;
    li.setLibraryPlatformName( record.data(0) );
    if(record.columnCount() > 1){
      li.setAbsoluteFilePath( record.data(1) );
    }
    libraryInfoList.addLibrary(li);
  }

  return libraryInfoList;
}

}} // namespace Mdt{ namespace DeployUtils{
