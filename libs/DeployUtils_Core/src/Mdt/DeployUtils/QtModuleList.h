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
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_LIST_H
#define MDT_DEPLOY_UTILS_QT_MODULE_LIST_H

#include "QtModule.h"
#include <QVector>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief List of QtModule
   */
  class QtModuleList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QVector<QtModule>::const_iterator;

    /*! \brief Construct a empty module list
     */
    QtModuleList() = default;

    /*! \brief Construct a module list from \a list
     */
    QtModuleList(std::initializer_list<QtModule> list);

    /*! \brief Copy construct a module list from a other
     */
    QtModuleList(const QtModuleList &) = default;

    /*! \brief Assign a module list to this one by copy
     */
    QtModuleList & operator=(const QtModuleList &) = default;

    /*! \brief Move construct a module list from a other
     */
    QtModuleList(QtModuleList &&) = default;

    /*! \brief Assign a module list to this one by move
     */
    QtModuleList & operator=(QtModuleList &&) = default;

    /*! \brief Get count of modules in this list
     */
    int count() const
    {
      return mList.count();
    }

    /*! \brief Add a modules to this list
     *
     * If \a module allready exists,
     *  it will not be added.
     *  QtModule::Unknown will also not be added.
     */
    void addModule(QtModule module);

    /*! \brief Get module at index
     *
     * \pre \a index must be in valid range (0 >= index < count())
     */
    QtModule at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList.at(index);
    }

    /*! \brief Clear this list
     */
    void clear();

    /*! \brief Returns a const STL-style iterator pointing to the first item in the list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns a const STL-style iterator pointing to the first item in the list
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns a const STL-style iterator pointing to the last item in the list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

    /*! \brief Returns a const STL-style iterator pointing to the last item in the list
     */
    const_iterator cend() const
    {
      return mList.cend();
    }

   private:

    QVector<QtModule> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::QtModuleList)

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_LIST_H
