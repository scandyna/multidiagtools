#ifndef MDT_FRAME_H
#define MDT_FRAME_H

#include <QByteArray>

/*! \brief Provide an array of bytes
 *
 * mdtFrame is based on Qt's QByteArray, that provide a array of bytes.
 * To store data, the QByteArray's methods append() can be used.<br>
 * For ASCII frames reception, the end is a special char/strting on most case. mdtFrame
 * provide the functions putUntil() for that purpose.<br>
 * During data reception, for exemple from serial port, the best way to check if more data can be stored is to use bytesToStore().
 * It returns the remain capacity of the frame, and 0 if the end of frame (EOF) condition was
 * reached. To know if the frame is complete (i.e. end of frame condition ok), use
 * isComplete()<br>
 * Here are the states of a frame with return values of both functions:<br>
 *  <table border="1" cellpadding="5">
 *   <tr><th>State</th><th>bytesToStore()</th><th>isComplete()</th></tr>
 *   <tr><td>Frame is not full, and EOF condition not reached</td><td>remainCapacity()</td><td>false</td></tr>
 *   <tr><td>Frame is full, and EOF condition not reached</td><td>0</td><td>false</td></tr>
 *   <tr><td>Frame is not full, and EOF condition reached</td><td>0</td><td>true</td></tr>
 *   <tr><td>Frame is full, and EOF condition reached</td><td>0</td><td>true</td></tr>
 *  </table>
 * At this state, whenn bytesToStore() returns 0 and isComplete() is true, the received frame is ok.
 * The next step is to decode it, according to the used protocol (modbus, ...).<br>
 * Note that these states are only set when using putUntil() functions.
 */
class mdtFrame : public QByteArray
{
 public:

  mdtFrame();
  ~mdtFrame();

  /*! \brief Put data until EOF token was reached
   *
   * This function is usefull for ASCII frames reception. Whenn token is reached, the EOF condition will become true (the EOF char is not stored).<br>
   * When the EOF condition was reached, this function does not store any more data.<br>
   * Note that this function is slower than QByteArray's append() method, because it compares each byte with the token.
   * \param data Pointer to source data
   * \param token Token that represents the EOF (End Of Frame)
   * \param maxLen Length of the array pointed by data
   * \param ignoreNullValues If true, null ('\0' or 0) values are not stored.
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  void putUntil(const char *data, char token, int maxLen, bool ignoreNullValues);

  /*! \brief Put data until EOF token was reached
   * 
   * This is an overloaded function. The token can be a sequence of bytes.
   * \sa putUntil()
   */
  void putUntil(const char *data, QByteArray &token, int maxLen, bool ignoreNullValues);

  /*! \brief Returns true if frame is full
   */
  bool isFull();

  /*! \brief Returns the remain capacity
   */
  int remainCapacity();

  /*! \brief Returns the bytes to store into the frame
   * Bytes to store can be the remainCapacity() value if frame is not full and EOF condition was not reached.
   * Else, it returns 0.
   */
  int bytesToStore();
  
  /*! \brief Returns true if EOF condition was reached
   */
  bool isComplete();

  /*! \brief Overloaded function
   * Internally, it calls QByteArray's clear() and reserve() functions.
   * The goal is to keep the same capacity and reset some mdtFrame specific flags
   */
  void clear();

  /*! \brief Take some data
   * \param data Pointer to data destination
   * \param len Number of bytes to take
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  int take(char *data, int len);

 private:

  bool pvEOFcondition;  // End Of Frame condition
};
#endif  // ifndef MDT_FRAME_H
