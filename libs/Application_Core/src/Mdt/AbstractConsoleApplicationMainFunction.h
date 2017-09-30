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
#ifndef MDT_ABSTRACT_CONSOLE_APPLICATION_MAIN_FUNCTION_H
#define MDT_ABSTRACT_CONSOLE_APPLICATION_MAIN_FUNCTION_H

#include <QObject>
#include <QStringList>

namespace Mdt{

  /*! \brief Abstract base of a main function in Qt console application with a event loop
   *
   * The class declaration MyMainFunction.h could look like:
   * \code
   * class MyMainFunction : public Mdt::AbstractConsoleApplicationMainFunction
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   explicit MyMainFunction(QObject* parent = nullptr);
   *   int runMain() override;
   * };
   * \endcode
   *
   * Example of the implementation in MyMainFunction.cpp:
   * \code
   * int MyMainFunction::runMain()
   * {
   *   qDebug() << "My main ...";
   * }
   * \endcode
   *
   * Finaly, in main.cpp:
   * \code
   * QCoreApplication app(argc, argv);
   *
   * // This will call runMain() once the event loop started
   * MyMainFunction mainImpl;
   *
   * return app.exec();
   * \endcode
   */
  class AbstractConsoleApplicationMainFunction : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractConsoleApplicationMainFunction(QObject* parent = nullptr);

    /*! \brief Returns the list of command-line arguments
     *
     * Returns QCoreApplication::arguments().
     *  As stated in QCoreApplication documentation,
     *  this method is slow, so the result should be stored
     *  in a variable if accessed many times
     *  (this is wy it is prefixed with get here)
     *
     * \pre A instance of a QCoreApplication (or one of its derivate)
     *       must exist.
     */
    static QStringList getArguments();

    /*! \brief main function implementation
     */
    virtual int runMain() = 0;

   public slots:

    /*! \brief Cleanup code
     *
     * If some cleanup has to be done before the application exists,
     *  this function can be implemented.
     *  See QCoreApplication documentation to know why
     *  this is a recommanded way to do cleanup.
     *
     * This default implementation does nothing.
     */
    virtual void aboutToQuit();

   private slots:

    void run();
  };

} // namespace Mdt{

#endif // #ifndef MDT_ABSTRACT_CONSOLE_APPLICATION_MAIN_FUNCTION_H
