/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_STL_TABLE_PROXY_IMPLEMENTATION_INTERFACE_H
#define MDT_ENTITY_STL_TABLE_PROXY_IMPLEMENTATION_INTERFACE_H

namespace Mdt{ namespace Entity{

  /*! \brief Interface to implement a StlTableProxy
   *
   * \todo Obselete
   */
  template<typename EntityData>
  class StlTableProxyImplementationInterface
  {
   public:

    /*! \brief Destructor
     */
    virtual ~StlTableProxyImplementationInterface() = default;

    /*! \brief Get row count
     */
    virtual int rowCount() const = 0;
    
    virtual EntityData recordAt(int row) const = 0;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_STL_TABLE_PROXY_IMPLEMENTATION_INTERFACE_H
