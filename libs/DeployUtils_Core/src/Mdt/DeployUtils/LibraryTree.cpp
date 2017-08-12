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
#include "LibraryTree.h"
#include "Impl/LibraryTree/LabeledGraph.h"
#include <boost/property_map/property_map.hpp>
#include <vector>
#include <utility>

// #include <QDebug>

using namespace Mdt::DeployUtils::Impl::LibraryTree;

namespace Mdt{ namespace DeployUtils{

/*
 * Library tree impl
 */

class LibraryTreeImpl
{
 public:

  LibraryTreeNode setRootBinary(const QString & name);
  QString rootBinary() const;
  LibraryTreeNode addLibrary(const QString & name, LibraryTreeNode parent);
  QStringList getLibraryList(LibraryTreeNode parent) const;
  QStringList toFlatList() const;
  bool containsNode(LibraryTreeNode node) const;
  void clear();

 private:

  int nextId();

  int mLastInsertId = 0;
  LibraryTreeNode mRootNote;
  Impl::LibraryTree::LabeledGraph mGraph;
};

LibraryTreeNode LibraryTreeImpl::setRootBinary(const QString& name)
{
  clear();
  mLastInsertId = 1;
  boost::add_vertex(1, VertexData(name), mGraph);

  return LibraryTreeNode(1);
}

QString LibraryTreeImpl::rootBinary() const
{
  if(mLastInsertId < 1){
    return QString();
  }
  return mGraph[1].name;
}

LibraryTreeNode LibraryTreeImpl::addLibrary(const QString& name, LibraryTreeNode parent)
{
  const LibraryTreeNode newNode( nextId() );

  boost::add_vertex(newNode.id(), VertexData(name), mGraph);
  boost::add_edge_by_label(parent.id(), newNode.id(), mGraph);

  return newNode;
}

QStringList LibraryTreeImpl::getLibraryList(LibraryTreeNode parent) const
{
  QStringList list;

  if(boost::num_vertices(mGraph) < 2){
    return list;
  }
  const auto vertex = mGraph.vertex(parent.id());
  const auto firstEndIt = boost::adjacenct_vertices(vertex, mGraph);
  for(auto it = firstEndIt.first; it != firstEndIt.second; ++it){
    list << boost::get(boost::vertex_bundle, mGraph)[*it].name;
  }

  return list;
}

QStringList LibraryTreeImpl::toFlatList() const
{
  QStringList list;

  if(boost::num_vertices(mGraph) < 2){
    return list;
  }
  const auto firstEndIt = boost::vertices(mGraph);
  for(auto it = firstEndIt.first; it != firstEndIt.second; ++it){
    list << boost::get(boost::vertex_bundle, mGraph)[*it].name;
  }
  list.removeDuplicates();
  list.removeAll(rootBinary());

  return list;
}

bool LibraryTreeImpl::containsNode(LibraryTreeNode node) const
{
  return ( mGraph.vertex(node.id()) != mGraph.null_vertex() );
}

void LibraryTreeImpl::clear()
{
  mGraph.graph().clear();
  mLastInsertId = 0;
}

int LibraryTreeImpl::nextId()
{
  ++mLastInsertId;
  return mLastInsertId;
}

/*
 * Library tree
 */

LibraryTree::LibraryTree()
 : mImpl( std::make_unique<LibraryTreeImpl>() )
{
}

LibraryTree::~LibraryTree()
{
}

LibraryTreeNode LibraryTree::setRootBinary(const QString& name)
{
  return mImpl->setRootBinary(name);
}

QString LibraryTree::rootBinary() const
{
  return mImpl->rootBinary();
}

LibraryTreeNode LibraryTree::addLibrary(const QString& name, LibraryTreeNode parent)
{
  Q_ASSERT(!parent.isNull());
  Q_ASSERT(containsNode(parent));

  return mImpl->addLibrary(name, parent);
}

QStringList LibraryTree::getLibraryList(LibraryTreeNode parent) const
{
  Q_ASSERT(!parent.isNull());
  Q_ASSERT(containsNode(parent));

  return mImpl->getLibraryList(parent);
}

QStringList LibraryTree::toFlatList() const
{
  return mImpl->toFlatList();
}

void LibraryTree::clear()
{
  mImpl->clear();
}

bool LibraryTree::containsNode(Mdt::DeployUtils::LibraryTreeNode node) const
{
  Q_ASSERT(!node.isNull());

  return mImpl->containsNode(node);
}

}} // namespace Mdt{ namespace DeployUtils{
