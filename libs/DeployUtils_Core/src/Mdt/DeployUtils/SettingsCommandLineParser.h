/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_SETTINGS_COMMAND_LINE_PARSER_H
#define MDT_DEPLOY_UTILS_SETTINGS_COMMAND_LINE_PARSER_H

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QCoreApplication>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Build Settings from the command line arguments
   */
  class SettingsCommandLineParser
  {
    Q_DECLARE_TR_FUNCTIONS(SettingsCommandLineParser)

  public:

    /*! \brief Constructor
     */
    SettingsCommandLineParser();

    /*! \brief Process given arguments
     */
    void process(const QStringList & arguments);

    /*! \brief Process arguments of \a app
     */
    void process(const QCoreApplication & app);

  private:

    void displayValues();

    QCommandLineParser mParser;
    QCommandLineOption mSourceFilePath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SETTINGS_COMMAND_LINE_PARSER_H
