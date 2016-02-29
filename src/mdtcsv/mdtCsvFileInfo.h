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
#ifndef MDT_CSV_FILE_INFO_H
#define MDT_CSV_FILE_INFO_H

#include "mdtCsvSourceInfo.h"
#include "mdtCsvSettings.h"
#include "mdtError.h"
#include "mdtExpected.h"
#include <QFileInfo>
#include <QByteArray>

/*! \brief Provides informations about a CSV file
 */
class mdtCsvFileInfo : public mdtCsvSourceInfo
{
 public:

  /*! \brief Set file
   */
  mdtExpected<bool> setFile(const QFileInfo & fileInfo, const QByteArray & fileEncoding, const mdtCsvParserSettings & settings);

  /*! \brief Get file info
   */
  QFileInfo fileInfo() const
  {
    return pvFileInfo;
  }

  /*! \brief Get file encoding
   */
  QByteArray fileEncoding() const
  {
    return pvFileEncoding;
  }

 private:

  QFileInfo pvFileInfo;
  QByteArray pvFileEncoding;
};

#endif // #ifndef MDT_CSV_FILE_INFO_H
