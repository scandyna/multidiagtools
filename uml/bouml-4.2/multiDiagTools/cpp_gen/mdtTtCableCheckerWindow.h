#ifndef _MDTTTCABLECHECKERWINDOW_H
#define _MDTTTCABLECHECKERWINDOW_H


class mdtTtCableChecker;

class mdtTtCableCheckerWindow : public QMainWindow, public Ui::mdtTtCableCheckerWindow
{
  public:
  mdtTtCableCheckerWindow(const QWidget & parent);

  void setCableChecker(const mdtTtCableChecker & cc);

};

#endif // #ifndef MDTTTCABLECHECKERWINDOW_H
