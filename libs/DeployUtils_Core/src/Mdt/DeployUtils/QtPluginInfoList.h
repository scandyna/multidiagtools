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
#ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_LIST_H
#define MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_LIST_H

#include "QtPluginInfo.h"
#include "LibraryInfoList.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QVector>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Container that holds a list of QtPluginInfo
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QtPluginInfoList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QVector<QtPluginInfo>::const_iterator;

    /*! \brief STL value type
     */
    using value_type = QVector<QtPluginInfo>::value_type;

    /*! \brief Construct a empty Qt plugin info list
     */
    QtPluginInfoList() = default;

    /*! \brief Construct a Qt plugin info list from initializer lists
     */
    QtPluginInfoList(std::initializer_list<QtPluginInfo> list);

    /*! \brief Copy construct a Qt plugin info list from a other
     */
    QtPluginInfoList(const QtPluginInfoList &) = default;

    /*! \brief Copy assign a Qt plugin info list this this one
     */
    QtPluginInfoList & operator=(const QtPluginInfoList &) = default;

    /*! \brief Move construct a Qt plugin info list from a other
     */
    QtPluginInfoList(QtPluginInfoList &&) = default;

    /*! \brief Move assign a Qt plugin info list this this one
     */
    QtPluginInfoList & operator=(QtPluginInfoList &&) = default;

    /*! \brief Add a plugin info to the end of this list
     *
     * If \a plugin allready exists,
     *  it will not be added.
     */
    void addPlugin(const QtPluginInfo & plugin);

    /*! \brief Get count of items in this list
     */
    int count() const
    {
      return mList.count();
    }

    /*! \brief Add a list of qt plugins to this list
     *
     * Will add each plugin from \a plugins
     *  that not actually exists in this list.
     */
    void addPlugins(const QtPluginInfoList & plugins);

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.isEmpty();
    }

    /*! \brief Get a list of library info that is contained in plugins of this list
     *
     * \note The returned list is generated at each call of this method
     */
    LibraryInfoList toLibraryInfoList() const;

    /*! \brief Get plugin at \a index
     *
     * \pre \a index must be in valid range ( 0 <= index < count() ).
     */
    const QtPluginInfo & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList.at(index);
    }

    /*! \brief Returns an STL-style const iterator pointing to the first item in this list
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns an STL-style iterator pointing to the imaginary item after the last item in this list
     */
    const_iterator cend() const
    {
      return mList.cend();
    }

    /*! \brief Returns an STL-style const iterator pointing to the first item in this list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns an STL-style iterator pointing to the imaginary item after the last item in this list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

   private:

    QVector<QtPluginInfo> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::QtPluginInfoList)

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_LIST_H
