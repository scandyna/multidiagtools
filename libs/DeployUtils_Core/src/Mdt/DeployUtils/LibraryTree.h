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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_TREE_H
#define MDT_DEPLOY_UTILS_LIBRARY_TREE_H

#include "LibraryTreeNode.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  class LibraryTreeImpl;

  /*! \brief LibraryTree is a tree that contains library names
   *
   * The tree represents library dependencies of a binary
   *  (a executable or a library).
   *
   * \note All binary and library are simple names,
   *       stored and returned unchanged.
   *       The user of the tree chooses what name makes sense
   *       (name with or without extension, ...)
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT LibraryTree
  {
   public:

    /*! \brief Construct a empty library tree
     */
    LibraryTree();

    /*! \brief Destructor
     */
    ~LibraryTree();

    // Copy disabled
    LibraryTree(const LibraryTree &) = delete;
    LibraryTree & operator=(const LibraryTree &) = delete;
    // Move disabled
    LibraryTree(LibraryTree &&) = delete;
    LibraryTree & operator=(LibraryTree &&) = delete;

    /*! \brief Set the root binary
     *
     * \note Setting a new root binary will first clear this tree
     */
    LibraryTreeNode setRootBinary(const QString & name);

    /*! \brief Get the root binary
     */
    QString rootBinary() const;

    /*! \brief Add a library that is a child of \a parent
     *
     * \pre Root binary must have been set before adding a library
     * \pre \a parent must not be null
     * \pre \a parent must be a existing node in this tree
     * \return A node identifier that relates to the added library
     */
    LibraryTreeNode addLibrary(const QString & name, LibraryTreeNode parent);

    /*! \brief Get a list of libraries that are direct children of \a parent
     *
     * \pre \a parent must not be null
     * \pre \a parent must be a existing node in this tree
     * \note The list is build at each call of this method
     */
    QStringList getLibraryList(LibraryTreeNode parent) const;

    /*! \brief Get a list of all libraries
     *
     * Returns a list containing all libraries,
     *  except the root.
     *  The list contains a unique instance of each library.
     *
     * \note The list is build at each call of this method
     */
    QStringList toFlatList() const;

    /*! \brief Clear this tree
     */
    void clear();

    /*! \brief Check if this tree contains a specific node
     *
     * \pre \a node must not be null
     */
    bool containsNode(LibraryTreeNode node) const;

   private:

    std::unique_ptr<LibraryTreeImpl> mImpl;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_TREE_H
