#ifndef _MDTFIELDMAPITEM_H
#define _MDTFIELDMAPITEM_H


class mdtFieldMapItem {
  public:
    mdtFieldMapItem();

    ~mdtFieldMapItem();


  private:
    int pvFieldIndex;

    QString pvFieldName;

    QString pvFieldDisplayText;

    int pvSourceFieldIndex;

    QString pvSourceFieldName;

    QString pvSourceFieldDisplayText;

    int pvSourceFieldDataStartOffset;

    int pvSourceFieldDataEndOffset;

    QVariant::Type pvDataType;


  public:
    void set_pvFieldIndex(int value);

    inline const int get_pvFieldIndex() const;

    void set_pvFieldName(QString value);

    inline const QString get_pvFieldName() const;

    void set_pvFieldDisplayText(QString value);

    inline const QString get_pvFieldDisplayText() const;

    void set_pvSourceFieldIndex(int value);

    inline const int get_pvSourceFieldIndex() const;

    void set_pvSourceFieldName(QString value);

    inline const QString get_pvSourceFieldName() const;

    void set_pvSourceFieldDisplayText(QString value);

    inline const QString get_pvSourceFieldDisplayText() const;

    void set_pvSourceFieldDataStartOffset(int value);

    inline const int get_pvSourceFieldDataStartOffset() const;

    void set_pvSourceFieldDataEndOffset(int value);

    inline const int get_pvSourceFieldDataEndOffset() const;

    void set_pvDataType(QVariant::Type value);

    inline const QVariant::Type get_pvDataType() const;

};
inline const int mdtFieldMapItem::get_pvFieldIndex() const {
  return pvFieldIndex;
}

inline const QString mdtFieldMapItem::get_pvFieldName() const {
  return pvFieldName;
}

inline const QString mdtFieldMapItem::get_pvFieldDisplayText() const {
  return pvFieldDisplayText;
}

inline const int mdtFieldMapItem::get_pvSourceFieldIndex() const {
  return pvSourceFieldIndex;
}

inline const QString mdtFieldMapItem::get_pvSourceFieldName() const {
  return pvSourceFieldName;
}

inline const QString mdtFieldMapItem::get_pvSourceFieldDisplayText() const {
  return pvSourceFieldDisplayText;
}

inline const int mdtFieldMapItem::get_pvSourceFieldDataStartOffset() const {
  return pvSourceFieldDataStartOffset;
}

inline const int mdtFieldMapItem::get_pvSourceFieldDataEndOffset() const {
  return pvSourceFieldDataEndOffset;
}

inline const QVariant::Type mdtFieldMapItem::get_pvDataType() const {
  return pvDataType;
}

#endif
