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

  /*! \brief Get binary file path
   */
  QString binaryFilePath() const
  {
    return mBinaryFilePath;
  }

  /*! \brief Get destination directory path
   */
  QString destinationDirectoryPath() const
  {
    return mDestinationDirectoryPath;
  }

private:

  bool checkAndSetArguments();

  QString mBinaryFilePath;
  QString mDestinationDirectoryPath;
  QCommandLineParser mParser;
  QCommandLineOption mDestinationDirectoryOption;
};

#endif // #ifndef COMMAND_LINE_PARSER_H
