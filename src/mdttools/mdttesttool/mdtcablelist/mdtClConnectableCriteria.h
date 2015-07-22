/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_CONNECTABLE_CRITERIA_H
#define MDT_CL_CONNECTABLE_CRITERIA_H

/*! \brief Criteria that defines if 2 connectors are connectable
 *
 * \sa mdtClAutoConnection
 */
struct mdtClConnectableCriteria
{
  /*! \brief Contruct a default criteria struct.
   *
   * Will set parameters as follow:
   *  - checkGenderAreOpposite: true
   *  - checkContactCount: true
   *  - checkContactType: true
   *  - checkContactName: true
   *  - checkForm: true
   *  - checkInsert: true
   *  - checkInsertRotation: true
   */
  mdtClConnectableCriteria()
   : checkGenderAreOpposite(true),
     checkContactCount(true),
     checkContactType(true),
     checkContactName(true),
     checkForm(true),
     checkInsert(true),
     checkInsertRotation(true)
  {
  }

  /*! \brief Gender must be opposite
   *
   * If true, gender of both connectors must be opposite (male and female)
   *  Note: this concerns the connector gender, not necessarely the gender contacts.
   *   See checkContactType for contact gender checking.
   */
  bool checkGenderAreOpposite;

  /*! \brief Contact quantity must be the same
   *
   * If true, it will be checked that both connectors have the same quantity of contacts.
   */
  bool checkContactCount;

  /*! \brief Contact gender must be compatible
   *
   * If true, each contact gender are checked:
   *  - A socket (S) can be connected to a pin (P) , and reverse (P to T)
   *  - Terminal (T) can be connected to a terminal (T)
   */
  bool checkContactType;

  /*! \brief Contact name must match (be the same)
   */
  bool checkContactName;

  /*! \brief Form must match for both connectors
   *
   * If true, it will be check that both connectors have the same form
   */
  bool checkForm;

  /*! \brief Insert must match for both connector
   *
   * If true, it will be checked that both connectors have the same insert (f.ex. 24-5)
   */
  bool checkInsert;

  /*! \brief Insert rotation code must match for both connector
   *
   * If true, it will be checked that both connectors have the same insert rotation code (Q, X, W, ...)
   */
  bool checkInsertRotation;
};

#endif // #ifndef MDT_CL_CONNECTABLE_CRITERIA_H
