#ifndef _MDTFIELDMAPITEMDIALOG_H
#define _MDTFIELDMAPITEMDIALOG_H


class mdtFieldMapItem;
class mdtFieldMap;

class mdtFieldMapItemDialog : public QDialog, public Ui::mdtFieldMapItemDialog
{
  public:
  mdtFieldMapItemDialog(const QWidget & parent);

  ~mdtFieldMapItemDialog();

  void setSourceFields(const QList<mdtFieldMapField> & fields);

  void setDestinationFields(const QList<mdtFieldMapField> & fields);

  //For preview
  void setSourceData(const QString & data);

  //For preview
  void setDestinationData(const QString & data);

  void setMapItem(const mdtFieldMapItem & item);


  private:
    //Prévoir aussi une copie pour édition
    mdtFieldMapItem * pvMapItem;


  public:
    //Si édition Ok, copier les données dans le field map pointé
    virtual void accept();


  private:
  void updateDestination Preview();

  void setSrcSplitParameterWidgetsEnabled(bool enable);

};

#endif // #ifndef MDTFIELDMAPITEMDIALOG_H
