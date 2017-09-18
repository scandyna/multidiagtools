#ifndef _MDTPORTMANAGERSTATEMACHINE_H
#define _MDTPORTMANAGERSTATEMACHINE_H


#include "mdtStateMachine.h"

class mdtState;

class mdtPortManagerStateMachine : public mdtStateMachine {
  private:
    mdtPortManager ;


  public:
    mdtPortManagerStateMachine(const QObject & parent);

    ~mdtPortManagerStateMachine();

    void buildMainStateMachine(const mdtPortManager & portManager);


  protected:
    void buildRunningSubMachine(const mdtPortManager & portManager, const mdtState & parentState);

    void buildConnectedSubMachine(const mdtPortManager & portManager, const mdtState & parentState);

};
#endif
