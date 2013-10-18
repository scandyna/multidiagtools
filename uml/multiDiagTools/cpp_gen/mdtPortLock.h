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

    bool isLocked();

    void unlock();


  private:
    void scanForLockDirectories();

    void buildLockFilesList();

    bool pvIsLocked;

    bool createLockFile(const QFileInfo & file);

    struct flock pvLock;

    int pvFd;

};
#endif
