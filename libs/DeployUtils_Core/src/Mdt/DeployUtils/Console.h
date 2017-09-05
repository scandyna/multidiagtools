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
#ifndef MDT_DEPLOY_UTILS_CONSOLE_H
#define MDT_DEPLOY_UTILS_CONSOLE_H

#include "ConsoleStream.h"
// class QDebug;

namespace Mdt{ namespace DeployUtils{

//   class ConsoleStream;

  /*! \brief Display messages to the console
   */
  class Console
  {
   public:

    /*! \brief Set level
     */
    static void setLevel(int level);

    /*! \brief Get level
     */
    static int level()
    {
      return Console::Level;
    }

    /*! \brief Display a information
     *
     * \a minlevel
     */
    static ConsoleStream info(int minlevel);
//     static QDebug info(int minlevel);

   private:

    static int Level;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_CONSOLE_H
