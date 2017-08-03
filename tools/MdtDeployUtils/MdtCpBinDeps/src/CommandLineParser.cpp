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

CommandLineParser::CommandLineParser()
  : mDestinationDirectoryOption(QStringList{"d","destination"})/*,
    mTargetOperatingSystem("target-os")*/
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

  return true;
}
