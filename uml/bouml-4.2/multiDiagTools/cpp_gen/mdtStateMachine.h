#ifndef _MDTSTATEMACHINE_H
#define _MDTSTATEMACHINE_H


class mdtState;

class mdtStateMachine : public QStateMachine {
  public:
    mdtStateMachine(const QObject & parent);

    virtual ~mdtStateMachine();

    void addState(const mdtState & state);

    int currentState() const;

    bool waitOnState(int state, int timeout);

    void stateChanged(int id);

    void stateChangedForUi(int id, const QString & text);


  private:
    void onStateEntered(const mdtState & state);

    QTimer pvTimeoutTimer;

    void onWaitTimeout();

    int pvCurrentState;

    bool pvTimeoutOccured;

};
#endif
