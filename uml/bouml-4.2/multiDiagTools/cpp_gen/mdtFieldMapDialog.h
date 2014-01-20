#ifndef _MDTFIELDMAPDIALOG_H
#define _MDTFIELDMAPDIALOG_H


#include "mdtFieldMap.h"

class mdtFieldMapItemDialog;

class mdtFieldMapDialog : public QDialog, public Ui::mdtFieldMapDialog
{
  public:
  mdtFieldMapDialog(const QWidget & parent);

  ~mdtFieldMapDialog();


  private:
    mdtFieldMap * pvCurrentFieldMap;

    //For memory management
    mdtFieldMap pvInternalFieldMap;


  public:
  void setFieldMap(const mdtFieldMap & map);

  mdtFieldMap fieldMap();


  private:
  void updateSourcePreview();

  void updateDestinationPreview();

};

#endif // #ifndef MDTFIELDMAPDIALOG_H
