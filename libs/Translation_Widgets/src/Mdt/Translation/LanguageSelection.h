/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_H
#define MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_H

#include "Mdt/FileSystem/PathList.h"
#include "Mdt/Translation/LanguageCode.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include "Mdt/Error.h"
#include "MdtTranslation_WidgetsExport.h"
#include <QObject>
#include <QList>
#include <vector>
#include <memory>

class QMenu;
class QToolBar;
class QAction;
class QWidget;
class QTranslator;
class QEvent;

namespace Mdt{ namespace Translation{

  /*! \brief LanguageSelection facilitates the creation of multi-language applications 
   *
   * \sa MultiLanguageMainWindow
   */
  class MDT_TRANSLATION_WIDGETS_EXPORT LanguageSelection : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit LanguageSelection(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~LanguageSelection();

    /*! \brief Find translation files
     *
     * Will search translations (*.qm files) in
     *  QCoreApplication::applicationDirPath() + "/translations"
     */
    bool findTranslations();

    /*! \brief Find translation files
     *
     * Will search translations (*.qm files) in
     *  a subdirectory, named translations,
     *  for each path in \a pathPrefixList .
     *
     * \sa Mdt::Translation::findTranslations()
     */
    bool findTranslations(const Mdt::FileSystem::PathList & pathPrefixList);

    /*! \brief Add available languages to \a menu
     */
    void addAvailableLanguagesToMenu(QMenu *menu);

    /*! \brief Add available languages to \a toolBar
     */
    void addAvailableLanguagesToToolBar(QToolBar *toolBar);

    /*! \brief Create language selection actions
     *
     * This method is automatically called by
     *  addAvailableLanguagesToMenu() and addAvailableLanguagesToToolBar()
     *  when needed.
     */
    void createLanguageSelectionActions(QObject *parent);

    /*! \brief Get language selection actions
     */
    QList<QAction*> languageSelectionActions() const
    {
      return mLanguageSelectionActions;
    }

    /*! \brief Handle the locale change event
     *
     * This method can be called from changeEvent()
     *  reimplentation.
     *  If \a event is of type QEvent::LocaleChange
     *  (occuring when system locale has changed),
     *  the language of the corresponding new locale will be loaded.
     */
    void handleChangeEvent(QEvent *event);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private slots:

    void onLanguageSelected(QAction *action);

   private:

    bool loadLanguage(const LanguageCode & languageCode);
    bool loadTranslations(const TranslationInfoList & translations);
    void unloadTranslations();
    void resizeTranslatorsList(int size);

    void setLastError(const Mdt::Error & error);

    LanguageCode mCurrentLanguage;
    TranslationInfoList mTranslations;
    QList<QAction*> mLanguageSelectionActions;
    std::vector< std::unique_ptr<QTranslator> > mTranslators;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_H
