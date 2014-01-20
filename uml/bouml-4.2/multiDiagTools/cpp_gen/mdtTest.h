#ifndef _MDTTEST_H
#define _MDTTEST_H


class mdtTest : public QObject {
  public:
    static bool verify(bool statement, char statementStr, char description, char file, int line);

};
#endif
