#ifndef _MDTFILETEST_H
#define _MDTFILETEST_H


class mdtFileCopierTest;

class mdtFileTest : public mdtTest {
  private:
    void csvFileWriteTest();

    void csvFileReadTest();

    void mdtPartitionAttributesTest();

    void mdtFileCopierTest();

};
#endif
