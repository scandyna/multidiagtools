#ifndef MDT_PORT_MANAGER_H
#define MDT_PORT_MANAGER_H

#include <QString>
#include <QObject>
#include <QThread>
#include "mdtAbstractPort.h"
#include "mdtPortConfig.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtError.h"

#include "mdtFrame.h"
#include <QByteArray>


class mdtPortManager : public QThread
{
 Q_OBJECT

 public:

  mdtPortManager(QObject *parent = 0);
  ~mdtPortManager();

  /*! \brief Set port object
   */
  void setPort(mdtAbstractPort *port);

  /*! \brief Set port name
   *
   * Try to open given port. If ok, the port name
   * will be kept
   * \param portName Port to open
   * \return True on success, false else
   * \sa mdtPort
   */
  bool setPortName(const QString &portName);

  /*! \brief Get the config object
   * 
   * Usefull to alter internal port configuration
   */
  mdtPortConfig &config();

  /*! \brief Open the port
   * 
   * \return True on success, false else
   * \pre setPortName() must be called first
   * \sa setPortName()
   * \sa mdtPort
   */
  bool openPort();

  /*! \brief Close the port
   * 
   * This stops the threads and close the port.
   */
  void closePort();

  /*! \brief Start the read thread
   * \return True on successfull start
   * \sa mdtThread
   * \sa mdtPortReadThread
   */
  bool startReading();

  /*! \brief Stop the read thread
   * \sa startReading()
   */
  void stopReading();

  /*! \brief Start the write thread
   * \return True on successfull start
   * \sa mdtThread
   * \sa mdtPortWriteThread
   */
  bool startWriting();

  /*! \brief Stop the write thread
   * \sa startWriting()
   */
  void stopWriting();

  /*! \brief Start reading and writing threads
   * \sa startReading()
   * \sa startWriting()
   */
  bool start();

  /*! \brief Stop reading and writing threads
   * \sa start()
   */
  void stop();

  /*! \brief Write data by copy
   *
   * The data conatined in frame will be passed to the mdtPort's
   * write queue by copy. This method returns immediatly after enqueue,
   * and don't wait until data was written.
   * \param frame Frame to write
   * \return True on success. False if write queue is full
   */
  bool writeData(QByteArray data);

  /*! \brief Wait until a complete frame is available
   * 
   * This method will return when a complete frame was readen.<br>
   * This is usefull for query/answer protocols.<br>
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True if Ok, false on timeout
   */
  bool waitReadenFrame(int timeout = 500);

  /*! \brief Get the last readen frame
   */
  QByteArray &lastReadenFrame();

 public slots:

  /*! \brief Called by the read thread whenn a complete frame was readen
   * \sa mdtPortReadThread
   */
  void newFrameReaden();

  /*! \brief Manage errors comming from port threads
   */
  void onThreadsErrorOccured(int error);

 protected:

  mdtAbstractPort *pvPort;
  mdtPortReadThread *pvReadThread;
  mdtPortWriteThread *pvWriteThread;
  mdtPortConfig pvConfig;
  QByteArray pvLastReadenFrame;
};
#endif  // #ifndef MDT_PORT_MANAGER_H
