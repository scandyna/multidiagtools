/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_UIC_NUMBER_H
#define MDT_UIC_NUMBER_H

#include <QList>
#include <QMap>
#include <QString>

#include "mdtUicNumberItem.h"

/*! \brief Parse a UIC number and get details
 * 
 * \sa http://www.uic.org/spip.php?rubrique95
 * \sa http://en.wikipedia.org/wiki/UIC_wagon_numbers
 */
class mdtUicNumber
{
 public:

  mdtUicNumber();
  ~mdtUicNumber();

  /*! \brief Set the UIC number
   * 
   * The UIC number passed as argument
   * is first checked. If ok, it is stored as
   * list of digits.
   * Note: some chars are allowed (space, -),
   *  but alpha (a-b, A-B) are not valid.
   */
  bool setNumber(const QString &uicNumber);

  /*! \brief Get the complete formated UIC number
   * 
   * The UIC number must first be set with setNumber().
   * \return 7 or 12 digit formated UIC number, or \<unknow> on error
   */
  QString formatedUicNumber();

  /*! \brief Get 6 digit compact unformated UIC number
   * 
   * A 6 or 11 digit UIC number must first be set with setNumber().<br>
   * Note: if a 11 digits UIC number is set, the 6 last digit (without control key) are returned.<br>
   *  F.ex: if 94 85 7 560 256-0 is set, this method returns 560256.
   * \return 6 digit UIC number without space, without control key, or a empty string on error
   */
  QString compact6UicNumber();

  /*! \brief Get 11 digit compact unformated UIC number
   * 
   * A 11 digit UIC number must first be set with setNumber().
   * \return 11 digit UIC number without space, without control key, or a empty string on error
   */
  QString compact11UicNumber();

  /*! \brief Check the UIC number validity
   * 
   * A valid UIC number contains 6, 7, 11 or 12 digits.
   */
  bool isValid();

  /*! \brief Get the control key
   * 
   * The UIC number must be set before.
   * \return Control key, or value < 0 on error
   * \sa setNumber()
   */
  int controlKey();

  /*! \brief Get the control key
   * 
   * The UIC number must be set before.
   * \return Control key, \<unknow> on error
   * \sa controlKey()
   */
  QString controlKeyString();

  /*! \brief Get the serial number part
   * 
   * \return Serial number or \<unknow> on error
   */
  QString serialNumber();

  /*! \brief Load the coutries database
   * 
   * The database is a CSV file containing
   * the correspondance between 2 digits number
   * and the country.
   * \sa http://www.uic.org/IMG/xls/country_code_applicable.xls
   */
  bool loadCountriesDb(const QString &dbPath);

  /*! \brief Return true if Countries DB was successfully loaded
   */
  bool countriesDbLoaded();

  /*! \brief Load the usage database
   * 
   * The database is a CSV file containing
   * the correspondance between 2 digits number
   * and the usage.
   */
  bool loadUsagesDb(const QString &dbPath);

  /*! \brief Return true if usages DB was successfully loaded
   */
  bool usagesDbLoaded();

  /*! \brief Load the vehicule type database
   * 
   * The database is a CSV file containing
   * the correspondance between 2 digits number
   * and the vehicule type.
   */
  bool loadTypesDb(const QString &dbPath);

  /*! \brief Return true if types DB was successfully loaded
   */
  bool typesDbLoaded();

  /*! \brief Load the vehicule speend and heat
   * 
   * The database is a CSV file containing
   * the correspondance between 2 digits number
   * and the vehicule type.
   */
  bool loadSpeedAndHeatsDb(const QString &dbPath);

  /*! \brief Return true if speed and heat DB was successfully loaded
   */
  bool speedAndHeatDbLoaded();

  /*! \brief Load all needed DBs.
   * 
   * The directory that contains the databeses must be given as argument.
   * This directory must contain following files:
   *  - uic_countries_db.csv
   *  - uic_speedandheat_db.csv
   *  - uic_types_db.csv
   *  - uic_usages_db.csv
   */
  bool loadDbs(const QString &dir);

  /*! \brief Return true if all DBs was successfully loaded
   */
  bool dbsLoaded();

  /*! \brief Unload databases
   * 
   * This will free memory used by load*Db().
   * This method is automatically called by destructor.
   */
  void unloadDbs();

  /*! \brief Get the country
   * 
   * The UIC number must be set, and country database loaded
   * before this method works.
   */
  mdtUicNumberItem country();

  /*! \brief Get the usage
   * 
   * The UIC number must be set, and usage database loaded
   * before this method works.
   */
  mdtUicNumberItem usage();

  /*! \brief Get the vehicule type
   * 
   * The UIC number must be set, and type database loaded
   * before this method works.
   */
  mdtUicNumberItem type();

  /*! \brief Get the vehicule speed and heat
   * 
   * The UIC number must be set, and type database loaded
   * before this method works.
   */
  mdtUicNumberItem speedAndHeat();

 private:

  // Compute the control sum of 6 digits UIC number
  int controlSum6();
  // Compute the control sum of 11 digits UIC number
  int controlSum11();

  QMap<int, mdtUicNumberItem*> pvCountries;
  QMap<int, mdtUicNumberItem*> pvUsages;
  QMap<int, mdtUicNumberItem*> pvTypes;
  QMap<int, mdtUicNumberItem*> pvSpeedAndHeats;
  QList<int> pvUicNumberDigits;
};

#endif  // #ifndef MDT_UIC_NUMBER_H
