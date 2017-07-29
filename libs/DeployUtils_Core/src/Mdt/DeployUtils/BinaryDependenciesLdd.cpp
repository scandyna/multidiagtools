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
#include "Mdt/PlainText/StringRecord.h"
#include <array>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

/*! \internal List of library to not deploy
 *
 * This was grabbed from https://github.com/probonopd/linuxdeployqt
 */
static const std::array<const char * const, 40> LibrayExcludeListLinux =
{
  "asound", "com_err", "crypt", "c", "dl", "drm", "expat", "fontconfig", "gcc_s","gdk_pixbuf-2",
  "gdk-x11-2.0", "gio-2.0", "glib-2.0", "GL", "gobject-2.0", "gpg-error", "gssapi_krb5", "gtk-x11-2.0", "ICE", "idn",
  "k5crypto", "keyutils", "m", "nss3", "nssutil3", "p11-kit", "pangoft2-1", "pangocairo-1.0", "pango-1.0", "pthread",
  "resolv", "rt", "selinux", "SM", "stdc++", "usb-1.0", "uuid", "X11", "xcb", "z"
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

  const auto list = parser.rawDependencies();
  for(const auto & rec : list){
    qDebug() << "Lib: " << rec.data(0);
    if(rec.columnCount() > 1){
      qDebug() << " path: " << rec.data(1);
    }
  }

//   qDebug() << ldd.readAllStandardOutputString().split('\n');
  
  return false;
}

/** \todo Add moveIf() in Mdt::Algorithm:
 *
 * \note Probably not a good idea
 *
 *  - Internaly, use std::partition
 *  - Explain what SourceContainer requires: iterator, begin(), end()
 *  - Explain what DestinationContainer requires: iterator, compatible with std::back_inserter, erase(const_iterator first, const_iterator last)
 *                                                        erase: QVector has erase(iterator first, iterator last), check.
 *
 * template<typename ForwardIt, typename OutputIt, typename UnaryPredicate>
 * void moveIf(ForwardIt first, ForwardIt last, OutputIt d_first, UnaryPredicate p);
 *
 * template<typename SourceContainer, typename DestinationContainer, typename UnaryPredicate>
 * void moveIf(SourceContainer & sourceConatiner, DestinationContainer & destinationContainer, UnaryPredicate p);
 */

void BinaryDependenciesLdd::fillAndSetDependencies(const PlainText::StringRecordList & data)
{
  // Move libraries that are not found to <notFoundDependencies> list - see moveIf - Try: write a static function as predicate
  // NOTE: data will detach, and probably deep-copy, but this should allready be a bad situation if some libraries are not found.
  
  // Copy libraries, from data, that are not found, to <notFoundDependencies> - see std::copy_if()
  
  // Copy libraries, from data, that not exists in <notFoundDependencies>, ?? a other temp ? NOTE: moveIf ok ??
  
  // Copy libraries that are not in the exclude list to <dependencies> - see std::set_difference (needs < comp!, or sort both before ?), or std::copy_if
  
}

}} // namespace Mdt{ namespace DeployUtils{
