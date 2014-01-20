#ifndef _MDTTTTESTNODEUNITSETUPDATA_H
#define _MDTTTTESTNODEUNITSETUPDATA_H


class mdtTtTestNodeUnitSetupData
{
  private:
    QVariant pvId;

    QVariant pvState;

    QVariant pvValue;

    QVariant pvTestModelItemId;

    QVariant pvTestItemId;


  public:
  mdtTtTestNodeUnitSetupData();

  inline const QVariant Id() const;

  void setId(QVariant value);

  inline const QVariant State() const;

  void setState(QVariant value);

  inline const QVariant Value() const;

  void setValue(QVariant value);

  inline const QVariant TestModelItemId() const;

  void setTestModelItemId(QVariant value);

  inline const QVariant TestItemId() const;

  void setTestItemId(QVariant value);

};
inline const QVariant mdtTtTestNodeUnitSetupData::Id() const 
{
  return pvId;
}

inline const QVariant mdtTtTestNodeUnitSetupData::State() const 
{
  return pvState;
}

inline const QVariant mdtTtTestNodeUnitSetupData::Value() const 
{
  return pvValue;
}

inline const QVariant mdtTtTestNodeUnitSetupData::TestModelItemId() const 
{
  return pvTestModelItemId;
}

inline const QVariant mdtTtTestNodeUnitSetupData::TestItemId() const 
{
  return pvTestItemId;
}


#endif // #ifndef MDTTTTESTNODEUNITSETUPDATA_H
