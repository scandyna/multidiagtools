#include "mdtAlgorithms.h"

QStringList mdtAlgorithms::sortStringListWithNumericEnd(QStringList &list)
{
  qSort(list.begin(), list.end(), stringWithNumericEndLessThan);

  return list;
}

QString mdtAlgorithms::extractNumericPartAtEnd(const QString &str)
{
  QString num;

  if(str.size() < 1){
    return num;
  }
  for(int i=str.size()-1; i>=0; --i){
    if(str.at(i).isDigit()){
      num.prepend(str.at(i));
    }else{
      if(str.at(i) != ' '){
        break;
      }
    }
  }

  return num;
}

bool mdtAlgorithms::stringWithNumericEndLessThan(QString str1, QString str2)
{
  // Empty string
  if(str1.isEmpty()&&(!str2.isEmpty())){
    return true;
  }
  if((!str1.isEmpty()) && str2.isEmpty()){
    return false;
  }
  if((str1.isEmpty())&&(str2.isEmpty())){
    return false;
  }

  // Extract end numerical part
  QString strNum1, strNum2;
  strNum1 = extractNumericPartAtEnd(str1);
  strNum2 = extractNumericPartAtEnd(str2);

  // Extract begin part
  int len;
  len = str1.size() - strNum1.size();
  if(len >= 0){
    str1 = str1.mid(0, len);
  }
  len = str2.size() - strNum2.size();
  if(len >= 0){
    str2 = str2.mid(0, len);
  }
  // Compare the begin part
  if(str1 < str2){
    return true;
  }
  if(str2 < str1){
    return false;
  }

  // Begin part of str1 and str2 are the same, check on numerical end part
  int num1, num2;
  bool ok = false;

  // Error or empty string on numeric part
  if(strNum1.isEmpty()&&(!strNum2.isEmpty())){
    return true;
  }
  if((!strNum1.isEmpty()) && strNum2.isEmpty()){
    return false;
  }
  if((strNum1.isEmpty())&&(strNum2.isEmpty())){
    return false;
  }
  num1 = strNum1.toInt(&ok);
  if(!ok){
    return false;
  }
  ok = false;
  num2 = strNum2.toInt(&ok);
  if(!ok){
    return false;
  }
  // Result
  return (num1 < num2);
}
