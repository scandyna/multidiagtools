/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_LINK_DATA_H
#define MDT_CL_LINK_DATA_H

#include <QVariant>
#include <QList>
#include <QPair>
#include <QStringList>

/*! \brief Container class for Link data
 */
class mdtClLinkData
{
 public:

  /*! \brief Check data validity
   *
   * Data object is valid if:
   *  - unitConnectionStartId is set
   *  - unitConnectionEndId is set
   *  - linkDirectionCode is set
   *  - linkTypeCode is set
   */
  bool isValid();

  /*! \brief Set article link ID
   */
  void setArticleLinkId(const QVariant & value);

  /*! \brief Get article ID
   */
  QVariant articleLinkId() const;

  /*! \brief Set unit connection start ID
   */
  void setUnitConnectionStartId(const QVariant & value);

  /*! \brief Get unit connection start ID
   */
  QVariant unitConnectionStartId() const;

  /*! \brief Set unit connection end ID
   */
  void setUnitConnectionEndId(const QVariant & value);

  /*! \brief Get unit connection end ID
   */
  QVariant unitConnectionEndId() const;

  /*! \brief Set since version
   */
  void setSinceVersion(const QVariant & value);

  /*! \brief Get since version
   */
  QVariant sinceVersion() const;

  /*! \brief Set modification
   */
  void setModification(const QVariant & value);

  /*! \brief Get modification
   */
  QVariant modification() const;

  /*! \brief Set identification
   */
  void setIdentification(const QVariant & value);

  /*! \brief Get identification
   */
  QVariant identification() const;

  /*! \brief Set code of link direction
   */
  void setLinkDirectionCode(const QVariant & value);

  /*! \brief Get code of link direction
   */
  QVariant linkDirectionCode() const;

  /*! \brief Set code of link type
   */
  void setLinkTypeCode(const QVariant & value);

  /*! \brief Get code of link type
   */
  QVariant linkTypeCode() const;

  /*! \brief Set link value
   */
  void setValue(const QVariant & value);

  /*! \brief Get link value
   */
  QVariant value() const;

  /*! \brief Set the start list of vehicle type IDs
   */
  void setVehicleTypeStartIdList(const QList<QVariant> & idList);

  /*! \brief Add a start vehicle type ID
   */
  void addVehicleTypeStartId(const QVariant &id);

  /*! \brief Get the start list of vehicle type IDs
   */
  const QList<QVariant> &vehicleTypeStartIdList() const;

  /*! \brief Set the end list of vehicle type IDs
   */
  void setVehicleTypeEndIdList(const QList<QVariant> & idList);

  /*! \brief Add a end vehicle type ID
   */
  void addVehicleTypeEndId(const QVariant &id);

  /*! \brief Get the end list of vehicle type IDs
   */
  const QList<QVariant> &vehicleTypeEndIdList() const;

  /*! \brief Build the list with pairs of start and end vehicle type IDs
   *
   * The list is built regarding distinct cases:
   *  - If both start and end vehicle types lists contains exactly the same count of items,
   *     each start ID is put together with each end ID , in the order of both lists.
   *  - If one vehicle types list has only 1 item, and the other has many items,
   *     the ID of the 1 item list is copied to the other list
   *     (case of 1 vehicle type linked to many vehicle types) .
   *  - All other cases are wrong, and a empty list will also be returned .
   *     In this case, errorList will contain the error message.
   *
   * To get the list, use vehicleTypeStartEndIdList() .
   * 
   * \todo Check if actions should be done by dialog ?
   */
  bool buildVehicleTypeStartEndIdList();

  /*! \brief Get the list with pairs of start and end vehicle type IDs
   *
   * Call buildVehicleTypeStartEndIdList() before getting the list.
   */
  const QList<QPair<QVariant, QVariant> > vehicleTypeStartEndIdList() const;

 private:

  /*! \brief Check about vehicle types errors
   */
  bool checkVehicleTypes();

  ///QVariant pvLinkId;
  QVariant pvArticleLinkId;
  QVariant pvUnitConnectionStartId;
  QVariant pvUnitConnectionEndId;
  QVariant pvSinceVersion;
  QVariant pvModification;
  QVariant pvIdentification;
  QVariant pvLinkDirectionCode;
  QVariant pvLinkTypeCode;
  QVariant pvValue;
  QList<QVariant> pvVehicleTypeStartIdList;
  QList<QVariant> pvVehicleTypeEndIdList;
  QList<QPair<QVariant, QVariant> > pvVehicleTypeStartEndIdList;
  QStringList pvErrorList;
};

#endif  // #ifndef MDT_CL_LINK_DATA_H
