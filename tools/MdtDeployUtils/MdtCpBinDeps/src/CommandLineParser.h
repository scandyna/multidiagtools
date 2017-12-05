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
#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QString>
#include <QStringList>
#include <QCoreApplication>

/*! \brief Command line parser for MdtCpBinDeps
 */
class CommandLineParser
{
  Q_DECLARE_TR_FUNCTIONS(CommandLineParser)

public:

  /*! \brief Constructor
   */
  CommandLineParser();

  /*! \brief Process arguments given to the application
   */
  bool process();

  /*! \brief Get the list of path to binary files
   */
  QStringList binaryFilePathList() const
  {
    return mBinaryFilesPathList;
  }

  /*! \brief Get path to the destination of shared libraries
   */
  QString libraryDestinationPath() const
  {
    return mLibraryDestinationPath;
  }

  /*! \brief Get path to the destination of plugins
   */
  QString pluginDestinationPath() const
  {
    return mPluginDestinationPath;
  }

  /*! \brief Get the list of paths prefixes where to search dependencies first
   */
  Mdt::DeployUtils::PathList searchFirstPathPrefixList() const
  {
    return mSearchFirstPathPrefixList;
  }

  /*! \brief Get the list of translations suffixes
   */
  QStringList translations() const
  {
    return mTranslations;
  }

  /*! \brief Get the list of project QM files
   */
  Mdt::Translation::TranslationInfoList projectQmFiles() const
  {
    return mProjectQmFiles;
  }

  /*! \brief Get path to the destination of translations
   */
  QString translationDestinationPath() const
  {
    return mTranslationDestinationPath;
  }

  /*! \brief Get verbose level
   */
  int verboseLevel() const
  {
    return mVerboseLevel;
  }

private:

  bool checkAndSetArguments();

  QStringList mBinaryFilesPathList;
  QString mLibraryDestinationPath;
  QString mPluginDestinationPath;
  Mdt::DeployUtils::PathList mSearchFirstPathPrefixList;
  QStringList mTranslations;
  Mdt::Translation::TranslationInfoList mProjectQmFiles;
  QString mTranslationDestinationPath;
  int mVerboseLevel = 1;
  QCommandLineParser mParser;
  QCommandLineOption mSearchFirstPathPrefixListOption;
  QCommandLineOption mLibraryDestinationOption;
  QCommandLineOption mPluginDestinationOption;
  QCommandLineOption mTranslationsOption;
  QCommandLineOption mProjectQmFilesOption;
  QCommandLineOption mTranslationDestinationOption;
  QCommandLineOption mVerboseLevelOption;
};

#endif // #ifndef COMMAND_LINE_PARSER_H
