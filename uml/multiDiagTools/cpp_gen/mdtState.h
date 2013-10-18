#ifndef _MDTSTATE_H
#define _MDTSTATE_H


class mdtState : public QState {
  public:
    mdtState(int id, const QState & parent);

    ~mdtState();

    void setId(int id);

    int id() const;

    void setText(const QString & text);

    QString text() const;

    void setLedColorId(int colorId);

    int ledColorId() const;

    void setLedOn(bool state);

    bool ledOn();

    void setNotifyEnteredToUi(bool notify);

    bool notifyEnteredToUi() const;

    void entered(const mdtState & state);


  private:
    void onStateEntered();

    int pvId;

    QString pvText;

    bool pvNotifyEnteredToUi;

    int pvLedColorId;

    bool pvLedOn;

};
#endif
