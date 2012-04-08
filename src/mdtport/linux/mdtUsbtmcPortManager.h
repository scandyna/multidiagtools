#ifndef MDT_USBTMC_PORT_MANAGER_H
#define MDT_USBTMC_PORT_MANAGER_H


#include "mdtUsbtmcPort.h"
#include "mdtPortManager.h"
#include <QObject>
#include <QStringList>

/*! \brief USBTMC port manager
 */
class mdtUsbtmcPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct the manager
   * 
   * The port and configuration instances will be created here
   */
  mdtUsbtmcPortManager(QObject *parent = 0);
  ~mdtUsbtmcPortManager();

  /*! \brief Scan available USBTMC ports
   */
  QStringList scan();

  /*! \brief Write data to device
   *
   * By sending data, it's important to tell if
   * the device will return some data back.
   * This is due to large timeout of USBTMC kernel driver
   * whenn no data are available on port.
   * \sa mdtPortManager
   * \sa mdtUsbtmcPort
   */
  bool writeData(QByteArray data, bool waitAnswer);

 public slots:

  // Called by write thread whenn data was written
  void frameWritten();
  
 private:

  bool pvFrameWritten;
  bool pvWaitingFrame;  // If true, frameWritten() will enable the read thread flag
};

#endif  // #ifndef MDT_USBTMC_PORT_MANAGER_H
