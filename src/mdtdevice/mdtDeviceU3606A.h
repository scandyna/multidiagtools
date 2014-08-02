/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_DEVICE_U3606A_H
#define MDT_DEVICE_U3606A_H

#include "mdtDeviceScpi.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtValue.h"

/*! \brief Representation of a Agilent U3606A
 *
 * The U3606A has following I/Os
 *
 *  <table border="1" cellpadding="5">
 *   <tr><th>Type</th><th>Address</th><th>Label short</th><th>Description</th></tr>
 *   <tr><td>Analog input</td><td>0</td><td>MEASURE</td><td>Represent the measurement part</td></tr>
 *   <tr><td>Analog input</td><td>1</td><td>SENSEU</td><td>Represent the voltage sense of the source part</td></tr>
 *   <tr><td>Analog input</td><td>2</td><td>SENSEI</td><td>Represent the current sense of the source part</td></tr>
 *   <tr><td>Analog output</td><td>0</td><td>SOURCE</td><td>Represent the source part</td></tr>
 *  </table>
 *
 */
class mdtDeviceU3606A : public mdtDeviceScpi
{
 Q_OBJECT

 public:

  /*! \brief Range
   */
  enum range_t {
                  RangeAuto = -10,  /*!< AUTO range */
                  RangeMin,         /*!< MIN range */
                  RangeMax,         /*!< MAX range */
                  Range100 = 1,     /*!< 100 range */
                  Range1k,          /*!< 1k range */
                  Range10k,         /*!< 10k range */
                  Range100k,        /*!< 100k range */
                  Range1M,          /*!< 1M range */
                  Range10M,         /*!< 10M range */
                  Range100M         /*!< 100M range */
               };

  /*! \brief Resolution
   */
  enum resolution_t {
                  ResolutionMin,    /*!< 5 1/2 digit */
                  ResolutionMax     /*!< 4 1/2 digit */
                    };

  /*! \brief Constructor
   *
   * Will create the needed codec, I/Os and configure internal port manager .
   */
  mdtDeviceU3606A(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtDeviceU3606A();

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached maching request.
   *
   * \param devInfo Requested device's informations.
   *                 Note: Vendor ID and/or product ID are automatically set.
   *                       (in other words, a empty mdtDeviceInfo can be passed).
   *                       Optionnaly, a serial ID can be set (usefull if many U3606A devices are connected)
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Setup resistance measure
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   * \return Value < 0 on error, >= 0 on success
   */
  int setupResistanceMeasure(range_t range, resolution_t resolution);

  /*! \brief Get the measure configuration
   *
   * Will get the device current measure configuration
   *  by sending the CONF? query .
   *
   * Internal analog measure input is also updated with device's range and unit .
   *
   * By error, unknown measure type is returned .
   */
  mdtFrameCodecScpiU3606A::measure_type_t getMeasureConfiguration();

  /*! \brief Get measure value
   *
   * This is similar to call getAnalogInputValue("MEASURE", true, true) .
   */
  mdtValue getMeasureValue();

 public slots:

  /*! \brief Update (G)UI when device's state has changed
   */
  void onStateChanged(int state);

 private slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   */
  void decodeReadenFrame(mdtPortTransaction *transaction);

 private:

  /*! \brief Read one analog input on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogInput().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details).
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readAnalogInput(mdtPortTransaction *transaction);

  /*! \brief Sequence of queries to send periodically
   *
   * This method is called from runQueries().
   *
   * \return true if all queries are sent successfully.
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing and allways returns false.
   *  - This method can be reimplemented periodic queries must be sent to device.
   */
  bool queriesSequence();

  /*! \brief Update the measure I/O setup in internal container
   *
   * This method is called when a QT_CONF reply was decoded .
   */
  void updateMeasureSetup();

  /*! \brief Handle device's specific error
   *
   * This method is called from handleDeviceError().
   */
  ///void handleDeviceSpecificError();

  Q_DISABLE_COPY(mdtDeviceU3606A);

  mdtFrameCodecScpiU3606A *pvCodec;
};

#endif  // #ifndef MDT_DEVICE_U3606A_H
