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
#ifndef MDT_QACTION_ENABLE_STATE_GUARD_H
#define MDT_QACTION_ENABLE_STATE_GUARD_H

#include <vector>

class QAction;

/*! \brief Guard for QAction enable state
 *
 * Qt's QAction class provide a way to display
 *  a action in a menu, tool bar, etc.. in a central way,
 *  and keep state of each displayed widgets that concerns the action
 *  coherent. See Qt's QAction documentation for details.
 *
 * Take a example of a save function, witch must disable a save action during saving.
 *  Code could be something like:
 * \code
 *  void save()
 *  {
 *    actionSave->setEnabled(false);
 *    // Some code here ...
 *    if(someCondition){
 *      actionSave->setEnabled(true);
 *      return;
 *    }
 *    // Some code here ...
 *    // End
 *    actionSave->setEnabled(true);
 *  }
 * \endcode
 *
 * If our save function becomes a bit complex,
 *  it's easy to foget to re-enable save action.
 *  Rewriting the code with a action guard could be:
 * \code
 *  void save()
 *  {
 *    mdtQActionEnableStateGuard ag(action, false);  // Initially we disable the action
 *    // Some code here ...
 *    if(someCondition){
 *      return; // action state will be reversed here (i.e. re-enabled)
 *    }
 *    // Some code here ...
 *    // End
 *    // action state will be reversed here (i.e. re-enabled)
 *  }
 * \endcode
 */
class mdtQActionEnableStateGuard
{
 public:

  /*! \brief Construct a action guard
   *
   * Will also apply initialState to action.
   */
  mdtQActionEnableStateGuard(QAction *action, bool initialState);

  /*! \brief Destructor
   *
   * Will reverse action state
   *  (If is enabled, it will be desabled and vice versa).
   */
  ~mdtQActionEnableStateGuard();

 private:

  QAction *pvAction;
};

/*! \brief A list of mdtQActionEnableStateGuard
 */
class mdtQActionEnableStateGuardList
{
 public:

  /*! \brief Destructor
   *
   * Will reverse action state for each added guard
   *  (If is enabled, it will be desabled and vice versa).
   */
  ~mdtQActionEnableStateGuardList();

  /*! \brief Add a action enable state guard
   */
  void append(QAction *action, bool initialState);

 private:

  std::vector<mdtQActionEnableStateGuard*> pvGuards;
};

#endif // #ifndef MDT_QACTION_ENABLE_STATE_GUARD_H
