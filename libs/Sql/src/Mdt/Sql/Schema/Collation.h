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

#include <QLocale>
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Store information about a SQL collation
   */
  class Collation final
  {
   public:

    /*! \brief Default constructor
     */
    Collation()
    : pvIsCaseSensitive(true),
      pvWasEdited(false),
      pvCountry(QLocale::AnyCountry),
      pvLanguage(QLocale::AnyLanguage)
    {
    }

    /*! \brief Set case sensitivity
     */
    void setCaseSensitive(bool cs)
    {
      pvIsCaseSensitive = cs;
      pvWasEdited = true;
    }

    /*! \brief Check case sensitivity
     */
    bool isCaseSensitive() const
    {
      return pvIsCaseSensitive;
    }

    /*! \brief Set country
     */
    void setCountry(QLocale::Country country)
    {
      pvCountry = country;
      pvWasEdited = true;
    }

    /*! \brief Get country
     */
    QLocale::Country country() const
    {
      return pvCountry;
    }

    /*! \brief Set language
     */
    void setLanguage(QLocale::Language language)
    {
      pvLanguage = language;
      pvWasEdited = true;
    }

    /*! \brief Get language
     */
    QLocale::Language language() const
    {
      return pvLanguage;
    }

    /*! \brief Set charset
     */
    void setCharset(const QString & charset)
    {
      pvCharset = charset;
      pvWasEdited = true;
    }

    /*! \brief Get charset
     */
    QString charset() const
    {
      return pvCharset;
    }

    /*! \brief Check if null
     *
     * collate is null after default construction,
     *  or a call of clear(),
     *  until a attribute is set.
     */
    bool isNull() const
    {
      return !pvWasEdited;
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvIsCaseSensitive = true;
      pvCountry = QLocale::AnyCountry;
      pvLanguage = QLocale::AnyLanguage;
      pvCharset.clear();
      pvWasEdited = false;
    }

   private:

    uint pvIsCaseSensitive : 1;
    uint pvWasEdited : 1;
    QLocale::Country pvCountry;
    QLocale::Language pvLanguage;
    QString pvCharset;
  };

}}} //namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifnef MDT_SQL_SCHEMA_COLLATION_H
