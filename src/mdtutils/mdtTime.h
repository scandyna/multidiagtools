#ifndef MDT_TIME_H
#define MDT_TIME_H

#include <QTime>

/*! \brief Help class based on QTime
 */
class mdtTime : public QTime
{
 public:
  
  mdtTime();
    
  /*! \brief Stop the time count
   */
  void stop();
 
  /*! \brief Returns elapsed time since stop() was called [ms]
   * At first, start() must be called, than stop().
   * Note: this function is different that QTime::elapsed()
   * 
   * \return Time since last call of stop() [ms]
   */
  int elapsed() const;
  
  /*! \brief Returns elapsed time since stop() was called [s]
   * See: elapsed() for more details
   * 
   * \return Time since last call of stop() [s]
   */
  double elapsedSec();
  
 private:
  
  int pvElapsed;  // Time between start() and stop() [ms]
};

#endif
