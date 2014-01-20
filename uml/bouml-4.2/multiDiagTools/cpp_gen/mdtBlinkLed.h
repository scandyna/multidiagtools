#ifndef _MDTBLINKLED_H
#define _MDTBLINKLED_H


class mdtBlinkLed : public mdtLed {
  public:
    mdtBlinkLed(const QWidget & parent);


  private:
    QTimer pvTimer;

    int pvBlinkHalfPeriod;


  public:
    void setBlinkPeriod(int T);

    void setBlinking(bool on);

};
#endif
