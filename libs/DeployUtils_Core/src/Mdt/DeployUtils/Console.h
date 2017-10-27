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
#include "MdtDeployUtils_CoreExport.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Display messages to the console
   *
   * Console can be used to display mesages to the user
   *  in a console application.
   *  It provides a basic filter mechanisme, based on levels.
   *
   * In the main:
   * \code
   * Console::setLevel(2);
   * \endcode
   * This configures Console to output all messages
   *  that are at least of level 2
   *  (messages of level 0, 1 and 2 are diplayed, messages with greater levels are not).
   *
   * Somewhere in the code:
   * \code
   * Console::info(1) << "Starting processing ...";
   * \endcode
   * This message will be displayed with obove configuration.
   *
   * \code
   * Console::info(3) << "  Format of file" << someFile << ": " << someFormat;
   * \endcode
   * This message will not be displayed with above configuration.
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT Console
  {
   public:

    /*! \brief Set level
     *
     * \pre \a level must be >= 0
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
     * \a minlevel is the minimum level required so that the message is diplayed.
     * \pre \a minlevel must be >= 0
     */
    static ConsoleStream info(int minlevel);

    /*! \brief Display a error
     */
    static ConsoleStream error();
    
   private:

    static int Level;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_CONSOLE_H
