#ifndef _MDTFILECOPIER_H
#define _MDTFILECOPIER_H


class mdtFileCopierItem;

class mdtFileCopier : public QThread {
  public:
     mdtFileCopier(const QObject & parent = 0);

     ~mdtFileCopier();


  private:
    FILE pvSrcFile;

    FILE pvDestFile;

    QFileInfo pvSrcFileInfo;

    QFileInfo pvDestFileInfo;

    QMutex pvMutex;

    QMessageBox pvMessageBox;

    QProgressDialog pvProgressDialog;

    bool pvShowProgressDialog;

    <mdtFileCopierItem> pvCopies;

    bool pvDirectDestOverwrite;

    bool pvAllDialogsDiseabled;


  public:
    void setShowProgressDialog(bool show);

    void setDirectDestOverwrite(bool directOverwrite);

    void diseableAllDialogs(bool diseable);

    void setDefaultSetup();

    void addCopy(const QString & srcFilePath, const QString & destFilePath, bool syncAfterCopy, bool checkAfterCopy);

    void startCopy();

    bool waitFinished();

    void finished(bool success);


  private:
    bool sync();

    virtual void run();

    <mdtFileCopierItem> pvCopiesInProcess;

};
#endif
