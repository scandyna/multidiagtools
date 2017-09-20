/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_CSV_COMMON_SETTINGS_H
#define MDT_PLAIN_TEXT_CSV_COMMON_SETTINGS_H

#include "Mdt/Error.h"
#include <QMetaType>

namespace Mdt{ namespace PlainText{

  /*! \brief Common CSV settings
   *
   * \note Some part of this API documentation
   *       refers to CSV-1203 standard.
   *       CSV-1203 is a open standard available here: http://mastpoint.com/csv-1203
   */
  class CsvCommonSettings
  {
   public:

    /*! \brief Construct default settings
     */
    CsvCommonSettings()
    : mFieldSeparator(','),
      mFieldProtection('\"')
    {
    }

    /*! \brief Construct settings as a copy of \a other
     */
    CsvCommonSettings(const CsvCommonSettings & other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvCommonSettings & operator=(const CsvCommonSettings & other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvCommonSettings(CsvCommonSettings && other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvCommonSettings & operator=(CsvCommonSettings && other) = default;

    /*! \brief Set field (or column) separator
     *
     * \sa fieldSeparator()
     */
    void setFieldSeparator(char separator);

    /*! \brief Get field (or column) separator
     *
     * As suggested by CSV-1203 standard, rule 4.3,
     *  the default field separator is comma ","
     *
     * \sa setFieldSeparator()
     */
    char fieldSeparator() const
    {
      return mFieldSeparator;
    }

    /*! \brief Set field payload protection
     */
    void setFieldProtection(char protection);

    /*! \brief Get field payload protection
     *
     * Field payload protection is explained
     *  in CSV-1203 standard, §9.
     *
     * The default is the double quote '"' .
     *
     * The ability to choose another protection
     *  is a non standard extention.
     */
    char fieldProtection() const
    {
      return mFieldProtection;
    }

    /*! \brief Check if settings are valid
     *
     * If this settings is not valid,
     *  use lastError() to get what is wrong.
     */
    bool isValid() const;

    /*! \brief Clear
     *
     * Will reset to default settings.
     */
    void clear();

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Set last error
     *
     * \note Will also commit error.
     */
    void setLastError(const Mdt::Error & error) const;

   private:

    char mFieldSeparator;
    char mFieldProtection;
    mutable Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace PlainText{
Q_DECLARE_METATYPE(Mdt::PlainText::CsvCommonSettings)

#endif // #ifndef MDT_PLAIN_TEXT_CSV_COMMON_SETTINGS_H
