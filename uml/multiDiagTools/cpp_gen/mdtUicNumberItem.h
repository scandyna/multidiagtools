#ifndef _MDTUICNUMBERITEM_H
#define _MDTUICNUMBERITEM_H


class mdtUicNumberItem {
  private:
    int pvNumber;

    QString pvCode;

    QString pvNameFr;

    QString pvNameEn;

    QString pvNameDe;


  public:
    mdtUicNumberItem();

    ~mdtUicNumberItem();

    void setNumber(int number);

    int number();

    void setCode(const QString & code);

    QString code();

    void setNameFr(const QString & code);

    QString nameFr();

    void setNameEn(const QString & code);

    QString nameEn();

    void setNameDe(const QString & code);

    QString nameDe();

};
#endif
