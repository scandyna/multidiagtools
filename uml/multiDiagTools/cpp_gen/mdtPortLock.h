#ifndef _MDTPORTLOCK_H
#define _MDTPORTLOCK_H


class mdtPortLock {
  public:
    mdtPortLock();

    ~mdtPortLock();


  private:
    QFileInfo pvPort;

    QFileInfoList pvLockfiles;

    QStringList pvWritableLockDirectories;


  public:
    int openLocked(const QString & portName, int flags);

    void unlock();

    bool isLocked();


  private:
    void scanForLockDirectories();

    void buildLockFilesList();

};
#endif
