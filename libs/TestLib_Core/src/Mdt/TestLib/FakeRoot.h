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
#ifndef MDT_TEST_LIB_FAKE_ROOT_H
#define MDT_TEST_LIB_FAKE_ROOT_H

///#include "Mdt/FileSystem/PathList.h"
#include "MdtTestLib_CoreExport.h"
#include <QTemporaryDir>
#include <QString>
#include <QStringList>

/*! \namespace Mdt::TestLib
 *  \brief FakeRoot.h provides some helpers to make tests in a fake file system
 *
 * Example of usage:
 * \code
 * void SomeTest::findPluginsRootTest()
 * {
 *  QFETCH(QStringList, pathPrefixList);
 *  QFETCH(QStringList, existingPaths);
 *  QFETCH(QString, expectedPluginsRoot);
 *  // Setup a fake root
 *  QTemporaryDir root;
 *  QVERIFY(root.isValid());
 *  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
 *  prependFakeRootToPathList(root, pathPrefixList);
 *  prependFakeRootToPath(root, expectedPluginsRoot);
 *  // Check
 *  QCOMPARE( findPluginsRoot(pathPrefixList), expectedPluginsRoot );
 * }
 * \endcode
 */

namespace Mdt{ namespace TestLib{

  /*! \brief Get full path to root + path
   *
   * For example, if root is /tmp/someDir :
   * \code
   * const auto fullPath = pathWithFakeRoot(root, "/usr/lib");
   * // fullPath is: /tmp/someDir/usr/lib
   * \endcode
   */
  QString MDT_TESTLIB_CORE_EXPORT pathWithFakeRoot(const QTemporaryDir & root, const QString & path);

  /*! \brief Prepend path of \a root to \a path
   */
  void MDT_TESTLIB_CORE_EXPORT prependFakeRootToPath(const QTemporaryDir & root, QString & path);

  /*! \brief Apply prependFakeRootToPath() to all elements in \a pathList
   */
  void MDT_TESTLIB_CORE_EXPORT prependFakeRootToPathList(const QTemporaryDir & root, QStringList & pathList);

  ///void MDT_TESTLIB_CORE_EXPORT prependFakeRootToPathList(const QTemporaryDir & root, Mdt::FileSystem::PathList & pathList);

  /*! \brief Create a path in a fake root
   */
  bool MDT_TESTLIB_CORE_EXPORT createPathInFakeRoot(const QTemporaryDir & root, const QString & path);

  /*! \brief Create a list of path in a fake root
   */
  bool MDT_TESTLIB_CORE_EXPORT createPathListInFakeRoot(const QTemporaryDir & root, const QStringList & pathList);

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_FAKE_ROOT_H
