/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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

/*! \page guideline_page API guide line
 *
 * \section getters Propoerty access
 *
 * When a member function returns a property that is directly known,
 *  the function name should be directly similar to the propoerty name.
 *  Example: SomeObject::isNull() .
 *
 * \todo Should be: is If a member function needs some processing before returning the requested value, it should be named explicitly if possible.
 *    For example, on conversion, toSomeString() .
 *    If no such name can be found, prefix get..
 *
 *  If a member function needs some processing before returning the requested value,
 *  it should be prefixed get.
 *  For example, we suppose that getting available tables in a database needs to query the database,
 *  the function should be named like SomeObject::getAvailableTables(T db).
 *
 * \section error_handlig Error handling
 *
 * When a function fails, it should be possible to get the reason why it failed.
 *  This is done by using the Mdt::Error class.
 *
 * \subsection error_handlig_return_value_context Return value: context
 *
 * Take the example of a repository class.
 *  Some method will return a value, but other not.
 *
 * The first example will use Mdt::Expected when a value is returned,
 *  and Mdt::ExpectedResult if no value is returned:
 * \code
 * ProductRepository repository;
 *
 * const auto product = repository.get(id);
 * if(!product){
 *   // Use product.error()
 *   return ...
 * }
 * doSomeStuff(*product);
 * if(product->isOutdated()){
 *   const auto result = repository.remove(*product);
 *   if(!result){
 *     // Use result.error()
 *     return ...
 *   }
 * }else{
 *   const auto result = repository.update(*product);
 *   if(!result){
 *     // Use result.error()
 *     return ...
 *   }
 * }
 * \endcode
 *
 * Above code shows that a new variable must be created each time a method (that can fail) is called.
 *
 * The next example will use Mdt::Expected when a value is returned,
 *  and bool + lastError if no value is returned:
 * \code
 * ProductRepository repository;
 *
 * const auto product = repository.get(id);
 * if(!product){
 *   // Use product.error()
 *   return ...
 * }
 * doSomeStuff(*product);
 * if(product->isOutdated()){
 *   if( !repository.remove(*product) ){
 *     // Use repository.lastError()
 *     return ...
 *   }
 * }else{
 *   if( !repository.update(*product) ){
 *     // Use repository.lastError()
 *     return ...
 *   }
 * }
 * \endcode
 *
 *
 * \todo See also about ExpectedOptional
 *
 * \subsection error_handlig_return_value_rules Return value: rules
 *
 * A class that has some methods that can fail should follow these rules:
 *  - It should provide a lastError() method
 *  - It should be implemented using a private setLastError() method.
 *    This way, it will be easier to change the class if, for example,
 *    it should send a notification when a error occured.
 *    It should also be checked that a null error is never set.
 *  - A method that can fail, but returns no value, should return bool
 *    and update the last error using setLastError()
 *  - A method that can fail, and returns a value, should return a Mdt::Expected
 *    and update the last error using setLastError()
 *
 * Example of such class:
 * \code
 * class ProductRepository
 * {
 *  public:
 *
 *   Mdt::Expected<ProductId> add(const Product & product);
 *   Mdt::Expected<Product> get(ProductId id) const;
 *   bool update(const Product & product);
 *   bool remove(ProductId id);
 *   bool removeAll();
 *
 *   Mdt::Error lastError() const
 *   {
 *     return mLastError;
 *   }
 *
 *  private:
 *
 *   void setLastError(const Mdt::Error & error) const
 *   {
 *     Q_ASSERT(!error.isNull());
 *     mLastError = error;
 *   }
 *
 *   mutable Mdt::Error mLastError;
 * };
 * \endcode
 *
 * Notice that mLastError is declared mutable.
 *  This lets the %get() method be const.
 *  The const tells that %get() will not mutate the repository,
 *  but it does not offer any thread safety at all.
 *
 * A plain function, or a static member method, that can fail,
 *  should return Mdt::Expected if it returns some data,
 *  otherwise Mdt::ExpectedResult.
 *
 * Value classes should not have methods that can fail,
 *  because they do not, for example, read files.
 *
 * \subsection error_handlig_dialog Dialogs
 *
 *  A dialog is a top level widget that is displayed to the user.
 *   When a error occurs, the dialog should also display it to the user.
 *
 *  If a public function can fail (for example setting a file),
 *   it should display the error to the user and return false,
 *   so the caller knows that something failed and he adapt his flow.
 *   \note If something could fail, the dialog should inform the user, and should not be acceptable.
 *
 * \section ui_files Ui files when using QtDesigner
 *
 *  One of the possible way to use headers generated by uic, is to inherit from the base class and from the class generated by uic.
 *   This option requires that ui_*.h is included in the header file of the class.
 *   Because this header file is generated during compilation, it will be somwhere in the build tree.
 *   For this library, this issue has to be solved, and is by setting correct CMake options.
 *   But, the problem will then happen again to the user, which has no access to this ui_*h file.
 *
 *  To solve this issue, the Ui class should be forward declared in the header,
 *   used as member of the class, using a unique_ptr.
 *   The ui_*h file can also be included in the *.cpp file.
 *
 *  Example of a widget class declaration:
 *   \code
 *   #include <QWidget>
 *   #include <memory>
 *
 *   namespace Ui{
 *     class MyWidget;
 *   }
 *
 *   class MyWidget : public QWidget
 *   {
 *    Q_OBJECT
 *
 *    public:
 *
 *     MyWidget(QWidget *parent = nullptr);
 *
 *    // unique_ptr needs complete definition to destruct Ui::MyWidget
 *    ~MyWidget();
 *
 *    private:
 *
 *     std::unique_ptr<Ui::MyWidget> mUi;
 *   };
 *   \endcode
 *
 *  Example of a widget class implementation:
 *   \code
 *   #include "MyWidget.h"
 *   #include "ui_MyWidget.h"
 *
 *   MyWidget::MyWidget(QWidget *parent)
 *    : QWidget(parent),
 *      mUi( std::make_unique<Ui::MyWidget>() )
 *   {
 *     mUi->setupUi(this);
 *   }
 *
 *   MyWidget::~MyWidget()
 *   {
 *   }
 *   \endcode
 *
 *   See Mdt::ItemEditor::StandardWindow to see a example of this usage.
 *
 * \section translations Translations
 *
 * \subsection translations_tr About using tr()
 *
 *  When a class is a subclass of QObject, simply using tr() works fine.
 *
 *  For a class that is not a subclass of QObject, Qt recommends using QCoreApplication::translate()
 *   and provide the class name as context, so that Qt Linguist can display it the proper way to the translator.
 *   Previously, many classes in Mdt libraries have added a tr() static member function, that simply called QObject::tr().
 *   This is bad, because Qt Linguist also displayed all strings as QObject context.
 *   The recommanded solution is to use Q_DECLARE_TR_FUNCTIONS() provided in QCoreApplication
 *   For example, in MyClass.h:
 *   \code
 *    #include <QCoreApplication>
 *
 *    class MyClass
 *    {
 *      Q_DECLARE_TR_FUNCTIONS(MyClass)
 *
 *     public:
 *
 *      MyClass();
 *    };
 *   \endcode
 *
 *  Note that Qt Linguist trows error messages when a class uses tr() and it does not provide the Q_OBJECT macro,
 *   despite the fact that translation file are generated and works.
 *
 * \section library_exports Library exports
 *
 * For some platforms, classes and functions that are used outside a shared library must be exported explicitly in the header.
 *
 * Mdt uses CMake to generate the MDT_LIBNAME_EXPORT macro.
 *  This macro is defined in MdtLibnameExport.h, which is not available in the source tree, but generated during compilation.
 *  When using mdt_add_library() to create a Mdt library, the path to the directory where Export.h is located will be added.
 *
 * Example of a class that must be accessible outside the ItemModel shared library:
 * \code
 * #include "MdtItemModelExport.h"
 *
 * class MDT_ITEMMODEL_EXPORT MyClass
 * {
 * };
 * \endcode
 */
