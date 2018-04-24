/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_TEST_LIB_COMPARE_HELPER_H
#define MDT_TEST_LIB_COMPARE_HELPER_H

#include "MdtTestLib_CoreExport.h"
#include <QString>

namespace Mdt{ namespace TestLib{

  /*! \brief Helper class to create %qCompare() overloads
   *
   * \note Some methods will call Qt::compare_helper()
   *   which is part of the private Qt API.
   *   Using CompareHelper can reduce the amount of work if the Qt API changes.
   */
  class MDT_TESTLIB_CORE_EXPORT CompareHelper
  {
   public:

    /*! \brief Call of Qt compare helper
     */
    static bool compareOk(char *val1, char *val2,
                          const char *actual, const char *expected,
                          const char *file, int line);

    /*! \brief Call of Qt compare helper
     */
    static bool compareOk(const char *actual, const char *expected,
                          const char *file, int line)
    {
      return compareOk(nullptr, nullptr, actual, expected, file, line);
    }

    /*! \brief Call of Qt compare helper
     */
    static bool compareNok(const QString & failureMsg,
                          char *val1, char *val2,
                          const char *actual, const char *expected,
                          const char *file, int line);

    /*! \brief Call of Qt compare helper
     */
    static bool compareNok(const QString & failureMsg,
                          const char *actual, const char *expected,
                          const char *file, int line)
    {
      return compareNok(failureMsg, nullptr, nullptr, actual, expected, file, line);
    }

  };

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_COMPARE_HELPER_H
