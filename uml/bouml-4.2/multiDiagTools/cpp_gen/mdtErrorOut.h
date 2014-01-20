#ifndef _MDTERROROUT_H
#define _MDTERROROUT_H


class mdtError;

class mdtErrorOut {
  private:
    static mdtErrorOut pvInstance;

    QMutex pvMutex;

    QFile pvLogFile;

     mdtErrorOut();

     ~mdtErrorOut();

     mdtErrorOut(const mdtErrorOut & orig);

    QMessageBox pvDialog;


  public:
    static bool init(const QString & logFile);

    static void addError(const mdtError & error);

    static mdtErrorOut instance();


  private:
    mdtError::level_t pvLogLevelMask;

    mdtError::level_t pvDialogLevelMask;

};
#endif
