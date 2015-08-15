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
#include "mdtMathTest.h"
#include "mdtMath.h"
#include "mdtApplication.h"
#include <QVector>
#include <vector>

/// \todo only for sandbox
#include <QWidget>
#include <QDialog>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <QPointF>

/*! \todo Remove when fixed
 * Bug in Ubuntu 15.04 :
 *  - Qt 5.4 has removed QT_STATIC_CONST from qblobal.h
 *  - Qwt fixed this in version 6.1.2
 *  - Ubuntu ships Qt 5.4 and Qwt 6.1.1
 */
#define QT_STATIC_CONST static const
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <random>

#include <QDebug>

Q_DECLARE_METATYPE(std::vector<double>)

void mdtMathTest::sandbox()
{
  QDialog dialog;
  QHBoxLayout *hl;

  QVBoxLayout *layout = new QVBoxLayout;
  
  QwtPlot *plot = new QwtPlot;
  layout->addWidget(plot);
  
  hl = new QHBoxLayout;
  QLabel *lbAmeanLabel = new QLabel("Arithmetic mean:");
  QLabel *lbAmean = new QLabel;
  hl->addWidget(lbAmeanLabel);
  hl->addWidget(lbAmean);
  layout->addLayout(hl);
  
  hl = new QHBoxLayout;
  QLabel *lbGmeanLabel = new QLabel("Geometric mean:");
  QLabel *lbGmean = new QLabel;
  hl->addWidget(lbGmeanLabel);
  hl->addWidget(lbGmean);
  layout->addLayout(hl);
  
  QLabel *lastLabel = new QLabel;
  layout->addWidget(lastLabel);

  /*
   * Generate data
   */
  std::vector<double> data;
  int N = 1000;
//   randomValueInit();
//   for(int i = 0; i < N; ++i){
//     data.push_back(randomValue(-10, 10));
//   }
   std::random_device rd;
//   ///std::default_random_engine e1(rd());
//   ///std::uniform_real_distribution<double> dist1(0.0, 1.0);
   std::mt19937 e1(rd());
   std::normal_distribution<double> dist1(70, 2);
   for(int i = 0; i < N; ++i){
     data.push_back( dist1(e1) );
   }
  
//   for(int i = 0; i < N; ++i){
//     data.push_back( i );
//   }

  /*
   * Create curve
   */
  QVector<QPointF> curveData;
  for(int i = 0; i < (int)data.size(); ++i){
    QPointF p(i, data[i]);
    curveData.append(p);
  }
  QwtPlotCurve *curve = new QwtPlotCurve;
  curve->setSamples(curveData);
  curve->attach(plot);
  /*
   * Calculate mean, variance, deviation
   */
  double aMean = mdt::arithmeticMean<double>(data);
  lbAmean->setText(QString::number(aMean));
  lbGmean->setText(QString::number(mdt::geometricMean(data, 100)));
  QString str = "Variance: " + QString::number(mdt::variance(data, aMean));
  str += "  , ecart type: " + QString::number(mdt::uncorrectedStandardDeviation(data, aMean));
  str += "  , ecart type corrige: " + QString::number(mdt::correctedStandardDeviation(data, aMean));
  lastLabel->setText(str);
  
  
  dialog.setLayout(layout);
  dialog.resize(500, 300);
  dialog.exec();
}


void mdtMathTest::advanceIteratorTest()
{
  std::vector<double> v1 {1.0};
  std::vector<double> v2 {1.0, 2.0};
  std::vector<double> v3 {1.0, 2.0, 3.0};
  QVector<double> qv1 = QVector<double>::fromStdVector(v1);
  QVector<double> qv2 = QVector<double>::fromStdVector(v2);
  QVector<double> qv3 = QVector<double>::fromStdVector(v3);

  auto it = v1.cbegin();
  auto qit = qv1.begin();
  /*
   * Advance of 0
   */
  // 1 size vectors
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 0), 0);
  QVERIFY(it == v1.cbegin());
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 0), 0);
  QVERIFY(qit == qv1.begin());
  // 2 size vectors
  it = v2.cbegin();
  qit = qv2.begin();
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 0), 0);
  QVERIFY(it == v2.cbegin());
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 0), 0);
  QVERIFY(qit == qv2.begin());
  // 3 size vectors
  it = v3.cbegin();
  qit = qv3.begin();
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 0), 0);
  QVERIFY(it == v3.cbegin());
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 0), 0);
  QVERIFY(qit == qv3.begin());
  /*
   * Advance of 1 - 1 element vectors
   */
  // std vector
  it = v1.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 1), 1);
  QVERIFY(it == v1.cend());
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 1), 0);
  QVERIFY(it == v1.cend());
  // qt vector
  qit = qv1.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 1), 1);
  QVERIFY(qit == qv1.end());
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 1), 0);
  QVERIFY(qit == qv1.end());
  /*
   * Advance of 1 - 2 element vectors
   */
  // std vector
  it = v2.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 1), 1);
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 1), 1);
  QVERIFY(it == v2.cend());
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 1), 0);
  QVERIFY(it == v2.cend());
  // qt vector
  qit = qv2.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 1), 1);
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 1), 1);
  QVERIFY(qit == qv2.end());
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 1), 0);
  QVERIFY(qit == qv2.end());
  /*
   * Advance of 1 - 3 element vectors
   */
  // std vector
  it = v3.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 1), 1);
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 1), 1);
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 1), 1);
  QVERIFY(it == v3.cend());
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 1), 0);
  QVERIFY(it == v3.cend());
  // qt vector
  qit = qv3.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 1), 1);
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 1), 1);
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 1), 1);
  QVERIFY(qit == qv3.end());
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 1), 0);
  QVERIFY(qit == qv3.end());
  /*
   * Advance of 2 - 1 element vectors
   */
  // std vector
  it = v1.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 2), 1);
  QVERIFY(it == v1.cend());
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 2), 0);
  QVERIFY(it == v1.cend());
  // qt vector
  qit = qv1.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 2), 1);
  QVERIFY(qit == qv1.end());
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 2), 0);
  QVERIFY(qit == qv1.end());
  /*
   * Advance of 2 - 2 element vectors
   */
  // std vector
  it = v2.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 2), 2);
  QVERIFY(it == v2.cend());
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 2), 0);
  QVERIFY(it == v2.cend());
  // qt vector
  qit = qv2.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 2), 2);
  QVERIFY(qit == qv2.end());
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 2), 0);
  QVERIFY(qit == qv2.end());
  /*
   * Advance of 2 - 3 element vectors
   */
  // std vector
  it = v3.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 2), 2);
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 2), 1);
  QVERIFY(it == v3.cend());
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 2), 0);
  QVERIFY(it == v3.cend());
  // qt vector
  qit = qv3.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 2), 2);
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 2), 1);
  QVERIFY(qit == qv3.end());
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 2), 0);
  QVERIFY(qit == qv3.end());
  /*
   * Advance of 3 - 1 element vectors
   */
  // std vector
  it = v1.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 3), 1);
  QVERIFY(it == v1.cend());
  QCOMPARE(mdt::advanceIterator(it, v1.cend(), 3), 0);
  QVERIFY(it == v1.cend());
  // qt vector
  qit = qv1.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 3), 1);
  QVERIFY(qit == qv1.end());
  QCOMPARE(mdt::advanceIterator(qit, qv1.end(), 3), 0);
  QVERIFY(qit == qv1.end());
  /*
   * Advance of 3 - 2 element vectors
   */
  // std vector
  it = v2.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 3), 2);
  QVERIFY(it == v2.cend());
  QCOMPARE(mdt::advanceIterator(it, v2.cend(), 3), 0);
  QVERIFY(it == v2.cend());
  // qt vector
  qit = qv2.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 3), 2);
  QVERIFY(qit == qv2.end());
  QCOMPARE(mdt::advanceIterator(qit, qv2.end(), 3), 0);
  QVERIFY(qit == qv2.end());
  /*
   * Advance of 3 - 3 element vectors
   */
  // std vector
  it = v3.cbegin();
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 3), 3);
  QVERIFY(it == v3.cend());
  QCOMPARE(mdt::advanceIterator(it, v3.cend(), 3), 0);
  QVERIFY(it == v3.cend());
  // qt vector
  qit = qv3.begin();
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 3), 3);
  QVERIFY(qit == qv3.end());
  QCOMPARE(mdt::advanceIterator(qit, qv3.end(), 3), 0);
  QVERIFY(qit == qv3.end());
}

void mdtMathTest::productTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedProduct);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::product(v.cbegin(), v.cend()), expectedProduct);
  QCOMPARE(mdt::product(qv.begin(), qv.end()), expectedProduct);
}

void mdtMathTest::productTest_data()
{
  std::vector<double> tmp;
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedProduct");

  for(int i = 1; i <= 100; ++i){
    std::vector<double> tmp;
    tmp.assign(i, 1.0);
    QTest::newRow("All 1.0") << tmp << 1.0;
    tmp.assign(i, 2.0);
    QTest::newRow("All 2.0") << tmp << std::pow(2.0, i);
  }
}

void mdtMathTest::productBenchamrk()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedProduct);

  double x;

  QBENCHMARK{
    x = mdt::product(v.cbegin(), v.cend());
  }
  QCOMPARE(x, expectedProduct);
}

void mdtMathTest::productBenchamrk_data()
{
  std::vector<double> tmp;
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedProduct");

  // 3 elements input
  tmp.assign(3, 2.0);
  QTest::newRow("3 elements") << tmp << std::pow(2.0, 3);
  // 100 elements input
  tmp.assign(100, 2.0);
  QTest::newRow("100 elements") << tmp << std::pow(2.0, 100);
}

void mdtMathTest::arithmeticMeanTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedMean);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::arithmeticMean<double>(v), expectedMean);
  QCOMPARE(mdt::arithmeticMean<double>(qv), expectedMean);
}

void mdtMathTest::arithmeticMeanTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedMean");

  // Some simple cases
  for(int i = 1; i <= 100; ++i){
    std::vector<double> tmp;
    tmp.assign(i, 1.0);
    QTest::newRow("All 1.0") << tmp << 1.0;
    tmp.assign(i, 2.0);
    QTest::newRow("All 2.0") << tmp << 2.0;
    tmp.assign(i, 3.0);
    QTest::newRow("All 3.0") << tmp << 3.0;
    tmp.assign(i, 4.0);
    QTest::newRow("All 4.0") << tmp << 4.0;
    tmp.assign(i, 5.0);
    QTest::newRow("All 5.0") << tmp << 5.0;
    tmp.assign(i, 3.876);
    QTest::newRow("All 3.876") << tmp << 3.876;
  }
  // Some vectors with variable data
  std::vector<double> tmp1 {1.0, 2.5, 1.6, 5.8, 3.2, 7.8};
  QTest::newRow("5 variable values") << tmp1 << 3.65;
  std::vector<double> tmp2 {1.0, 2.0, 3.0};
  QTest::newRow("3 variable values") << tmp2 << 2.0;
}

void mdtMathTest::arithmeticMeanBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedMean);

  double x;

  QBENCHMARK{
    x = mdt::arithmeticMean<double>(v);
  }
  QCOMPARE(x, expectedMean);
}

void mdtMathTest::arithmeticMeanBenchmark_data()
{
  std::vector<double> tmp;
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedMean");

  // 3 elements input
  tmp.assign(3, 2.5);
  QTest::newRow("3 elements") << tmp << 2.5;
  // 100 elements input
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 2.5;
  // 10000 elements input
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 2.5;
}

void mdtMathTest::geometricMeanTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedMean);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::geometricMean(v, v.size()), expectedMean);
  QCOMPARE(mdt::geometricMean(qv, v.size()), expectedMean);
  if(v.size() > 2){
    QCOMPARE(mdt::geometricMean(v, 2), expectedMean);
    QCOMPARE(mdt::geometricMean(qv, 2), expectedMean);
  }
}

void mdtMathTest::geometricMeanTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedMean");

  // Some simple cases
  for(int i = 1; i <= 100; ++i){
    std::vector<double> tmp;
    tmp.assign(i, 1.0);
    QTest::newRow("All 1.0") << tmp << 1.0;
    tmp.assign(i, 2.0);
    QTest::newRow("All 2.0") << tmp << 2.0;
    tmp.assign(i, 3.0);
    QTest::newRow("All 3.0") << tmp << 3.0;
    tmp.assign(i, 4.0);
    QTest::newRow("All 4.0") << tmp << 4.0;
    tmp.assign(i, 5.0);
    QTest::newRow("All 5.0") << tmp << 5.0;
    tmp.assign(i, 3.876);
    QTest::newRow("All 3.876") << tmp << 3.876;
  }
  // Some vectors with variable data
  std::vector<double> tmp1 {2.0, 8.0, 256.0};
  QTest::newRow("3 variable values") << tmp1 << 16.0;
}

void mdtMathTest::geometricMeanBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, expectedMean);

  double x;
  int n = std::min((int)v.size(), 100);

  QBENCHMARK{
    x = mdt::geometricMean(v, n);
  }
  QCOMPARE(x, expectedMean);
}

void mdtMathTest::geometricMeanBenchmark_data()
{
  std::vector<double> tmp;
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("expectedMean");

  // 3 elements input
  tmp.assign(3, 2.5);
  QTest::newRow("3 elements") << tmp << 2.5;
  // 100 elements input
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 2.5;
  // 10000 elements input
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 2.5;
}

void mdtMathTest::sumOfSquaredDifferencesTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, y);
  QFETCH(double, expectedSum);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::sumOfSquaredDifferences(v, y), expectedSum);
  QCOMPARE(mdt::sumOfSquaredDifferences(qv, y), expectedSum);
}

void mdtMathTest::sumOfSquaredDifferencesTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("y");
  QTest::addColumn<double>("expectedSum");

  std::vector<double> tmp1 {1.0};
  QTest::newRow("1 variable values") << tmp1 << 0.5 << 0.25;
  std::vector<double> tmp2 {1.0, 2.0};
  QTest::newRow("2 variable values") << tmp2 << 0.5 << 2.5;
  std::vector<double> tmp3 {1.0, 2.0, 3.0};
  QTest::newRow("3 variable values") << tmp3 << 0.5 << 8.75;
  std::vector<double> tmp4 {1.0, 2.0, 3.0, 4.0};
  QTest::newRow("4 variable values") << tmp4 << 0.5 << 21.0;
  // Make some grater data sets
  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 400.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 4000.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 40000.0;
}

void mdtMathTest::sumOfSquaredDifferencesBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, y);
  QFETCH(double, expectedSum);

  double sum;

  QBENCHMARK{
    sum = mdt::sumOfSquaredDifferences(v, y);
  }
  QCOMPARE(sum, expectedSum);
}

void mdtMathTest::sumOfSquaredDifferencesBenchmark_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("y");
  QTest::addColumn<double>("expectedSum");

  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 400.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 4000.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 40000.0;
}

void mdtMathTest::varianceTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedVariance);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::variance(v, mean), expectedVariance);
  QCOMPARE(mdt::variance(qv, mean), expectedVariance);
}

void mdtMathTest::varianceTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedVariance");

  std::vector<double> tmp1 {1.0};
  QTest::newRow("1 variable values") << tmp1 << 0.5 << 0.25/1.0;
  std::vector<double> tmp2 {1.0, 2.0};
  QTest::newRow("2 variable values") << tmp2 << 0.5 << 2.5/2.0;
  std::vector<double> tmp3 {1.0, 2.0, 3.0};
  QTest::newRow("3 variable values") << tmp3 << 0.5 << 8.75/3.0;
  std::vector<double> tmp4 {1.0, 2.0, 3.0, 4.0};
  QTest::newRow("4 variable values") << tmp4 << 0.5 << 21.0/4.0;
  // Make some grater data sets
  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 400.0/100.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 4000.0/1000.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 40000.0/10000.0;
}

void mdtMathTest::varianceBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedVariance);

  double sum;

  QBENCHMARK{
    sum = mdt::variance(v, mean);
  }
  QCOMPARE(sum, expectedVariance);
}

void mdtMathTest::varianceBenchmark_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedVariance");

  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 4.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 4.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 4.0;
}

void mdtMathTest::uncorrectedStandardDeviationTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedDeviation);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::uncorrectedStandardDeviation(v, mean), expectedDeviation);
  QCOMPARE(mdt::uncorrectedStandardDeviation(qv, mean), expectedDeviation);
}

void mdtMathTest::uncorrectedStandardDeviationTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedDeviation");

  std::vector<double> tmp1 {1.0};
  QTest::newRow("1 variable values") << tmp1 << 0.5 << 0.5;
  std::vector<double> tmp2 {1.0, 2.0};
  QTest::newRow("2 variable values") << tmp2 << 0.5 << std::sqrt(2.5/2.0);
  std::vector<double> tmp3 {1.0, 2.0, 3.0};
  QTest::newRow("3 variable values") << tmp3 << 0.5 << std::sqrt(8.75/3.0);
  std::vector<double> tmp4 {1.0, 2.0, 3.0, 4.0};
  QTest::newRow("4 variable values") << tmp4 << 0.5 << std::sqrt(21.0/4.0);
  // Make some grater data sets
  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 2.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 2.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 2.0;
}

void mdtMathTest::uncorrectedStandardDeviationBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedDeviation);

  double sum;

  QBENCHMARK{
    sum = mdt::uncorrectedStandardDeviation(v, mean);
  }
  QCOMPARE(sum, expectedDeviation);
}

void mdtMathTest::uncorrectedStandardDeviationBenchmark_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedDeviation");

  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << 2.0;
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << 2.0;
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << 2.0;
}

void mdtMathTest::correctedStandardDeviationTest()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedDeviation);

  QVector<double> qv = QVector<double>::fromStdVector(v);

  QCOMPARE(mdt::correctedStandardDeviation(v, mean), expectedDeviation);
  QCOMPARE(mdt::correctedStandardDeviation(qv, mean), expectedDeviation);
}

void mdtMathTest::correctedStandardDeviationTest_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedDeviation");

  std::vector<double> tmp2 {1.0, 2.0};
  QTest::newRow("2 variable values") << tmp2 << 0.5 << std::sqrt(2.5);
  std::vector<double> tmp3 {1.0, 2.0, 3.0};
  QTest::newRow("3 variable values") << tmp3 << 0.5 << std::sqrt(8.75/2.0);
  std::vector<double> tmp4 {1.0, 2.0, 3.0, 4.0};
  QTest::newRow("4 variable values") << tmp4 << 0.5 << std::sqrt(21.0/3.0);
  // Make some grater data sets
  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << std::sqrt(400.0/99.0);
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << std::sqrt(4000.0/999.0);
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << std::sqrt(40000.0/9999.0);
}

void mdtMathTest::correctedStandardDeviationBenchmark()
{
  QFETCH(std::vector<double>, v);
  QFETCH(double, mean);
  QFETCH(double, expectedDeviation);

  double sum;

  QBENCHMARK{
    sum = mdt::correctedStandardDeviation(v, mean);
  }
  QCOMPARE(sum, expectedDeviation);
}

void mdtMathTest::correctedStandardDeviationBenchmark_data()
{
  QTest::addColumn<std::vector<double>>("v");
  QTest::addColumn<double>("mean");
  QTest::addColumn<double>("expectedDeviation");

  std::vector<double> tmp;
  tmp.assign(100, 2.5);
  QTest::newRow("100 elements") << tmp << 0.5 << std::sqrt(400.0/99.0);
  tmp.assign(1000, 2.5);
  QTest::newRow("1000 elements") << tmp << 0.5 << std::sqrt(4000.0/999.0);
  tmp.assign(10000, 2.5);
  QTest::newRow("10'000 elements") << tmp << 0.5 << std::sqrt(40000.0/9999.0);
}



/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtMathTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
