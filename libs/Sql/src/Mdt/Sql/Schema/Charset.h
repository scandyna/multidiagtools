/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_CHARSET_H
#define MDT_SQL_SCHEMA_CHARSET_H

#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL schema character set
   */
  class Charset
  {
   public:

    /*! \brief Set character set name
     */
    void setCharsetName(const QString & name)
    {
      pvCharsetName = name;
    }

    /*! \brief Get character set name
     */
    QString charsetName() const
    {
      return pvCharsetName;
    }

    /*! \brief Check if character set is null
     */
    bool isNull() const
    {
      return pvCharsetName.isEmpty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvCharsetName.clear();
    }

   private:

    QString pvCharsetName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_CHARSET_H
