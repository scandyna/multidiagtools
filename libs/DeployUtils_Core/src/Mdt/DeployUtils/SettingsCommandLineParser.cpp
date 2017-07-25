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
#include "SettingsCommandLineParser.h"

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

SettingsCommandLineParser::SettingsCommandLineParser()
 : mSourceFilePath({"s","source-file"}, tr("Path to the source file."), "source")
{
  mParser.setApplicationDescription(tr("Utilities to help deploying a application."));
  mParser.addHelpOption();
  mParser.addVersionOption();
  mParser.addPositionalArgument("action", tr("Action to do.."));
  mParser.addOption(mSourceFilePath);
}

void SettingsCommandLineParser::process(const QStringList & arguments)
{
  mParser.process(arguments);
  displayValues();
}

void SettingsCommandLineParser::process(const QCoreApplication & app)
{
  mParser.process(app);
  displayValues();
}

void SettingsCommandLineParser::displayValues()
{
  qDebug() << "Positional args: " << mParser.positionalArguments();
  qDebug() << "Source file: " << mParser.value(mSourceFilePath);
}

}} // namespace Mdt{ namespace DeployUtils{
