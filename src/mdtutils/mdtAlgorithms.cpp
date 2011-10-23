#include "mdtAlgorithms.h"

QStringList mdtAlgorithms::sortStringListWithNumericEnd(QStringList &list)
{
  // Make a first sort
  list.sort();
  // Now consider numeric end to sort
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
  // Extract end numerical part
  str1 = extractNumericPartAtEnd(str1);
  str2 = extractNumericPartAtEnd(str2);
  // On error, we return false
  if((str1.isEmpty())||(str2.isEmpty())){
    return false;
  }
  int num1, num2;
  bool ok = false;
  num1 = str1.toInt(&ok);
  if(!ok){
    return false;
  }
  ok = false;
  num2 = str2.toInt(&ok);
  if(!ok){
    return false;
  }
  // Result
  return (num1 < num2);
}
