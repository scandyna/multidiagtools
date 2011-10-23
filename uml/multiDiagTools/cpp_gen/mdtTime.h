#ifndef _MDTTIME_H
#define _MDTTIME_H


class mdtTime : public QTime {
  private:
    int pvElapsed;


  public:
     mdtTime();

    void stop();

    int elapsed();

    double elapsedSec();

};
#endif
