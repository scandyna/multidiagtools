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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_OBJDUMP_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_OBJDUMP_H

#include "BinaryDependenciesImplementationInterface.h"
#include "LibraryInfoList.h"
#include "PathList.h"
#include "LibraryTree.h"
#include "LibraryTreeNode.h"
#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Binary dependencies objdump implementation
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT BinaryDependenciesObjdump : public BinaryDependenciesImplementationInterface
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependenciesObjdump(QObject* parent = nullptr);

    /*! \brief Find dependencies for a executable or a library
     */
    bool findDependencies(const QString & binaryFilePath) override;

    /*! \brief Find dependencies for a list of binaries
     */
    bool findDependencies(const QStringList & binariesFilePaths) override;

    /*! \brief Find dependencies for a list of libraries
     */
    bool findDependencies(const LibraryInfoList & libraries) override;

    /*! \brief Get a list of paths where libraries are searched
     */
    PathList librarySearchPathList() const
    {
      return mLibrarySearchPathList;
    }

   private:

    bool findAndAddDependenciesForNode(const QString & binaryFilePath, LibraryTreeNode node);
    static bool isLibraryInExcludeList(const PlainText::StringRecord & record);
    bool isBinaryFromCaller(const QString & binaryFilePath) const;
    void setLibrarySearchPathList();
    LibraryTreeNode init(const QString & binaryFilePath);
    void storeDependencies();

    LibraryTree mLibraryTree;
    PathList mLibrarySearchPathList;
    QStringList mAlreadyProcessedLibraries;
    QStringList mBinariesFromCaller;
    PlainText::StringRecordList mNotFoundDependencies;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_OBJDUMP_H
