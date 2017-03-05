/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "WidgetStyleSheetTest.h"
#include "Mdt/ItemEditor/WidgetStyleSheet.h"
#include <QLineEdit>
#include <QFont>
#include <QBrush>
#include <QColor>

using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void WidgetStyleSheetTest::initTestCase()
{
}

void WidgetStyleSheetTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void WidgetStyleSheetTest::setGetClearTest()
{
  /*
   * Initial state
   */
  WidgetStyleSheet ws;
  QVERIFY(ws.textFont().isNull());
  QVERIFY(ws.foregroundBrush().isNull());
  QVERIFY(ws.backgroundBrush().isNull());
  QVERIFY(ws.isNull());
  /*
   * Set text font
   */
  ws.setTextFont( QFont("Helvetica", 12) );
  QCOMPARE(ws.textFont().type(), QVariant::Font);
  QVERIFY(!ws.isNull());
  /*
   * Set foreground
   */
  ws.setForegroundBrush( QBrush(QColor(255,100,50)) );
  QCOMPARE(ws.foregroundBrush().type(), QVariant::Brush);
  /*
   * Set background
   */
  ws.setBackgroundBrush( QBrush(QColor(100,50,25)) );
  QCOMPARE(ws.backgroundBrush().type(), QVariant::Brush);
  /*
   * Clear
   */
  ws.clear();
  QVERIFY(ws.textFont().isNull());
  QVERIFY(ws.foregroundBrush().isNull());
  QVERIFY(ws.backgroundBrush().isNull());
  QVERIFY(ws.isNull());
}

void WidgetStyleSheetTest::textFontCssStringTest()
{
  WidgetStyleSheet ws;
  QFont font;
  QString expectedCss;

  font.setFamily("Helvetica");
  font.setPointSize(14);
  font.setItalic(false);
  font.setBold(false);
  ws.setTextFont(font);
  expectedCss = "*{font-family:\"Helvetica\";font-size:14pt;font-style:normal;font-weight:normal}";
  QCOMPARE(ws.toCssString(), expectedCss);

  font.setFamily("Helvetica");
  font.setPixelSize(14);
  font.setItalic(true);
  font.setBold(true);
  ws.setTextFont(font);
  expectedCss = "*{font-family:\"Helvetica\";font-size:14px;font-style:italic;font-weight:bold}";
  QCOMPARE(ws.toCssString(), expectedCss);
}

void WidgetStyleSheetTest::foregroundBrushCssStringTest()
{
  WidgetStyleSheet ws;
  QBrush brush;
  QString expectedCss;

  brush.setColor( QColor(255,0,0) );
  ws.setForegroundBrush(brush);
  expectedCss = "*{color:#ff0000}";
  QCOMPARE(ws.toCssString(), expectedCss);
}

void WidgetStyleSheetTest::backGroundBrushCssStringTest()
{
  WidgetStyleSheet ws;
  QBrush brush;
  QString expectedCss;

  brush.setColor( QColor(255,0,0) );
  ws.setBackgroundBrush(brush);
  expectedCss = "*{background-color:#ff0000}";
  QCOMPARE(ws.toCssString(), expectedCss);

}

void WidgetStyleSheetTest::cssStringTest()
{
  QBrush brush;
  QString expectedCss;

  /*
   * Null style sheet
   */
  WidgetStyleSheet ws;
  expectedCss = "";
  QCOMPARE(ws.toCssString(), expectedCss);
  /*
   * Text font + foreground color
   */
  ws.clear();
  ws.setTextFont(QFont("Helvetica", 14));
  ws.setForegroundBrush(QBrush(QColor(0,255,0)));
  expectedCss = "*{font-family:\"Helvetica\";font-size:14pt;font-style:normal;font-weight:normal;color:#00ff00}";
  QCOMPARE(ws.toCssString(), expectedCss);
  /*
   * Text font + background color
   */
  ws.clear();
  ws.setTextFont(QFont("Helvetica", 14));
  ws.setBackgroundBrush(QBrush(QColor(255,0,0)));
  expectedCss = "*{font-family:\"Helvetica\";font-size:14pt;font-style:normal;font-weight:normal;background-color:#ff0000}";
  QCOMPARE(ws.toCssString(), expectedCss);
  /*
   * Foreground + background
   */
  ws.clear();
  ws.setForegroundBrush(QBrush(QColor(0,255,0)));
  ws.setBackgroundBrush(QBrush(QColor(255,0,0)));
  expectedCss = "*{color:#00ff00;background-color:#ff0000}";
  QCOMPARE(ws.toCssString(), expectedCss);
  /*
   * Text font + foreground color + background
   */
  ws.clear();
  ws.setTextFont(QFont("Helvetica", 14));
  ws.setForegroundBrush(QBrush(QColor(0,255,0)));
  ws.setBackgroundBrush(QBrush(QColor(255,0,0)));
  expectedCss = "*{font-family:\"Helvetica\";font-size:14pt;font-style:normal;font-weight:normal;color:#00ff00;background-color:#ff0000}";
  QCOMPARE(ws.toCssString(), expectedCss);
}

void WidgetStyleSheetTest::cssStringBenchmark()
{
  WidgetStyleSheet ws;
  ws.setTextFont(QFont("Helvetica", 14));
  ws.setForegroundBrush(QBrush(QColor(0,255,0)));
  ws.setBackgroundBrush(QBrush(QColor(255,0,0)));
  QString css;
  QString expectedCss = "*{font-family:\"Helvetica\";font-size:14pt;font-style:normal;font-weight:normal;color:#00ff00;background-color:#ff0000}";
  QBENCHMARK{
    css = ws.toCssString();
  }
  QCOMPARE(css, expectedCss);
}

void WidgetStyleSheetTest::comparisonTest()
{
  WidgetStyleSheet a, b;
  /*
   * Both nul
   */
  QVERIFY(a == b);
  /*
   * a: text font
   * b: null
   */
  a.setTextFont(QFont("Helvetica", 12));
  QVERIFY(!(a == b));
  /*
   * a: text font
   * b: same text font
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  QVERIFY(a == b);
  /*
   * a: text font
   * b: other text font
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 14));
  QVERIFY(!(a == b));
  /*
   * a: foreground
   * b: same foreground
   */
  a.clear();
  b.clear();
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(10,20,30)));
  QVERIFY(a == b);
  /*
   * a: foreground
   * b: other foreground
   */
  a.clear();
  b.clear();
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(0,20,30)));
  QVERIFY(!(a == b));
  /*
   * a: background
   * b: same background
   */
  a.clear();
  b.clear();
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(10,20,30)));
  QVERIFY(a == b);
  /*
   * a: background
   * b: other background
   */
  a.clear();
  b.clear();
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(0,20,30)));
  QVERIFY(!(a == b));
  /*
   * a: text font + foreground
   * b: same text font + same foreground
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(10,20,30)));
  QVERIFY(a == b);
  /*
   * a: text font + foreground
   * b: same text font + other foreground
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(0,20,30)));
  QVERIFY(!(a == b));
  /*
   * a: text font + background
   * b: same text font + same background
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(10,20,30)));
  QVERIFY(a == b);
  /*
   * a: text font + background
   * b: same text font + other background
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(0,20,30)));
  QVERIFY(!(a == b));
  /*
   * a: text font + foreground + background
   * b: same text font + same foreground + same background
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(10,20,30)));
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(10,20,30)));
  QVERIFY(a == b);
  /*
   * a: text font + foreground + background
   * b: same text font + same foreground + other background
   */
  a.clear();
  b.clear();
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(10,20,30)));
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(0,20,30)));
  QVERIFY(!(a == b));
}

void WidgetStyleSheetTest::comparisonBenchmark()
{
  QLineEdit editor;
  WidgetStyleSheet a, b;
  /*
   * a: text font + foreground + background
   * b: same text font + same foreground + same background
   */
  a.setTextFont(QFont("Helvetica", 12));
  b.setTextFont(QFont("Helvetica", 12));
  a.setForegroundBrush(QBrush(QColor(10,20,30)));
  b.setForegroundBrush(QBrush(QColor(10,20,30)));
  a.setBackgroundBrush(QBrush(QColor(10,20,30)));
  b.setBackgroundBrush(QBrush(QColor(10,20,30)));
  QBENCHMARK{
    QVERIFY(a == b);
  }
  if(a == b){
    editor.setStyleSheet(a.toCssString());
  }
  QVERIFY(!editor.styleSheet().isEmpty());
}

void WidgetStyleSheetTest::setWidgetStyleSheetStaticBenchmark()
{
  QLineEdit editor;
  editor.show();
  QBENCHMARK{
    editor.setStyleSheet("*{background-color:#ff0000}");
  }
  QCOMPARE(editor.styleSheet(), QString("*{background-color:#ff0000}"));
}

void WidgetStyleSheetTest::setWidgetStyleSheetBenchmark()
{
  WidgetStyleSheet ws;
  ws.setBackgroundBrush(QBrush(QColor(255,0,0)));
  QLineEdit editor;
  editor.show();
  QBENCHMARK{
    editor.setStyleSheet(ws.toCssString());
  }
  QCOMPARE(editor.styleSheet(), QString("*{background-color:#ff0000}"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WidgetStyleSheetTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
