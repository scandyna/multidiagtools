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
#include "Mdt/DeployUtils/Console.h"
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

// #include <QDebug>

using namespace Mdt::DeployUtils;

CommandLineParser::CommandLineParser()
  : mDestinationDirectoryOption(QStringList{"d","destination"}),
    mSearchFirstPathPrefixListOption(QStringList{"p","prefix-path"}),
    mVerboseLevelOption("verbose")
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
  mSearchFirstPathPrefixListOption.setDescription(
    tr("If set, libraries and plugins will be searched in specified directories first. "
       "For libraries, this option is only used for binaries that do not support RPATH (for example .exe or .dll). "
       "Internally, searching is done in known subdirectories of each specified directory (for example bin, qt5/bin). "
       "Note: the list must be a string with ; separated values (This makes passing lists from CMake easy, and avoids platform specific issues).")
  );
  mSearchFirstPathPrefixListOption.setValueName("path-list");
  mParser.addOption(mSearchFirstPathPrefixListOption);
  mVerboseLevelOption.setDescription(
    tr("Level of details to display (0-4).")
  );
  mVerboseLevelOption.setValueName("level");
  mParser.addOption(mVerboseLevelOption);
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
    Console::error() << "binary-file argument is missing";
    return false;
  }
  if(mParser.positionalArguments().size() > 1){
    Console::error() << "to many arguments passed (expected a binary-file only).";
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
  // Library search first prefix paths
  mSearchFirstPathPrefixList = PathList::fromStringList( mParser.value(mSearchFirstPathPrefixListOption).split(';', QString::SkipEmptyParts) );
  // Verbose level
  if(mParser.isSet(mVerboseLevelOption)){
    bool ok;
    mVerboseLevel = mParser.value(mVerboseLevelOption).toInt(&ok);
    if(!ok || (mVerboseLevel < 0)){
      Console::error() << "Invalid verbose level:" << mParser.value(mVerboseLevelOption) << " Possible values: 0-4";
      return false;
    }
  }

  return true;
}
