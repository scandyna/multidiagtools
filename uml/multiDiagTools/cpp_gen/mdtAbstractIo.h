#ifndef _MDTABSTRACTIO_H
#define _MDTABSTRACTIO_H


class mdtAbstractIo : public QObject {
  public:
     mdtAbstractIo(const QObject & parent = 0);

    virtual  ~mdtAbstractIo() = 0;


  private:
    int pvAddress;

    QString pvLabelShort;

    QString pvLabel;

    QString pvDetails;


  public:
    void setAddress(int adr);

    int address();

    void setLabelShort(const QString & text);

    QString labelShort();

    void setLabel(const QString & text);

    QString label();

    void setDetails(const QString & text);

    QString details();


  private:
    slot addressChangedForUi(int adr);

    void labelShortChangedForUi(const QString & text);

    void labelChangedForUi(const QString & text);

    void detailsChangedForUi(const QString & text);

};
#endif
