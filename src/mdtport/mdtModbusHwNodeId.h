/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_MODBUS_HW_NODE_ID_H
#define MDT_MODBUS_HW_NODE_ID_H

#include <vector>
#include <algorithm>

/** \todo Introduire struct de setup avec bistCount et startFrom ??
 * NOTE: semble spécific au MODBUS port(manager ?)
 * */

/*! \brief MODBUS hardware node ID
   *
   * The concept of node ID is specific to MultiDiagTools.
   *  It was introduced to solve the problem of network IP configurations.
   *
   * Take a example: We have a application that needs several MODBUS/TCP nodes.
   *  Each node has several I/O's.
   *  Say that digital output 1 of node 1 is used to activate a lamp
   *  and output 1 of node 2 must activate heating register.
   *  We could choose to have static IP addresses in the network and configure the
   *  application to activate the correct output of correct node.
   *  But, this can be a problem when a node must be replaced.
   *  More cumbersome case is a modular application that could manage several nodes,
   *  and change his I/Os layout in a dynamic way regarding a context.
   *
   * A easy way to solve such problems is to have a DHCP server in the network and
   *  all nodes configured as DHCP clients
   *   (static addressing can still be a solution, but we not depend on it).
   *  Then, for a given application, we can fix a number of digital inputs to play
   *  the role of node ID. Then, instead of using IP address as identification
   *  (as in the example), we use node ID.
 */
class mdtModbusHwNodeId
{
 public:

  /*! \brief Construct a null ID
   */
  mdtModbusHwNodeId()
   : pvId(0),
     pvBitsCount(0),
     pvFirstBit(0)
  {
  }

  /*! \brief Construct a ID
   */
  mdtModbusHwNodeId(int id, int bitsCount, int firstBit)
   : pvId(id),
     pvBitsCount(bitsCount),
     pvFirstBit(firstBit)
  {
    Q_ASSERT(id >= 0);
    Q_ASSERT(bitsCount >= 0);
    Q_ASSERT(firstBit >= 0);
    /// \todo Make a assertion that garanties bitsCount and firstBit are coherent to given ID
  }

  /*! \brief Check if ID is null
   */
  inline bool isNull() const
  {
    return (pvBitsCount == 0);
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvId = 0;
    pvBitsCount = 0;
    pvFirstBit = 0;
  }

  /*! \brief Set ID
   *
   * \param id The hardware node ID
   * \param bitsCount Number of bits that represent the ID
   * \param firstBit First bit, i.e. first digital input, that represent the node ID, starting from offset 0 (is the LSB)
   */
  void setId(int id, int bitsCount, int firstBit)
  {
    Q_ASSERT(id >= 0);
    Q_ASSERT(bitsCount >= 0);
    Q_ASSERT(firstBit >= 0);
    /// \todo Make a assertion that garanties bitsCount and firstBit are coherent to given ID

    pvId = id;
    pvBitsCount = bitsCount;
    pvFirstBit = firstBit;
  }

  /*! \brief Set ID
   *
   * Set node ID from binary representation given in bits.
   *  Will also update bitsCount attribute,
   *  but firstBit is keeped unchanged.
   *
   * \param bits Binary representation of node ID
   *             The first element in the vector (index 0) is the LSB
   * \param bitsCount Limit of element to consider in bits
   * \pre bistCount must be >= 0 and <= bits size.
   */
  void setId(const std::vector<bool> & bits, int bitsCount)
  {
    Q_ASSERT(bitsCount >= 0);
    Q_ASSERT((std::size_t)bitsCount <= bits.size());

    int id = 0;
    std::size_t bc = bitsCount;
    std::size_t i;

    pvBitsCount = bitsCount;
    for(i = 0; i < bc; ++i){
      if(bits[i] == true){
        id |= (1<<i);
      }
    }
    Q_ASSERT(id >= 0);
    pvId = id;
  }

  /*! \brief Set ID
   *
   * Set node ID from binary representation given in bits.
   *  Will also update bitsCount attribute,
   *  but firstBit is keeped unchanged.
   *
   * \param bits Binary representation of node ID
   *             The first element in the vector (index 0) is the LSB
   */
  inline void setId(const std::vector<bool> & bits)
  {
    setId(bits, bits.size());
  }

  /*! \brief Get ID
   */
  int id() const
  {
    return pvId;
  }

  /*! \brief Get bits count
   */
  int bitsCount() const
  {
    return pvBitsCount;
  }

  /*! \brief Get first bit position
   */
  int firstBit() const
  {
    return pvFirstBit;
  }

  /*! \brief ID comparison operator
   *
   * Note: will only compare id,
   *       other attributes like bistCount and firstBit are ignored.
   *
   * If current node ID is null, this function returns allways false.
   */
  bool operator==(int id) const
  {
    if(isNull()){
      return false;
    }
    return (id == pvId);
  }

  /*! \brief ID comparison operator
   *
   * \sa operator==()
   */
  inline bool operator!=(int id) const
  {
    return !operator==(id);
  }

 private:

  int pvId;
  int pvBitsCount;  // Number of digital inputs that represents the hardware node ID.
  int pvFirstBit;   // First digital input that represents the hardware node ID (is the LSB).
};

/*! \brief List of mdtModbusHwNodeId
 * 
 * \todo Vu que l'on peut avoir plusieurs noeuds different,
 *        bitsCount et firstBit peuvent être différents.
 *        -> Adapter
 */
class mdtModbusHwNodeIdList
{
 public:

  /*! \brief Construct a empty ID list
   */
  mdtModbusHwNodeIdList() {}

  /*! \brief Construct a empty ID list
   */
//   mdtModbusHwNodeIdList(int bitsCount, int firstBit)
//    : pvBitsCount(bitsCount),
//      pvFirstBit(firstBit)
//   {
//     Q_ASSERT(bitsCount >= 0);
//     Q_ASSERT(firstBit >= 0);
//   }

  /*! \brief Check if ID list is empty
   */
  bool isEmpty() const
  {
    return pvIdList.empty();
  }

  /*! \brief Get ID list size
   */
  int size() const
  {
    return pvIdList.size();
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvIdList.clear();
//     pvBitsCount = 0;
//     pvFirstBit = 0;
  }

  /*! \brief Set bitsCount and firstBit
   *
   * \param bitsCount Number of bits that represent the ID
   * \param firstBit First bit, i.e. first digital input, that represent the node ID, starting from offset 0 (is the LSB)
   */
//   void setBitsAttributes(int bitsCount, int firstBit)
//   {
//     Q_ASSERT(bitsCount >= 0);
//     Q_ASSERT(firstBit >= 0);
// 
//     pvBitsCount = bitsCount;
//     pvFirstBit = firstBit;
//   }

  /*! \brief Get bits count
   */
//   int bitsCount() const
//   {
//     return pvBitsCount;
//   }
// 
//   /*! \brief Get first bit position
//    */
//   int firstBit() const
//   {
//     return pvFirstBit;
//   }

  /*! \brief Append a ID at the end of the list
   */
  void append(const mdtModbusHwNodeId & nid)
  {
    Q_ASSERT(!nid.isNull());
    /// \todo Make a assertion that garanties bitsCount and firstBit are coherent to given ID

    pvIdList.emplace_back(nid);
  }

  /*! \brief Access internal vector
   *
   * Can be used, for example, to iterate all IDs:
   * \code
   *  for(const auto & nid : list.internalVector() ){
   *    if(nid.id() == 24){
   *      doSomeStuff(nid);
   *    }
   *  }
   * \endcode
   * Please take in accound that returned reference become invalid as soon as this mdtModbusHwNodeIdList object is destroyed.
   */
  const std::vector<mdtModbusHwNodeId> & internalVector() const
  {
    return pvIdList;
  }

  /*! \brief Check if given ID exists in list
   */
  bool contains(int id)
  {
    return ( std::find(pvIdList.cbegin(), pvIdList.cend(), id) != pvIdList.cend() );
  }

  /*! \brief Check if given ID exists in list
   */
  inline bool contains(const mdtModbusHwNodeId & mid)
  {
    if(mid.isNull()){
      return false;
    }
    return contains(mid.id());
  }

 private:

  std::vector<mdtModbusHwNodeId> pvIdList;
//   int pvBitsCount;  // Number of digital inputs that represents the hardware node ID.
//   int pvFirstBit;   // First digital input that represents the hardware node ID (is the LSB).
};

#endif // #ifndef MDT_MODBUS_HW_NODE_ID_H
