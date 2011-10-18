
#include "mdtBuffer.h"
#include <cstring>
#include <string>

template <class T> mdtBuffer<T>::mdtBuffer()
{
  pvBufferStart = 0;
  pvCapacity = 0;
  pvRemainCapacity = 0;
  pvAvailable = 0;
  pvTokenReached = false;
}

template <class T> mdtBuffer<T>::mdtBuffer(mdtBuffer<T> &src)
{
  // Initialisation
  pvBufferStart = 0;
  pvCapacity = 0;
  pvRemainCapacity = 0;
  pvAvailable = 0;
  pvTokenReached = false;
  assert(init(src.capacity()));
  // Tampon de copie
  T *buffer = new T[src.available()];
  assert(buffer != 0);
  // Copie
  src.get(buffer, src.available());
  put(buffer, src.available());
  delete[] buffer;
}

template <class T> mdtBuffer<T> &mdtBuffer<T>::operator=(mdtBuffer<T> &src)
{
  // On effectue la copie uniquement si la source est une autre instance
  if(&src != this){
    // On initialise selon la source
    assert(init(src.capacity()));
    // Tampon de copie
    T *buffer = new T[src.available()];
    assert(buffer != 0);
    // Copie
    src.get(buffer, src.available());
    put(buffer, src.available());
    delete[] buffer;
  }
  return *this;
}

template <class T> mdtBuffer<T>::~mdtBuffer()
{
  if(pvBufferStart != 0){
    delete[] pvBufferStart;
  }
}

template <class T> bool mdtBuffer<T>::init(size_t size)
{
  // (Ré-)allocation mémoire si nécessaire
  if(size > pvCapacity){
    if(pvBufferStart != 0){
      delete[] pvBufferStart;
      pvBufferStart = 0;
    }
    pvBufferStart = new T[size];
    if(pvBufferStart == 0){
      pvCapacity = 0;
      return false;
    }
  }
  // Capacité initiale
  pvCapacity = size;
  // Effacement et positionnement des curseurs
  clear();

  return true;
}

template <class T> void mdtBuffer<T>::clear()
{
  //size_t i;

  // Effacement des données
  //for(i=0; i<pvCapacity; i++){
  //  pvBufferStart[i] = (T)0;
  //}
  pvAvailable = 0;
  pvRemainCapacity = pvCapacity;
  // Pointeur de fin de zone mémoire
  pvBufferEnd = pvBufferStart + pvCapacity-1;
  // Positions initiales des cureurs
  pvRdCursor = pvBufferStart;
  pvWrCursor = pvBufferStart;
  // Flags
  pvTokenReached = false;
}

template <class T> size_t mdtBuffer<T>::put(const T *data, size_t len)
{
  assert(data != 0);
  assert(pvBufferStart != 0);

  // Curseurs temporaires
  const T *c = data;
  // Longueur temporaire
  size_t len1;

  // Nombre d'éléments qu'il est possible de stocker
  if(len > pvRemainCapacity){
    len = pvRemainCapacity;
  }
  // Nouvelle capacité
  pvRemainCapacity = pvRemainCapacity - len;

  // Calcule de la zone disponible depuis le curseur jusqu'à la fin
  len1 = pvBufferEnd - pvWrCursor + 1;
  // On détermine la plus petite zone
  if(len < len1){
    len1 = len;
  }
  // Copie de la zone
  memcpy(pvWrCursor, c, len1*sizeof(T));
  // On repositionne les curseurs
  pvWrCursor = pvWrCursor + len1;
  if(pvWrCursor > pvBufferEnd){
    pvWrCursor = pvBufferStart;
  }
  c = c + len1;
  // On regarde s'il reste des données à copier
  if(len1 < len){
    len1 = len - len1;
    // Copie dans le début du tampon
    memcpy(pvBufferStart, c, len1*sizeof(T));
    // On repositionne le curseur d'écriture
    pvWrCursor = pvBufferStart + len1 - 1;
  }

  // Nouvelle qté de données disponibles
  pvAvailable = pvAvailable + len;

  assert(pvWrCursor >= pvBufferStart);
  assert(pvWrCursor <= pvBufferEnd);
  return len;
}

template <class T> bool mdtBuffer<T>::putOne(T data)
{
  // Nombre d'éléments qu'il est possible de stocker
  if(pvRemainCapacity < 1){
    return false;
  }
  // Copie
  *pvWrCursor = data;
  // Positionnement du curseur
  incWrCursor();
  // Nouvelle capacité
  pvRemainCapacity = pvRemainCapacity - 1;
  // Nouvelle qté de données disponibles
  pvAvailable = pvAvailable + 1;

  assert(pvWrCursor >= pvBufferStart);
  assert(pvWrCursor <= pvBufferEnd);
  return true;
}

template <class T> size_t mdtBuffer<T>::putUntil(const T *data, T token, size_t maxLen, bool IgnoreNullValues)
{
  assert(data != 0);

  // Curseurs temporaires
  const T *c = data;
  const T *end;
  // Nombre déléments écrits
  size_t written;

  // Nombre d'éléments qu'il est possible de stocker
  if(maxLen > pvRemainCapacity){
    maxLen = pvRemainCapacity;
  }
  // Curseurs
  end = data + maxLen - 1;

  // Lecture depuis data vers le tampon interne
  written = 0;
  while(c <= end){
    if((!IgnoreNullValues)||(*c != (T)0)){
      *pvWrCursor = *c;
      written++;
      incWrCursor();
    }
    if(*c == token){
      pvTokenReached = true;
      break;
    }
    c++;
  }
  // Nouvelle capacité
  pvRemainCapacity = pvRemainCapacity - written;
  // Nouvelle qté de données disponibles
  pvAvailable = pvAvailable + written;

  assert(pvWrCursor >= pvBufferStart);
  assert(pvWrCursor <= pvBufferEnd);
  return written;
}

template <class T> bool mdtBuffer<T>::tokenReached()
{
  return pvTokenReached;
}

template <class T> size_t mdtBuffer<T>::capacity()
{
  return pvCapacity;
}

template <class T> size_t mdtBuffer<T>::remainCapacity()
{
  return pvRemainCapacity;
}

template <class T> bool mdtBuffer<T>::full()
{
  return pvRemainCapacity == 0;
}

template <class T> size_t mdtBuffer<T>::available()
{
  return pvAvailable;
}

template <class T> size_t mdtBuffer<T>::get(T *data, size_t len)
{
  // Curseurs temporaires
  T *c = data;
  T *rd;
  // Longueur temporaire
  size_t len1;

  // Nombre d'éléments disponibles
  if(len > pvAvailable){
    len = pvAvailable;
  }
  // Positionnement des curseurs
  rd = pvRdCursor;

  // Calcule de la zone disponible depuis le curseur jusqu'à la fin
  len1 = pvBufferEnd - rd + 1;
  // On détermine la plus petite zone
  if(len < len1){
    len1 = len;
  }
  // Copie de la zone
  memcpy(c, rd, len1*sizeof(T));
  // On repositionne les curseurs
  rd = rd + len1;
  if(rd > rd){
    rd = rd;
  }
  c = c + len1;
  // On regarde s'il reste des données à copier
  if(len1 < len){
    len1 = len - len1;
    // Copie depuis le début du tampon
    memcpy(c, pvBufferStart, len1*sizeof(T));
  }

  return len;
}

template <class T> size_t mdtBuffer<T>::take(T *data, size_t len)
{
  // Curseurs temporaires
  T *c = data;
  // Longueur temporaire
  size_t len1;

  // Nombre d'éléments disponibles
  if(len > pvAvailable){
    len = pvAvailable;
  }
  // Calcule de la zone disponible depuis le curseur jusqu'à la fin
  len1 = pvBufferEnd - pvRdCursor + 1;
  // On détermine la plus petite zone
  if(len < len1){
    len1 = len;
  }
  // Copie de la zone
  memcpy(c, pvRdCursor, len1*sizeof(T));
  // On repositionne les curseurs
  pvRdCursor = pvRdCursor + len1;
  if(pvRdCursor > pvBufferEnd){
    pvRdCursor = pvBufferStart;
  }
  c = c + len1;
  // On regarde s'il reste des données à copier
  if(len1 < len){
    len1 = len - len1;
    // Copie depuis le début du tampon
    memcpy(c, pvBufferStart, len1*sizeof(T));
    // On repositionne le curseur de lecture
    pvRdCursor = pvBufferStart + len1 - 1;
  }

  // Calcule des qté de données disponibles
  pvAvailable = pvAvailable - len;
  // Calcule de la capacité disponible
  pvRemainCapacity = pvRemainCapacity + len;
  // Flags
  pvTokenReached = false;

  assert(pvRdCursor >= pvBufferStart);
  assert(pvRdCursor <= pvBufferEnd);
  return len;
}

template <class T>  T mdtBuffer<T>::takeOne()
{
  T c;

  // Nombre d'éléments disponibles
  if(pvAvailable < 1){
    return (T)0;
  }
  // Copie
  c = *pvRdCursor;
  // Mise à jours du curseur
  incRdCursor();

  // Calcule des qté de données disponibles
  pvAvailable = pvAvailable - 1;
  // Calcule de la capacité disponible
  pvRemainCapacity = pvRemainCapacity + 1;
  // Flags
  pvTokenReached = false;

  assert(pvRdCursor >= pvBufferStart);
  assert(pvRdCursor <= pvBufferEnd);
  return c;
}

// Instantiation explicite des templates (patrons) de fonctions (requis pour g++)

template mdtBuffer<char>::mdtBuffer();
template mdtBuffer<char>::mdtBuffer(mdtBuffer<char> &src);
template mdtBuffer<char> &mdtBuffer<char>::operator=(mdtBuffer<char> &src);
template mdtBuffer<char>::~mdtBuffer();
template bool mdtBuffer<char>::init(size_t size);
template void mdtBuffer<char>::clear();
template size_t mdtBuffer<char>::put(const char *data, size_t len);
template bool mdtBuffer<char>::putOne(char data);
template size_t mdtBuffer<char>::putUntil(const char *data, char token, size_t maxLen, bool IgnoreNullValues);
template bool mdtBuffer<char>::tokenReached();
template size_t mdtBuffer<char>::capacity();
template size_t mdtBuffer<char>::remainCapacity();
template bool mdtBuffer<char>::full();
template size_t mdtBuffer<char>::available();
template size_t mdtBuffer<char>::get(char *data, size_t len);
template size_t mdtBuffer<char>::take(char *data, size_t len);
template char mdtBuffer<char>::takeOne();

// Template std::string
template mdtBuffer<std::string>::mdtBuffer();
template mdtBuffer<std::string>::~mdtBuffer();
template bool mdtBuffer<std::string>::init(size_t size);
template void mdtBuffer<std::string>::clear();
template size_t mdtBuffer<std::string>::put(const std::string *data, size_t len);
template bool mdtBuffer<std::string>::putOne(std::string data);
template size_t mdtBuffer<std::string>::putUntil(const std::string *data, std::string token, size_t maxLen, bool IgnoreNullValues);
template bool mdtBuffer<std::string>::tokenReached();
template size_t mdtBuffer<std::string>::capacity();
template bool mdtBuffer<std::string>::full();
template size_t mdtBuffer<std::string>::remainCapacity();
template bool mdtBuffer<std::string*>::full();
template size_t mdtBuffer<std::string>::available();
template size_t mdtBuffer<std::string>::get(std::string *data, size_t len);
template size_t mdtBuffer<std::string>::take(std::string *data, size_t len);
template std::string mdtBuffer<std::string>::takeOne();
