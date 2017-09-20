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
#ifndef MDT_PLAIN_TEXT_CSV_PARSER_SETTINGS_H
#define MDT_PLAIN_TEXT_CSV_PARSER_SETTINGS_H

#include "CsvCommonSettings.h"
#include <QMetaType>

namespace Mdt{ namespace PlainText{

  /*! \brief CSV parser settings
   */
  class CsvParserSettings : public CsvCommonSettings
  {
   public:

    /*! \brief Construct default settings
     */
    CsvParserSettings()
     : CsvCommonSettings(),
       mParseExp(true)
    {
    }

    /*! \brief Construct settings as a copy of \a other
     */
    CsvParserSettings(const CsvParserSettings & other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvParserSettings & operator=(const CsvParserSettings & other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvParserSettings(CsvParserSettings && other) = default;

    /*! \brief Construct settings as a copy of \a other
     */
    CsvParserSettings & operator=(CsvParserSettings && other) = default;

    /*! \brief Set if Excel protection marker will be parsed
     *
     * \sa parseExp()
     */
    void setParseExp(bool parse);

    /*! \brief Check if Excel protection marker will be parsed
     *
     * The Excel protection marker (EXP) is explained
     *  in CSV-1203 standard, §10.
     *
     * When parseExp is true,
     *  when a field begins with a ~ (=EXP),
     *  it will not be stored in resulting data.
     *
     * The ability to not parse EXP
     *  is a non standard extention.
     *
     * \sa setParseExp()
     */
    bool parseExp() const
    {
      return mParseExp;
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

   private:

    bool mParseExp;
  };

}} // namespace Mdt{ namespace PlainText{
Q_DECLARE_METATYPE(Mdt::PlainText::CsvParserSettings)

#endif // #ifndef MDT_PLAIN_TEXT_CSV_PARSER_SETTINGS_H
