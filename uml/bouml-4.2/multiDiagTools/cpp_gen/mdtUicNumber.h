#ifndef _MDTUICNUMBER_H
#define _MDTUICNUMBER_H


class mdtUicNumber {
  private:
     pvCountries;

     pcUsages;

     pvTypes;

    QList<int> pvUicNumberDigits;


  public:
    mdtUicNumber();

    ~mdtUicNumber();

    bool setNumber(const QString & uicNumber);

    int controlKey(const  & );

    bool loadCountriesDb(const QString & dbPath);

    bool loadUsagesDb(const QString & dbPath);

    bool loadTypesDb(const QString & dbPath);


  private:
    <mdtUicNumberItem> ;


  public:
    mdtUicNumberItem country();

    mdtUicNumberItem usage();

    mdtUicNumberItem type();

};
#endif
