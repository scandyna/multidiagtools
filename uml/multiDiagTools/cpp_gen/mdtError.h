#ifndef _MDTERROR_H
#define _MDTERROR_H


class mdtErrorOut;

class mdtError {
  private:
    int pvNumber;

    QString pvText;

    level_t pvLevel;

    int pvSystemNumber;

    QString pvSystemText;

    QString pvFunctionName;

    QString pvFileName;

    int pvFileLine;


  public:
     mdtError(int number, const QString & text, const level_t & level);

    void setSystemError(int number, const QString & text);

    void setSource(const QString & fileName, int fileLine, const QString & className, const QString & function);

    void commit();

    int number();

    QString text();

    level_t level();

    int systemNumber();

    QString systemText();

    QString functionName();

    QString fileName();

    int fileLine();

};
#endif
