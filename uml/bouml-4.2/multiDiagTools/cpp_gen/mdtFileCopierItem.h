#ifndef _MDTFILECOPIERITEM_H
#define _MDTFILECOPIERITEM_H


class mdtFileCopierItem {
  public:
     mdtFileCopierItem(const QString & srcFilePath, const QString & destFilePath);

     ~mdtFileCopierItem();


  private:
    QString pvSrcFilePath;

    QString pvSrcFileName;

    QString pvDestFilePath;

    qInt64 pvFileSize;

    bool pvSyncAfterCopy;

    bool pvCheckAfterCopy;


  public:
    void setFileSize(const qInt64 & size);

    void setSrcFilePath(const QString & filePath);

    QString srcFilePath();

    void setSrcFileName(const QString & fileName);

    QString srcFileName();

    qInt64 fileSize();

    void setDestFilePath(const QString & filePath);

    QString destFilePath();

    void setSyncAfterCopy(bool sync);

    bool syncAfterCopy();

    void setCheckAfterCopy(bool check);

    bool checkAfterCopy();

};
#endif
