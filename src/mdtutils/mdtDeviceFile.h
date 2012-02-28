#ifndef MDT_DEVICE_FILE_H
#define MDT_DEVICE_FILE_H

#include <QObject>
#include <QString>
#include "mdtDeviceFileConfig.h"

#ifdef Q_OS_UNIX
 #include "linux/mdtDeviceFilePosix.h"
 #define mdtDeviceFileSys mdtDeviceFilePosix
#endif

class mdtDeviceFile : public mdtDeviceFileSys
{
 Q_OBJECT

 public:

  /*! \brief Construct a new device file object
   *  R/W timeouts are set to 500 ms
   */
  mdtDeviceFile(QObject *parent = 0);
  ~mdtDeviceFile();

  /*! \brief Open device file
   * 
   * Note: this method locks the internal mutex
   * \param path Path to device file
   * \return True if successfull
   */
  bool open(mdtDeviceFileConfig &cfg);

  /*! \brief Close the device file
   * 
   * Note: this method locks the internal mutex
   */
  void close();
  
  /// NOTE: provisoire
  int getFd();
};

#endif  // #ifndef MDT_DEVICE_FILE_H
