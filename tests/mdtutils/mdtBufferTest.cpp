#include "mdtBufferTest.h"
#include "mdtTime.h"
#include <typeinfo>
#include <cstring>
#include <cfloat>
#include <cmath>

/*
 *  Template tests
 */

template <class T> bool mdtBufferTest<T>::initTest()
{
  bool ok;
  mdtBuffer<T> rb;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;
  
  // On vérifie les flags avant initialisation
  ok = VERIFY(rb.full(), "Verify that buffer full is true before init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that buffer remain capacity is 0 before init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that buffer available len is 0 before init");
  if(!ok){
    return false;
  }
  // Initialisation
  ok = VERIFY(rb.init(1024), "Verify that buffer initialisation works (size: 1024)");
  if(!ok){
    return false;
  }
  // On vérifie les flags après initialisation
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 1024, "Verify that buffer remain capacity is 1024 after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that buffer available len is 0 after init");
  if(!ok){
    return false;
  }
  // Vérifications après réinitialisation avec une taille plus petite
  ok = VERIFY(rb.init(512), "Verify that buffer initialisation works (size: 512)");
  if(!ok){
    return false;
  }
  // On vérifie les flags après initialisation
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 512, "Verify that buffer remain capacity is 512 after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that buffer available len is 0 after init");
  if(!ok){
    return false;
  }
  // Vérifications après réinitialisation avec une taille plus grande
  ok = VERIFY(rb.init(2000), "Verify that buffer initialisation works (size: 2000)");
  if(!ok){
    return false;
  }
  // On vérifie les flags après initialisation
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 2000, "Verify that buffer remain capacity is 2000 after init");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that buffer available len is 0 after init");
  if(!ok){
    return false;
  }
  
  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::readWriteTest1()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  // On remplis entièrement le buffer
  n = 0;
  n = rb.put(refData, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }
  // Lecture sans altération des disponibilités
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.get(data, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true after get");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0 after get");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10 after get");
  if(!ok){
    return false;
  }
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Lecture avec altération des disponibilités
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::readWriteTest2()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }

  // On remplis par paquets de 6 éléments
  n = 0;
  n = rb.put(refData, 6);
  ok = VERIFY(n == 6, "Verify that 6 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 4, "Verify that remain capacity is 4");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 6, "Verify that available is 6");
  if(!ok){
    return false;
  }
  n = 0;
  n = rb.put(&refData[6], 6);
  ok = VERIFY(n == 4, "Verify that 4 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }

  // Lecture par paquets de 6 éléments
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 6);
  ok = VERIFY(n == 6, "Verify that 6 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 6, "Verify that remain capacity is 6 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 4, "Verify that available is 4 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, refData, 6), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 6);
  ok = VERIFY(n == 4, "Verify that 4 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(compare(data, &refData[6], 4), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::readWriteTest3()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }

  // On dépose 8 éléments - 0,1,2,3,4,5,6,7
  n = 0;
  n = rb.put(refData, 8);
  ok = VERIFY(n == 8, "Verify that 8 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 2, "Verify that remain capacity is 2");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 8, "Verify that available is 8");
  if(!ok){
    return false;
  }
  // On consomme 3 éléments - 0,1,2
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 3);
  ok = VERIFY(n == 3, "Verify that 3 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 5, "Verify that remain capacity is 5 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 5, "Verify that available is 5 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, refData, 3), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // On dépose 4 éléments - 0,1,2,3
  n = 0;
  n = rb.put(refData, 4);
  ok = VERIFY(n == 4, "Verify that 4 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 1, "Verify that remain capacity is 1");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 9, "Verify that available is 9");
  if(!ok){
    return false;
  }
  // On consomme 3 éléments - 3,4,5
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 3);
  ok = VERIFY(n == 3, "Verify that 3 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 4, "Verify that remain capacity is 4 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 6, "Verify that available is 6 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, &refData[3], 3), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // On consomme les 6 éléments restants - 6,7,0,1,2,3
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 6);
  ok = VERIFY(n == 6, "Verify that 6 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues - 6,7
  ok = VERIFY(compare(data, &refData[6], 2), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // Vérification des données lues - 0,1,2,3
  ok = VERIFY(compare(&data[2], refData, 4), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::readWriteTestUntilToken()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.tokenReached() , "Verify that token reached is false");
  if(!ok){
    return false;
  }

  // On stocke jusqu'à l'élément 4 - Dispomible: 0,1,2,3,4 (5 éléments)
  n = 0;
  n = rb.putUntil(refData, (T)4 ,10, false);
  ok = VERIFY(n == 5, "Verify that 5 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.tokenReached() , "Verify that token reached is true");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 5, "Verify that remain capacity is 5");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 5, "Verify that available is 5");
  if(!ok){
    return false;
  }
  // On retire 2 éléments - 0,1 - Disponible: 2,3,4 (3 éléments)
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 2);
  ok = VERIFY(n == 2, "Verify that 2 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.tokenReached() , "Verify that token reached is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 7, "Verify that remain capacity is 7 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 3, "Verify that available is 3 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, refData, 2), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // On stocke jusqu'à l'élément 50 qui ne sera jamais trouvé
  // Il reste de la place pour 7 éléments - Dispomible: 2,3,4,0,1,2,3,4,5,6 (10 éléments)
  n = 0;
  n = rb.putUntil(refData, (T)50 ,10, false);
  ok = VERIFY(n == 7, "Verify that 7 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }
  // On lis tous les éléments - 2,3,4,0,1,2,3,4,5,6 (10 éléments)
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.get(data, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true after get");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0 after get");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10 after get");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(data[0] == (T)2, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[1] == (T)3, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[2] == (T)4, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  for(n=3; n<10; n++){
    ok = VERIFY(data[n] == (T)(n-3), "Verify that data matches the reference data");
    if(!ok){
      return false;
    }
  }
  // On consomme tous les éléments - 2,3,4,0,1,2,3,4,5,6 (10 éléments)
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(data[0] == (T)2, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[1] == (T)3, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[2] == (T)4, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  for(n=3; n<10; n++){
    ok = VERIFY(data[n] == (T)(n-3), "Verify that data matches the reference data");
    if(!ok){
      return false;
    }
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::readWriteTestUntilTokenNullChars()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = new T[10];
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.tokenReached() , "Verify that token reached is false");
  if(!ok){
    return false;
  }
  // Données de référence
  refData[0] = (T)2;
  refData[1] = (T)5;
  refData[2] = (T)0;
  refData[3] = (T)3;
  refData[4] = (T)0;
  refData[5] = (T)9;
  refData[6] = (T)25;
  refData[7] = (T)127;
  refData[8] = (T)-2;
  refData[9] = (T)0;

  // -- Test avec IgnoreNullChars true (les 0 ne sont pas stockés)

  // On stocke jusqu'à l'élément 48 qui ne sera jamais trouvé
  n = 0;
  n = rb.putUntil(refData, (T)48 ,10, true);
  ok = VERIFY(n == 7, "Verify that 7 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 3, "Verify that remain capacity is 3");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 7, "Verify that available is 7");
  if(!ok){
    return false;
  }
  // On consomme tous les éléments
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, rb.available());
  ok = VERIFY(n == 7, "Verify that 7 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues - but: 2,5,3,9,25,127,-2
  ok = VERIFY(data[0] == (T)2, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[1] == (T)5, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[2] == (T)3, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[3] == (T)9, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[4] == (T)25, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[5] == (T)127, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  ok = VERIFY(data[6] == (T)-2, "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // -- Test avec IgnoreNullChars false (les 0 sont stockés)

  // On stocke jusqu'à l'élément 48 qui ne sera jamais trouvé
  n = 0;
  n = rb.putUntil(refData, (T)48 ,10, false);
  ok = VERIFY(n == 10, "Verify that 10 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }
  // On consomme tous les éléments
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, rb.available());
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::copyTest()
{
  bool ok;
  mdtBuffer<T> rb1;
  T *refData;
  T *data;
  size_t i;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  // Buffer 1
  ok = VERIFY(rb1.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  rb1.put(refData, 10);
  // Vérification des données sur l'original
  for(i=0; i<10; i++){
    data[i] = (T)0;
  }
  rb1.get(data, rb1.available());
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // Copie par constructeur
  mdtBuffer<T> rb2(rb1);
  // Vérification des données sur l'original
  for(i=0; i<10; i++){
    data[i] = (T)0;
  }
  rb1.take(data, rb1.available());
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // Vérifications sur la copie
  ok = VERIFY(rb2.available() == 10, "Verify that decond buffer has correct data");
  if(!ok){
    return false;
  }
  for(i=0; i<10; i++){
    data[i] = (T)0;
  }
  ok = VERIFY(rb2.take(data, rb2.available()) == 10, "Verify that decond buffer has correct data");
  if(!ok){
    return false;
  }
  // On vérifie que le buffer 2 soit bien vide
  ok = VERIFY(rb2.available() == 0, "Verify that second buffer is empty");
  if(!ok){
    return false;
  }
  // On vérifie la cohérence des données
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  /*
    Essais avec l'opérateur =
  */
  rb1.clear();
  rb2.clear();

  // On dépose les données dans rb1
  rb1.put(refData, 10);
  ok = VERIFY(rb1.available() == 10, "Verify that first buffer has data");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb2.available() == 0, "Verify that second buffer is empty");
  if(!ok){
    return false;
  }
  // Copie
  rb2 = rb1;
  ok = VERIFY(rb2.available() == 10, "Verify that second buffer is not empty after copy");
  if(!ok){
    return false;
  }
  // Vérification sur la copie
  for(i=0; i<10; i++){
    data[i] = (T)0;
  }
  ok = VERIFY(rb2.take(data, rb2.available()) == 10, "Verify that decond buffer has correct data");
  if(!ok){
    return false;
  }
  // On vérifie que le buffer 2 soit bien vide
  ok = VERIFY(rb2.available() == 0, "Verify that second buffer is empty");
  if(!ok){
    return false;
  }
  // On vérifie la cohérence des données
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }
  // On vérifie que le buffer 1 ne soit pas vite
  ok = VERIFY(rb1.available() == 10, "Verify that first buffer is not empty");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::putOneTest()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }

  // On dépose un élément
  ok = VERIFY(rb.putOne(refData[0]), "Verify that element is written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 1, "Verify that available is 1");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 9, "Verify that remain capacity is 9");
  if(!ok){
    return false;
  }
  // On dépose les éléments restants
  for(n=1; n<10; n++){
    ok = VERIFY(rb.putOne(refData[n]), "Verify that element is written");
    if(!ok){
      return false;
    }
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.full(), "Verify that buffer full is true after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 0, "Verify that remain capacity is 0 after put");
  if(!ok){
    return false;
  }
  // Lecture de tous les éléments
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  n = 0;
  n = rb.take(data, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are readen");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.full(), "Verify that buffer full is false after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 0, "Verify that available is 0 after take");
  if(!ok){
    return false;
  }
  // Vérification des données lues
  ok = VERIFY(compare(data, refData, 10), "Verify that data matches the reference data");
  if(!ok){
    return false;
  }

  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::takeOneTest()
{
  bool ok;
  mdtBuffer<T> rb;
  T *refData;
  T *data;
  size_t n;

  cout << "**  Running " << __FUNCTION__ << " ..." << endl;

  // Initialisation
  refData = dataNumeric(10);
  ok = VERIFY(refData != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  data = new T[10];
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(10), "Verify that buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  // On dépose tous les éléments
  n = 0;
  n = rb.put(refData, 10);
  ok = VERIFY(n == 10, "Verify that 10 elements are written");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.available() == 10, "Verify that available is 10");
  if(!ok){
    return false;
  }
  // On vérifie la lecture
  for(n=0; n<10; n++){
    data[n] = (T)0;
  }
  for(n=0; n<10; n++){
    ok = VERIFY(rb.available() == (10-n), "Verify that available matches");
    if(!ok){
      return false;
    }
    ok = VERIFY(rb.takeOne() == (T)n, "Verify that the right element was returned");
    if(!ok){
      return false;
    }
  }
  ok = VERIFY(rb.remainCapacity() == 10, "Verify that remain capacity is 10 after take");
  if(!ok){
    return false;
  }
  ok = VERIFY(!rb.tokenReached(), "Verify that token reached flag is false after take");
  if(!ok){
    return false;
  }


  // Libérations
  delete[] refData;
  delete[] data;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> bool mdtBufferTest<T>::timingTest(size_t iterationCount, size_t dataLen)
{
  bool ok;
  mdtBuffer<T> rb;
  mdtTime t;
  size_t i;
  double dt1, dt2;
  double memcpyRate, ringBufferCopyRate;

  T *data;
  T *dataTmp;

  cout << "**  Running " << __FUNCTION__ << " with put() method ..." << endl;

  ok = VERIFY(iterationCount > 0 , "Verify that iteration count is min 1");
  if(!ok){
    return false;
  }

  // Initialisation
  data = dataNumeric(dataLen);
  ok = VERIFY(data != 0, "Verify ref data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  dataTmp = new T[dataLen];
  ok = VERIFY(dataTmp != 0, "Verify temp data buffer initialisation works (size: 10)");
  if(!ok){
    return false;
  }
  ok = VERIFY(rb.init(dataLen), "Verify that buffer initialisation works");
  if(!ok){
    return false;
  }

  // On utilise la technique de la double boucle pour les tests

  // Mesure avec memcpy()
  t.start();
  for(i=0; i<iterationCount; i++){
    memcpy(dataTmp, data, dataLen*sizeof(T));
  }
  t.stop();
  dt1 = t.elapsedSec();
  // Boucle "vide"
  t.start();
  for(i=0; i<iterationCount; i++){
    i=i;
  }
  t.stop();
  dt2 = t.elapsedSec();
  memcpyRate = displayDoubleLoopRateResults("** -> memcpy() ", iterationCount, dataLen, dt1, dt2);

  // Mesures avec le RingBuffer
  t.start();
  for(i=0; i<iterationCount; i++){
    // Ecriture
    rb.put(data, dataLen);
    // Lecture
    rb.take(dataTmp, dataLen);
  }
  t.stop();
  dt1 = t.elapsedSec() / 2.0; // On a effectué 2 copies é chaque itérations
  // Boucle "vide"
  t.start();
  for(i=0; i<iterationCount; i++){
    i=i;
  }
  t.stop();
  dt2 = t.elapsedSec();
  ringBufferCopyRate = displayDoubleLoopRateResults("** -> Ringbuffer: ", iterationCount, dataLen, dt1, dt2);

  // Comparaison
  cout << "** -> Comparison RinbufferRate / memcpyRate: " << ringBufferCopyRate / memcpyRate << endl;
  if(ringBufferCopyRate > memcpyRate){
    cout << "** -> Rinbuffer is " << ringBufferCopyRate / memcpyRate << " faster" << endl;
  }else{
    cout << "** -> memcpy() is " << memcpyRate / ringBufferCopyRate  << " faster" << endl;
  }

  // Libérations
  delete[] data;
  delete[] dataTmp;

  cout << "**  " << __FUNCTION__ << " passed" << endl;
  return true;
}

template <class T> T *mdtBufferTest<T>::dataNumeric(size_t len)
{
  size_t i;

  T *ptr = new T[len];
  if(ptr == 0){
    return 0;
  }

  // On remplis de données
  for(i=0; i<len; i++){
    ptr[i] = (T)i;
  }

  return ptr;
}

template <class T> bool mdtBufferTest<T>::compare(T *a, T *b, size_t len)
{
  size_t i;
  float *f = new float;
  double *d = new double;

  // On dois comparer les flotants différemment des autres types de base
  if((typeid(a) == typeid(f))||(typeid(b) == typeid(f))){
    for( i=0 ; i<len ; i++){
      // Check: if |a-b| > Epsilon  -> fail
      if( fabs(a[i]-b[i]) > FLT_EPSILON ){
        return false;
      }
    }
  }else if((typeid(a) == typeid(d))||(typeid(b) == typeid(d))){
    for( i=0 ; i<len ; i++){
      // Check: if |a-b| > Epsilon  -> fail
      if( fabs(a[i]-b[i]) > DBL_EPSILON ){
        return false;
      }
    }
  }else{
    for( i=0 ; i<len ; i++){
      if(a[i] != b[i]){
        return false;
      }
    }
  }

  delete f;
  delete d;
  return true;
}

template <class T> double mdtBufferTest<T>::displayDoubleLoopRateResults(string preMsg, size_t iterationCount, size_t elementsCountPerIteration, double workerLoopTime, double emptyLoopTime)
{
  double dt;
  double rate;

  // Temp effectif
  dt = fabs(workerLoopTime - emptyLoopTime);

  cout << preMsg << "Iteration count: " << iterationCount << " , number of elements per iteration [Me]: " << (double)elementsCountPerIteration/1e6;
  cout << " ([e]: " << elementsCountPerIteration << ")" << endl;
  cout << preMsg << "Total time [s]: " << dt << " , time per iteration [s]: " << dt/(double)iterationCount << std::endl;
  if(dt > 0.0){
    rate = (double)elementsCountPerIteration*(double)iterationCount/dt;
  }else{
    rate = 0.0;
  }
  cout << preMsg << "Rate: " << rate/1e6 << " [Me/s]" << endl;

  return rate;
}

/*
 * This tests are called by test lib
 */

void mdtBufferTestRun::charTypeTests()
{
  mdtBufferTest<char> test;
  
  // We run all tests from template test class
  QVERIFY(test.initTest());
  QVERIFY(test.readWriteTest1());
  QVERIFY(test.readWriteTest2());
  QVERIFY(test.readWriteTest3());
  QVERIFY(test.readWriteTestUntilToken());
  QVERIFY(test.readWriteTestUntilTokenNullChars());
  QVERIFY(test.copyTest());
  QVERIFY(test.putOneTest());
  QVERIFY(test.takeOneTest());
  QVERIFY(test.timingTest(10000000, 128));
  QVERIFY(test.timingTest(1000000, 1024));
  QVERIFY(test.timingTest(100000, 10000));
}

// Instantiation explicite des templates (patrons) de fonctions (requis pour g++)

template bool mdtBufferTest<char>::initTest();
template bool mdtBufferTest<char>::readWriteTest1();
template bool mdtBufferTest<char>::readWriteTest2();
template bool mdtBufferTest<char>::readWriteTest3();
template bool mdtBufferTest<char>::readWriteTestUntilToken();
template bool mdtBufferTest<char>::readWriteTestUntilTokenNullChars();
template bool mdtBufferTest<char>::copyTest();
template bool mdtBufferTest<char>::putOneTest();
template bool mdtBufferTest<char>::takeOneTest();
template bool mdtBufferTest<char>::timingTest(unsigned int, unsigned int);
