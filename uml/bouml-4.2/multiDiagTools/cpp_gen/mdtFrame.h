#ifndef _MDTFRAME_H
#define _MDTFRAME_H


class mdtFrame : public QByteArray {
  public:
    void appendUntil(char data, char token, int maxLen, bool ignoreNullValues);


  private:
    bool pvTokenReached;


  public:
    bool isFull();

    int remainCapacity();

    void trim();

};
#endif
