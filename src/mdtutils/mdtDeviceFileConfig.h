#ifndef MDT_DEVIC_EFILE_CONFIG_H
#define MDT_DEVIC_EFILE_CONFIG_H

#include <QString>
#include <QByteArray>
#include "mdtFrame.h"

class mdtDeviceFileConfig
{
 public:

  mdtDeviceFileConfig();
  ~mdtDeviceFileConfig();

  /*! \brief Set default configuration
   * 
   * Default configuration is:
   *  - interface: null (empty string)
   *  - Read and write frame size: 1024
   *  - Read and write queue size: 10
   *  - Frame type: ASCII   NOTE: chage this ?
   *  - En of frame sequence: '\0' (null char)
   */
  void setDefault();

  /*! \brief Set interface
   * Interface can be /dev/ttyS0 (on Posix) ,
   * or COM1 (On Windows)
   */
  void setInterface(const QString &interface);

  /*! \brief Get intarfce
   *
   * \sa setInterface()
   */
  QString interface();

  /*!\brief Set the read frame size
   *
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full. A full frame that never received
   * a EOF condition is considered invalid
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setReadFrameSize(int size);

  /*!\brief Get the read frame size
   * \sa setReadFrameSize()
   */
  int readFrameSize();

  /*!\brief Set the read Queue size
   *
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be received before they are threated
   * \pre size must be a positive value
   */
  void setReadQueueSize(int size);

  /*!\brief Get the read Queue size
   * \sa setReadQueueSize()
   */
  int readQueueSize();

  /*! \brief Set the minimal time to wait before try to read
   *
   * Internally, the event system is used for read calls.<br>
   * For exemple, on Linux, the select() call is used.<br>
   * It can happen that the underlaying system does not support
   * it, and call returns immediately, causing the reader thread
   * to consume all CPU ressource.<br>
   * In this situation, the solution is to wait sometime before calling the read function.
   * This wat is done by setting this parameter to a value greater than 0.
   * \param minWaitTime Wait time before read [ms]
   * \sa mdtDeviceFileReadThread
   */
  void setReadMinWaitTime(int minWaitTime);

  /*! \brief Get the minimal time to wait before try to read
   * \sa setReadMinWaitTime()
   */
  int readMinWaitTime();

  /*! \brief Set the minimal time to wait before try to write
   *
   * Internally, the event system is used for write calls.<br>
   * It can be usefull (or important) to delay the write call.
   * In this case, set a minWaitTime > 0
   * \param minWaitTime Wait time before write [ms]
   * \sa mdtDeviceFileWriteThread
   */
  void setWriteMinWaitTime(int minWaitTime);

  /*! \brief Get the minimal time to wait before try to write
   * \sa setWriteMinWaitTime()
   */
  int writeMinWaitTime();

  /*!\brief Set the write frame size
   *
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full.
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setWriteFrameSize(int size);

  /*!\brief Get the read frame size
   * \sa setWriteFrameSize()
   */
  int writeFrameSize();

  /*!\brief Set the read Queue size
   *
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be enqueued to serial port output.
   * \pre size must be a positive value
   */
  void setWriteQueueSize(int size);

  /*!\brief Get the write Queue size
   * \sa setWriteQueueSize()
   */
  int writeQueueSize();

  /*! \brief Set frame type
   *
   * Frame type can be an ASCII frame, binary frame, ...
   * \sa mdtFrame
   */
  void setFrameType(const mdtFrame::type_t type);

  /*! \brief Get frame type
   * \sa setFrameType()
   */
  mdtFrame::type_t frameType();

  /*! \brief Set the end of frame sequence (applies on ASCII frames)
   */
  void setEndOfFrameSeq(const QByteArray & seq);

  /*! \brief Set the end of frame sequence (applies on ASCII frames)
   * 
   * This is an overloaded method
   * \sa setEndOfFrameSeq()
   */
  void setEndOfFrameSeq(char c);

  /*! \brief Get the end of frame sequence
   * 
   * \sa setEndOfFrameSeq()
   */
  QByteArray endOfFrameSeq();

  bool operator==(const mdtDeviceFileConfig &other);
  bool operator!=(const mdtDeviceFileConfig &other);


 protected:

  QString pvInterface;    // Interface, like: /dev/ttyS0 or COM1
  // Frame and frame FIFO (queue) size
  int pvReadFrameSize;        // Maximum data length to store before a frame is considered invalid
  int pvReadQueueSize;        // Maximum number of frames that can be stored
  int pvReadMinWaitTime;      // Minimum time to wait before read call
  int pvWriteFrameSize;       // Maximum data length to store before a frame is considered invalid
  int pvWriteQueueSize;       // Maximum number of frames that can be stored
  int pvWriteMinWaitTime;     // Minimum time to wait before write call
  mdtFrame::type_t pvFrameType;
  QByteArray pvEndOfFrameSeq; // End of frame sequence (valid for ASCII frames)

 private:

  bool matches(const mdtDeviceFileConfig &other);
};

#endif  // #ifndef MDT_DEVIC_EFILE_CONFIG_H
