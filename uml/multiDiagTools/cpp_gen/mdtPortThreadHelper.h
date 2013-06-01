#ifndef _MDTPORTTHREADHELPER_H
#define _MDTPORTTHREADHELPER_H


class mdtPortThreadHelper {
  private:
    mdtFrame * pvCurrentReadFrame;

    mdtFrame * pvCurrentWriteFrame;

    mdtPortThread * pvThread;

    QAbstractSocket * pvSocket;


  public:
    void setPort(const mdtAbstractPort & port);

    void setThread(const mdtPortThread & thread);

    mdtAbstractPort::error_t handleCommonReadErrors(const mdtAbstractPort::error_t & portError, const  & );

    mdtAbstractPort::error_t handleCommonWriteErrors(const mdtAbstractPort::error_t & portError, const  & );

    mdtAbstractPort::error_t handleCommonReadWriteErrors(const mdtAbstractPort::error_t & portErros, const  & , const  & );

    mdtFrame getNewFrameRead();

    int submitReadenData(char data, int size, bool emitNewFrameReaden);

    mdtFrame getNewFrameWrite();

    mdtFrame currentReadFrame();

    mdtFrame currentWriteFrame();


  protected:
    virtual void requestWrite() = 0;

    void notifyError(int error, bool renotifySameError);


  private:
    mdtAbstractPort * pvPort;

};
#endif
