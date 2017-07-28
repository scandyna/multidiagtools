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
#ifndef MDT_DEPLOY_UTILS_CORE_TEST_BASE_H
#define MDT_DEPLOY_UTILS_CORE_TEST_BASE_H

#include "Mdt/Application.h"
#include "Mdt/PlainText/TestUtils.h"
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QtTest/QtTest>
#include <Qt>

class TestBase : public QObject
{
 Q_OBJECT

 protected:

  static bool createFileInTemporaryDirectory(const QTemporaryDir & testRootDirectory, const QString & absoluteFilePath);
  static bool writeTemporaryTextFile(QTemporaryFile & file, const QString & data, const QByteArray & encoding = QByteArray("UTF-8"));
};

#endif // #ifndef MDT_DEPLOY_UTILS_CORE_TEST_BASE_H
