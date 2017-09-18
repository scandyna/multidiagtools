#ifndef _MDTFIELDMAPFIELD_H
#define _MDTFIELDMAPFIELD_H


class mdtFieldMapField
{
  private:
    QSqlField pvSqlField;

    int pvIndex;

    QString pvDisplayText;


  public:
  void setIndex(int value);

  inline const int Index() const;

  void setDisplayText(QString value);

  inline const QString DisplayText() const;

  void setName(const QString & name);

  QString name();

  void setSqlField(const QSqlField & field);

  QSqlField sqlField();

};
inline const int mdtFieldMapField::Index() const 
{
  return pvIndex;
}

inline const QString mdtFieldMapField::DisplayText() const 
{
  return pvDisplayText;
}


#endif // #ifndef MDTFIELDMAPFIELD_H
