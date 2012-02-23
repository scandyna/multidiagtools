#ifndef MDT_ABSTRACT_SERIAL_PORT_H
#define MDT_ABSTRACT_SERIAL_PORT_H

#include "mdtSerialPortConfig.h"
#include "mdtFrame.h"
#include <QObject>
#include <QQueue>

/// NOTE: closePort() à ajouter !!!!

class mdtAbstractSerialPort : public QObject
{
 Q_OBJECT

 public:

  mdtAbstractSerialPort(QObject *parent = 0);
  ~mdtAbstractSerialPort();

  /*! \brief Open the serial port with given configuration
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param cfg Contains the setup for the serial port to open
   * \return True on successfull configuration and open port
   */
  virtual bool openPort(mdtSerialPortConfig &cfg) = 0;

  /*! \brief Set the RX (data) timeout
   *
   * This method must be re-implemented in plateform specific subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setRxTimeout(int timeout) = 0;

  /*! \brief Set the TX (data) timeout
   *
   * This method must be re-implemented in plateform specific subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setTxTimeout(int timeout) = 0;

  /*! \brief Enable/diseable the RTS (Request To Send) signal
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param on If true, RTS will be enabled, diseabled else
   */
  virtual void setRts(bool on) = 0;

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param on If true, DTR will be enabled, diseabled else
   */
  virtual void setDtr(bool on) = 0;

  /*! \brief Wait until data is available at port
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventRx() = 0;

  /*! \brief Flush the RX port
   * 
   * This method must be re-implemented in plateform specific subclass.
   */
  virtual void flushRx() = 0;

  /*! \brief Read data from port
   * 
   * This method must be implemented in plateform specific subclass.
   * \return Number of bytes that were really readen
   */
  virtual int readData(char *data, int maxLen) = 0;

  /*! \brief Wait until data can be written to port
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventTxReady() = 0;

  /*! \brief Flush the TX port
   * 
   * This method must be re-implemented in plateform specific subclass.
   */
  virtual void flushTx() = 0;

  /*! \brief Write data to port
   * 
   * This method must be implemented in plateform specific subclass.
   * \return Number of bytes that were really written
   */
  virtual int writeData(const char *data, int maxLen) = 0;

  /*! \brief Wait until a control (modem line) signal state changes
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventCtl() = 0;

  /*! \brief Get the control (modem line) signal states and update member flags
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool getCtlStates() = 0;

  /*! \brief Update the RX timeout state
   *
   *  This method must be called by system dependant waitEventTxReady() method
   *  When the RX timeout state chages, the signal rxTimeoutStateChanged() is emited
   */
  void updateRxTimeoutState(bool state);

  /*! \brief Update the TX timeout state
   *
   *  This method must be called by system dependant waitEventRx() method
   *  When the TX timeout state chages, the signal txTimeoutStateChanged() is emited
   */
  void updateTxTimeoutState(bool state);

  /*! \brief Get the RX frames Queue
   *
   * RX frames queue contains frames that where received
   * Note that serialPort mutex is not handled by this method
   */
  QQueue<mdtFrame*> &rxFrames();

  /*! \brief Get the RX frames Queue pool
   *
   * RX frames queue pool contains frames that are ready to use for reception
   * Note that serialPort mutex is not handled by this method
   */
  QQueue<mdtFrame*> &rxFramesPool();

  /*! \brief Get the TX frames Queue
   *
   * TX frames queue contains frames that must be sent
   * Note that serialPort mutex is not handled by this method
   */
  QQueue<mdtFrame*> &txFrames();

  /*! \brief Get the TX frames Queue pool
   *
   * TX frames queue pool contains frames that are ready to use for transmission
   * Note that serialPort mutex is not handled by this method
   */
  QQueue<mdtFrame*> &txFramesPool();

 signals:

  /*!  \brief Emited when RX timeout state changed
   */
  void rxTimeoutStateChanged(bool state);

  /*!  \brief Emited when TX timeout state changed
   */
  void txTimeoutStateChanged(bool state);

  /*! \brief Emited whenn CAR (CD) status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void carChanged(bool on);

  /*! \brief Emited whenn DSR status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void dsrChanged(bool on);

  /*! \brief Emited whenn CTS status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void ctsChanged(bool on);

  /*! \brief Emited whenn RNG (RI) status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void rngChanged(bool on);

 protected:
  
  bool pvRxTimeoutOccured;
  bool pvRxTimeoutOccuredPrevious;
  bool pvTxTimeoutOccured;
  bool pvTxTimeoutOccuredPrevious;
  // Control signals states
  bool pvCarIsOn;
  bool pvDsrIsOn;
  bool pvCtsIsOn;
  bool pvRngIsOn;
  // Frames queues
  QQueue<mdtFrame*> pvRxFrames;
  QQueue<mdtFrame*> pvRxFramesPool;
  QQueue<mdtFrame*> pvTxFrames;
  QQueue<mdtFrame*> pvTxFramesPool;
};

#endif  // #ifndef MDT_ABSTRACT_SERIAL_PORT_H
