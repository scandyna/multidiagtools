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
#include "CommandLineParser.h"
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#include <QDebug>

using namespace Mdt::DeployUtils;

/// \todo Specify format of lists (should be ; separated string, simply, and fully CMake compatible, no platform specific stuff, spaces problem, ...)

CommandLineParser::CommandLineParser()
  : mDestinationDirectoryOption(QStringList{"d","destination"}),
    mLibrarySearchFirstPathListOption(QStringList{"p","search-first-path-list"}),
    mPathSuffixesOption(QStringList{"s","path-suffixes"})
{
  mParser.setApplicationDescription(tr("Find binary dependencies of a executable or a library and copy them."));
  mParser.addHelpOption();
//   mParser.addVersionOption();
  mDestinationDirectoryOption.setDescription(
    tr("Dependencies will be copied to destination. "
       "If destination directory does not exist, it will be created first. "
       "If this option is omitted, the directory in which binary-file lives is considered.")
  );
  mDestinationDirectoryOption.setValueName("directory");
  mParser.addOption(mDestinationDirectoryOption);
  mLibrarySearchFirstPathListOption.setDescription(
    tr("If set, libraries will be searched in specified directories first. "
       "Note: this has no effect for executable or library that support RPATH (like Linux), "
       "because the location of each library is automatically resolved by ldd")
  );
  mLibrarySearchFirstPathListOption.setValueName("path-list");
  mParser.addOption(mLibrarySearchFirstPathListOption);
  mPathSuffixesOption.setDescription(
    tr("Specify addional subdirectories to check below each directory "
       "in those specified by search-first-path-list."
       "For example, if search-first-path-list is /opt/liba /opt/libb "
       "and path-suffixes contains bin qt5/bin, libraries will be searched in"
       "/opt/liba/bin, /opt/liba/qt5/bin, /opt/libb/bin, /opt/libb/qt5/bin")
  );
  mPathSuffixesOption.setValueName("suffixes");
  mParser.addOption(mPathSuffixesOption);
//   mTargetOperatingSystem.setDescription(
//     tr("When executable")
//   );
  mParser.addPositionalArgument("binary-file", tr("executable or library for which dependencies must be copied."));
}

bool CommandLineParser::process()
{
  mParser.process(QCoreApplication::arguments());

  return checkAndSetArguments();
}

bool CommandLineParser::checkAndSetArguments()
{
  // Binary file path
  if(mParser.positionalArguments().isEmpty()){
    qDebug() << "binary-file argument is missing";
    return false;
  }
  if(mParser.positionalArguments().size() > 1){
    qDebug() << "to many arguments passed.";
    return false;
  }
  Q_ASSERT(mParser.positionalArguments().size() == 1);
  QFileInfo binaryFileInfo(mParser.positionalArguments().at(0));
  mBinaryFilePath = binaryFileInfo.absoluteFilePath();
  // Destination directory
  mDestinationDirectoryPath = mParser.value(mDestinationDirectoryOption);
  if(mDestinationDirectoryPath.isEmpty()){
    mDestinationDirectoryPath = binaryFileInfo.absoluteDir().absolutePath();
  }
  // Library search first paths
  mLibrarySearchFirstPathList = PathList::fromStringList( mParser.value(mLibrarySearchFirstPathListOption).split(';', QString::SkipEmptyParts) );
  mLibrarySearchFirstPathSuffixList = mParser.value(mPathSuffixesOption).split(';', QString::SkipEmptyParts);
  qDebug() << "Lib search: " << mParser.value(mLibrarySearchFirstPathListOption);
  qDebug() << "Suffixes: " << mParser.value(mPathSuffixesOption);

  return true;
}
