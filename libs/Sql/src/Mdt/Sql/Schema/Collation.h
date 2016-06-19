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
#ifndef MDT_SQL_SCHEMA_COLLATION_H
#define MDT_SQL_SCHEMA_COLLATION_H

#include "CaseSensitivity.h"
#include "Locale.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Store information about a collation
   *
   * A collation is a set of rules to compare strings.
   *
   * For some DBMS, it is possible to define a collation
   *  with localisation (country, language).
   *  Some other support only case sensitivity collation option.
   *
   * For common DBMS, it is possible to specify collation
   *  for a database, a table or a field.
   *
   * \note You should only specify a locale (country and language)
   *        for special cases, and check that the DBMS can handle
   *        this locale with database default charset and locale.
   *        If no locale is specified in this collation,
   *        the driver will not generate locale COLLATE option
   *        for the table creation SQL, and let the DBMS choose the most appropriate
   *        collation.
   *
   * \todo Define better or remove language and country.
   * \todo If keep country/language, it must be possible to 
   *       know which part was edited (case sensitivity and/or country/loanguage)
   */
  class Collation final
  {
   public:

    /*! \brief Default constructor
     */
    Collation()
     : pvCaseSensitivity(CaseSensitivity::NotDefined)
    {
    }

    /*! \brief Set case sensitivity
     *
     * \note For some DBMS, like PostgreSQL, case insensitivity implies to use a CITEXT field type.
     */
    void setCaseSensitive(bool cs)
    {
      if(cs){
        pvCaseSensitivity = CaseSensitivity::CaseSensitive;
      }else{
        pvCaseSensitivity = CaseSensitivity::CaseInsensitive;
      }
    }

    /*! \brief Get case sensitivity
     */
    CaseSensitivity caseSensitivity() const
    {
      return pvCaseSensitivity;
    }

    /*! \brief Check case sensitivity
     */
    bool isCaseSensitive() const
    {
      return (pvCaseSensitivity == CaseSensitivity::CaseSensitive);
    }

    /*! \brief Set country
     */
    void setCountry(QLocale::Country country)
    {
      pvLocale.setCountry(country);
    }

    /*! \brief Get country
     */
//     QLocale::Country country() const
//     {
//       return pvCountry;
//     }

    /*! \brief Set language
     */
    void setLanguage(QLocale::Language language)
    {
      pvLocale.setLanguage(language);
    }

    /*! \brief Get language
     */
//     QLocale::Language language() const
//     {
//       return pvLanguage;
//     }

    /*! \brief Get locale
     */
    Locale locale() const
    {
      return pvLocale;
    }

    /*! \brief Set charset
     */
//     void setCharset(const QString & charset)
//     {
//       pvCharset = charset;
//       pvWasEdited = true;
//     }

    /*! \brief Get charset
     */
//     QString charset() const
//     {
//       return pvCharset;
//     }

    /*! \brief Check if null
     *
     * collate is null after default construction,
     *  or a call of clear(),
     *  until a attribute is set.
     */
    bool isNull() const
    {
      return ( (pvCaseSensitivity == CaseSensitivity::NotDefined) && pvLocale.isNull() );
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvCaseSensitivity = CaseSensitivity::NotDefined;
      pvLocale.clear();
    }

   private:

    CaseSensitivity pvCaseSensitivity;
    Locale pvLocale;
    
//     uint pvIsCaseSensitive : 1;
//     uint pvWasEdited : 1;
//     QLocale::Country pvCountry;
//     QLocale::Language pvLanguage;
//     QString pvCharset;
  };

}}} //namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifnef MDT_SQL_SCHEMA_COLLATION_H
