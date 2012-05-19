#ifndef _MDTUICNUMBERVALIDATOR_H
#define _MDTUICNUMBERVALIDATOR_H


class mdtUicNumberValidator {
  public:
    mdtUicNumberValidator(const QObject & parent);

    ~mdtUicNumberValidator();

    State validate(const QString & input, const QString & pos);

};
#endif
