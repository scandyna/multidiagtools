
#include <QGuiApplication>
#include <QQmlApplicationEngine>
// #include <QQuickView>
#include <QUrl>
#include <QLatin1String>

int main(int argc, char **argv)
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine qmlEngine( QUrl(QLatin1String("qrc:/main.qml")) );
//   QQuickView view;
// 
//   view.setSource( QUrl(QLatin1String("qrc:/main.qml")) );
//   view.show();

  return app.exec();
}
