#ifndef _MDTSQLFIELDHANDLER_H
#define _MDTSQLFIELDHANDLER_H


class mdtSqlFieldHandler : public QObject {
  private:
    QSqlField * pvSqlField;

    bool pvIsNull;

    bool pvDataEdited;


  public:
    mdtSqlFieldHandler(const QObject & parent);

    ~mdtSqlFieldHandler();

    void setField(const QSqlField & field);

    void setDataWidget(const QLineEdit & widget);

    void setDataWidget(const QAbstractButton & widget);

    void setDataWidget(const QDateTimeEdit & widget);

    void setDataWidget(const QDoubleSpinBox & widget);

    void setDataWidget(const QSpinBox & widget);

    void clear();

    bool isNull();

    bool dataWasEdited();

    bool checkBeforeSubmit();

    void dataEdited();

    void onCurrentIndexChanged(int index);


  private:
    void onDataEdited(const QString & text);

    void onDataEdited(bool state);

    void onDataEdited(const QDateTime & datetime);

    QLineEdit * pvLineEdit;

    QAbstractButton * pvAbstractButton;

    QDateTimeEdit * pvDateTimeEdit;

    QDoubleSpinBox * pvDoubleSpinBox;

    QSpinBox * pvSpinBox;

    QComboBox * pvComboBox;

};
#endif
