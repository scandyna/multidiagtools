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
#ifndef MDT_DEPLOY_UTILS_CONSOLE_STREAM_H
#define MDT_DEPLOY_UTILS_CONSOLE_STREAM_H

#include "MdtDeployUtils_CoreExport.h"
#include <QtGlobal>
#include <QString>
#include <memory>

class QDebug;

namespace Mdt{

  class Error;

} // namespace Mdt{

namespace Mdt{ namespace DeployUtils{

  /*! \brief Used to implement Console
   *
   * This class uses QDebug internally,
   *  but was designed to expose it in the public headers.
   *  This permits to use QDebug for debuging,
   *  and, when finished, remove the QDebug include,
   *  which permits the compiler to throw a error if we forgot a qDebug() somewhere.
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT ConsoleStream
  {
   public:

    /*! \brief Construct a stream
     */
    ConsoleStream(QtMsgType msgType, int minLevel, int level);

    /*! \brief Writes \a str to the stream and returns a reference to the stream
     */
    ConsoleStream & operator<<(const char * str);

    /*! \brief Writes \a str to the stream and returns a reference to the stream
     */
    ConsoleStream & operator<<(const QString & str);

    /*! \brief Writes \a error to the stream and returns a reference to the stream
     */
    ConsoleStream & operator<<(const Mdt::Error & error);

   private:

    std::shared_ptr<QDebug> mDbg;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_CONSOLE_STREAM_H
