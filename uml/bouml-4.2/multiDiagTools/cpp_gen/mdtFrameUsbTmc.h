#ifndef _MDTFRAMEUSBTMC_H
#define _MDTFRAMEUSBTMC_H


class mdtFrameUsbTmc : public mdtFrame {
  private:
    uint8 pvMsgID;

    uint8 pvbTag;

    uint8 pvbTagInverse;

    uint32 pvTransferSize;

    uint8 pvbmTransferAttributes;

    char pvTermChar;


  public:
    mdtFrameUsbTmc();

    ~mdtFrameUsbTmc();

    void clear();

    virtual int putData(char data, int maxLen);

    void setMsgID(const msg_id_t & MsgID);

    msg_id_t MsgID();

    void setbTag(const uint8 & bTag);

    uint8 bTag();


  private:
    bool pvEOM;


  public:
    void setEOM(bool EOM);

    bool isEOM();

    void encode(const  & );

};
#endif
