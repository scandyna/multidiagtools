#ifndef _MDTDEVICEINFO_H
#define _MDTDEVICEINFO_H


class mdtDeviceInfo {
  public:
    mdtDeviceInfo();

    ~mdtDeviceInfo();


  private:
    int pvVendorId;

    QString pvVendorName;

    int pvProductId;

    QString pvProductName;


  public:
    void setVendorId(int vid);

    int vendorId();

    QString vendorName();

    void setProductId(int pid);

    int productId();

    QString productName();

};
#endif
