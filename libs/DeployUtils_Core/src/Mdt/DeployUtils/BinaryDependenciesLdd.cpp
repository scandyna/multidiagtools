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

// class LibrayExcludeListLinux
// {
//  public:
// 
//   
// 
//  private:
// 
//   static const int mListSize = 2;
//   static const std::array<int, 2> mList;
// };

// constexpr int arraySize = 3;
// constexpr char excludeArray[arraySize][9] =
// {
//   "excluse0",
//   "excluse1",
//   "other"
// };

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
  if(!parser.parse( ldd.readAllStandardOutputString() ) ){
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

void BinaryDependenciesLdd::fillAndSetDependencies(const PlainText::StringRecordList & data)
{

}

}} // namespace Mdt{ namespace DeployUtils{
