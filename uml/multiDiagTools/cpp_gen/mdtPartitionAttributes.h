#ifndef _MDTPARTITIONATTRIBUTES_H
#define _MDTPARTITIONATTRIBUTES_H


class mdtPartitionAttributes {
  private:
    QString pvRootPath;

    QString pvName;

    bool pvIsReadOnly;

    QString pvFileSystem;


  public:
    mdtPartitionAttributes();

    ~mdtPartitionAttributes();

    bool setPath(const QString & path);

    QString rootPath(bool nativeFormat);

    QString name();

    bool isReadOnly();

    bool isWriteable();

    QString fileSystem();

    static QStringList availablePartitions();

};
#endif
