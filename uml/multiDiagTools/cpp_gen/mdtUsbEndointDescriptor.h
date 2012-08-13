#ifndef _MDTUSBENDOINTDESCRIPTOR_H
#define _MDTUSBENDOINTDESCRIPTOR_H


class mdtUsbEndointDescriptor {
  private:
    uint8 pvbEndpointAddress;

    uint8 pvbmAttributes;

    uint16 pvwMaxPacketSize;

    uint8 pvbInterval;


  public:
    mdtUsbEndointDescriptor();

    ~mdtUsbEndointDescriptor();

    void fetchAttributes(const libusb_endpoint_descriptor & descriptor);

    uint8 address();

    bool isDirectionIN();

    bool isDirectionOUT();

    bool isTransfertTypeControl();

    bool isTransfertTypeIsochronus();

    bool isTransfertTypeBulk();

    bool isTransfertTypeInterrupt();

    bool isAsynchronous();

    bool isAdaptative();

    bool isSynchronous();

    bool isDataEndpoint();

    bool isFeedbackEndpoint();

    bool isImplicitFeedbackEndpoint();

    int maxPacketSize();

    int transactionsCountPerMicroFrame();

    uint8 bInterval();

};
#endif
