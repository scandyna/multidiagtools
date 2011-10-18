#ifndef ZMS_BUFFER_H
#define ZMS_BUFFER_H

// Macros pour la gestion des curseurs de lecture et écriture

#define incWrCursor() \
  if(pvWrCursor < pvBufferEnd){ \
    pvWrCursor++; \
  }else{  \
    pvWrCursor = pvBufferStart; \
  }

#define incRdCursor() \
  if(pvRdCursor < pvBufferEnd){ \
    pvRdCursor++; \
  }else{ \
    pvRdCursor = pvBufferStart; \
  }

#include <cstdlib>
#include "config.h"

// Utilisation d'assert selon config.h
#if USE_ASSERT == 1
 #undef NDEBUG
#else
 #define NDEBUG
#endif
#include <cassert>

/*! \brief Stockage d'éléments basé sur le principe du tampon circulaire
 *
 *  Stockage d'éléments basé sur le principe du tampon circulaire
 *  Idéal pour le stockage temporaire si une taille maximale est connue.
 *  Evite de nombreuses allocations/libération mémoire
*/
template <class T> class mdtBuffer
{
 public:

  /*! \brief Constructeur
   *
   *  Constructeur, utiliser init() avant tout autre opération
   */
  zmsBuffer();

  /*! \brief Constructeur de copie
   *
   *  Constructeur de copie. Effectue une copie des données vers l'autre instance
   */
  zmsBuffer(zmsBuffer<T> &src);

  /*! \brief Opérateur de copie
   *
   *  Opérateur de copie. Effectue une copie des données vers l'autre instance
   */
  zmsBuffer<T> &operator=(zmsBuffer<T> &src);

  /*! \brief Destructeur
   *
   *  Destructeur
   */
  ~zmsBuffer();

  /*! \brief  Initialisation du tampon
   *
   *  Initialisation du tampon. Alloue la mémoire,
   *  initialise les données à une valeur nulle.
   *  Il est possible de ré-appeler cette fonction plusieurs fois,
   *  la zone mémoire étant alors ré-alouée
   *
   *  \param size Capacité du tampon [nombre d'éléments]
   *
   *  \return false en cas de problème d'allocation mémoire (true si tout s'est bien passé)
   */
  bool init(size_t size);

  /*! \brief Efface le contenu du tampon
   *
   *  Efface le contenu du tampon. La quantité disponible
   *  ainsi que la capacité restante sont aussi ré-initialisés
   */
  void clear();

  /*! \brief Stockage d'un lot de données
   *
   *  Stockage d'un lot de données par copie.
   *  La copie, basée sur memcpy, est performante
   *
   *  \param data Pointeur vers les données sources
   *  \param len  Nombre d'éléments à stocker
   *
   *  \return Le nombre d'éléments effectivement stockés
   */
  size_t put(const T *data, size_t len);

  /*! \brief Stockage d'un élément par copie
   *
   *  Stockage d'un élément par copie.
   *
   *  \param data Elément à stocker
   *
   *  \return True si Ok, false en cas d'erreur
   */
  bool putOne(T data);

  /*! \brief Stockage d'un lot de données jusqu'à la rencontre d'un élément
   *
   *  Stockage d'un lot de données par copie jusqu'à la rencontre d'un élément,
   *  ou maxLen si l'élément recherché n'est pas trouvé.
   *  L'élément de recherche est aussi stocké
   *  La copie, basée sur une boucle, est relativement lente (comparé à memcpy)
   *  Il est possible de ne pas stocker les caractères null ('\0')
   *
   *  \param data   Pointeur vers les données sources
   *  \param token  Elément désignant la fin de la copie
   *  \param Maxlen Nombre d'éléments maximal à stocker
   *  \param IgnoreNullValues Si true, les éléments null (0) ne seront pas stockés
   *
   *  \return Le nombre d'éléments effectivement stockés
   */
  size_t putUntil(const T *data, T token, size_t maxLen, bool IgnoreNullValues);

  /*! \brief Retourne true si l'élément de fin de copie à été atteint
   *
   *  Retourne true si l'élément de fin de copie à été atteint
   *  Voir la méthode putUntil()
   *  Ce flag est effacé par un appel de la méthode take() ,
   *  même si l'élément n'est pas "consommé"
   *  clear() efface aussi ce flag
   *
   *  \return True si l'élément de fin à été atteint
   */
  bool tokenReached();

  /*! \brief Capacité totale
   *
   *  Capacité totale
   *
   *  \return La capacité totale du tampon
   */
  size_t capacity();

  /*! \brief Capacité restante
   *
   *  Capacité restante
   *
   *  \return Le nombre d'éléments qu'il est encore possible de stocker
   */
  size_t remainCapacity();

  /*! \brief Déterminer si le tampon est plein
   *
   *  Déterminer si le tampon est plein
   *
   *  \return True si le tampon est plein
   */
  bool full();

  /*! \brief Nombre d'éléments disponibles en lecture
   *
   *  Nombre d'éléments disponibles en lecture
   *
   *  \return Le nombre d'éléments qu'il est possible de lire
   */
  size_t available();

  /*! \brief Lecture d'un lot de données
   *
   *  La copie, basée sur memcpy, est performante
   *  Le cusreur de lecture interne, ainsi que le quantité
   *  disponible et la capacité restante ne sont pas altérés
   *  (plusieurs appels à cette méthode renverrait chaque fois les même données)
   *
   *  \param data Pointeur vers les données de destination
   *  \param len  Nombre d'éléments à lire
   *
   *  \return Le nombre d'éléments effectivement lus
   */
  size_t get(T *data, size_t len);

  /*! \brief Retrait d'un lot de données
   *
   *  Retrait d'un lot de données par copie.
   *  La copie, basée sur memcpy, et performante
   *  Le cusreur de lecture interne, ainsi que le quantité
   *  disponible et la capacité restante sont altérés
   *
   *  \param data Pointeur vers les données de destination
   *  \param len  Nombre d'éléments à lire
   *
   *  \return Le nombre d'éléments effectivement retirés
   */
  size_t take(T *data, size_t len);

  /*! \brief Retrait d'un élément
   *
   *  Retrait d'un élément
   *  Le cusreur de lecture interne, ainsi que le quantité
   *  disponible et la capacité restante sont altérés
   *  Test retour: 0 est retourné si aucun élément n'est disponible.
   *  Etant donnée que 0 peut très bien être un élément valide stocké dans le tampon, 
   *  il faut utiliser une méthode de test avant le retrait. Voir available()
   *
   *  \return Un élément, ou 0 si aucune disponibilité
   */
  T takeOne();

 private:

  size_t pvCapacity;
  size_t pvAvailable;
  size_t pvRemainCapacity;
  T *pvBufferStart;
  T *pvBufferEnd;
  T *pvRdCursor;
  T *pvWrCursor;
  bool pvTokenReached;
};
#endif
