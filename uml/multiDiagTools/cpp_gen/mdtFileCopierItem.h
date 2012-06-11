#ifndef _MDTFILECOPIERITEM_H
#define _MDTFILECOPIERITEM_H


class mdtFileCopierItem {
  public:
     mdtFileCopierItem(const QString & srcFilePath, const QString & destFilePath);

     ~mdtFileCopierItem();


  private:
    QString pvSrcFilePath;

    QString pvDestFilePath;

    qInt64 pvFileSize;

    bool pvSyncAfterCopy;

    bool pvCheckAfterCopy;


  public:
    void setFileSize(const qInt64 & size);

    QString srcFilePath();

    qInt64 fileSize();

    QString destFilePath();

    void setSyncAfterCopy(bool sync);

    bool syncAfterCopy();

    void setCheckAfterCopy(bool check);

    bool checkAfterCopy();

};
#endif
