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

#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::FileSystem;
using namespace Mdt::Translation;

CommandLineParser::CommandLineParser()
  : mSearchFirstPathPrefixListOption(QStringList{"p","prefix-path"}),
    mLibraryDestinationOption("library-destination"),
    mPluginDestinationOption("plugin-destination"),
    mTranslationsOption("translations"),
    mProjectQmFilesOption("project-qm-files"),
    mTranslationDestinationOption("translation-destination"),
    mVerboseLevelOption("verbose")
{
  mParser.setApplicationDescription(tr("Find binary dependencies of executable(s) or library(ies) and copy them."));
  mParser.addHelpOption();
//   mParser.addVersionOption();
  mSearchFirstPathPrefixListOption.setDescription(
    tr("If set, libraries and plugins will be searched in specified directories first. "
       "For libraries, this option is only used for binaries that do not support RPATH (for example .exe or .dll). "
       "Internally, searching is done in known subdirectories of each specified directory (for example bin, qt5/bin). "
       "Note: the list must be a string with ; separated values (This makes passing lists from CMake easy, and avoids platform specific issues).")
  );
  mSearchFirstPathPrefixListOption.setValueName("path-list");
  mParser.addOption(mSearchFirstPathPrefixListOption);
  mLibraryDestinationOption.setDescription(
    tr("Destination of the dependend shared libraries.")
  );
  mLibraryDestinationOption.setValueName("path");
  mParser.addOption(mLibraryDestinationOption);
  mPluginDestinationOption.setDescription(
    tr("Destination of the dependent plugins.")
  );
  mPluginDestinationOption.setValueName("path");
  mParser.addOption(mPluginDestinationOption);
  mVerboseLevelOption.setDescription(
    tr("Level of details to display (0-4).")
  );
  mTranslationsOption.setDescription(
    tr("List of translation suffixes. "
       "For example, for French translations, pass fr. "
       "It is also possible to specifiy a country, in the form fr_ca . "
       "For each translation suffix and each binary file, a file, called <binary-file>_<translation_suffix>.qm, "
       "will be copied to the path psecified by --translation-destination option. "
       "This file will contain translations for used Qt libraries, for used Mdt libraries and the the binary file itself.")
  );
  mTranslationsOption.setValueName("suffix-list");
  mParser.addOption(mTranslationsOption);
  mProjectQmFilesOption.setDescription(
    tr("A list of full path to QM files for the project. "
       "Those files will also be included to the final QM file, regarding specified translations.")
  );
  mProjectQmFilesOption.setValueName("path-list");
  mParser.addOption(mProjectQmFilesOption);
  mTranslationDestinationOption.setDescription(
    tr("Destination of the translations. ")
  );
  mTranslationDestinationOption.setValueName("path");
  mParser.addOption(mTranslationDestinationOption);
  mVerboseLevelOption.setValueName("level");
  mParser.addOption(mVerboseLevelOption);
  mParser.addPositionalArgument(
    "binary-files",
    tr("list of executables or libraries for which dependencies must be copied. "
       "Note: the list must be a string with ; separated values (This makes passing lists from CMake easy, and avoids platform specific issues).")
  );
}

bool CommandLineParser::process()
{
  mParser.process(QCoreApplication::arguments());

  return checkAndSetArguments();
}

bool CommandLineParser::checkAndSetArguments()
{
  if(mParser.positionalArguments().size() != 1){
    Console::error() << "expected 1 arguments (binary-files), provided: " << mParser.positionalArguments().size();
    return false;
  }
  Q_ASSERT(mParser.positionalArguments().size() == 1);
  // List of binaries
  mBinaryFilesPathList = mParser.positionalArguments().at(0).split(';', QString::SkipEmptyParts);
  // Library destination
  if(!mParser.isSet(mLibraryDestinationOption)){
    Console::error() << "expected a library-destination";
    return false;
  }
  mLibraryDestinationPath = mParser.value(mLibraryDestinationOption);
  // Plugins destinations
  if(!mParser.isSet(mPluginDestinationOption)){
    Console::error() << "expected a plugin-destination";
    return false;
  }
  mPluginDestinationPath = mParser.value(mPluginDestinationOption);
  // Library search first prefix paths
  mSearchFirstPathPrefixList = PathList::fromStringList( mParser.value(mSearchFirstPathPrefixListOption).split(';', QString::SkipEmptyParts) );
  // Translations
  mTranslations = mParser.value(mTranslationsOption).split(';', QString::SkipEmptyParts);
  qDebug() << "TR: " << mTranslations;
  mProjectQmFiles = TranslationInfoList::fromQmFilePathList( mParser.value(mProjectQmFilesOption).split(';', QString::SkipEmptyParts) );
  
  mTranslationDestinationPath = mParser.value(mTranslationDestinationOption);
  qDebug() << "TR path: " << mTranslationDestinationPath;
  if(mTranslationDestinationPath.isEmpty() && !mTranslations.isEmpty()){
    Console::error() << "Argument error: given translations, but no path to install them.";
    return false;
  }
  if(mTranslations.isEmpty() && !mProjectQmFiles.isEmpty()){
    Console::error() << "Argument error: given project QM files, but no translations.";
    return false;
  }
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
