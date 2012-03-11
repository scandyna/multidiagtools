
#include "mdtFrameCodecTest.h"
#include "mdtFrameCodec.h"
#include "mdtFrameCodecAscii.h"
#include "mdtFrameCodecScpi.h"
#include "mdtFrameCodecScpiU3606A.h"
#include <QByteArray>

#include <QVariant>
#include <QList>

#include <QDebug>

void mdtFrameCodecTest::mdtFrameCodecScpiTest()
{
  mdtFrameCodecScpi f;
  QByteArray data;
  QByteArray eofSeq = "\n";
  QVariant value;

  // Test with empty frames (must fail and not crasch)
  QVERIFY(!f.decodeIdn(data));
  QVERIFY(!f.decodeSingleValueInt(data).isValid());
  data = "";
  QVERIFY(!f.decodeIdn(data));
  QVERIFY(!f.decodeSingleValueInt(data).isValid());
  // Set the EOF Sequence
  f.setEofSeq(eofSeq);
  QVERIFY(!f.decodeIdn(data));
  QVERIFY(!f.decodeSingleValueInt(data).isValid());

  // Decoding answer of *IDN? query
  data = "Agilent Technologies,U3606A,MY00000000,02.00-02.00-02.01\n";
  QVERIFY(f.decodeIdn(data));
  QVERIFY(f.values().size() == 4);
  QVERIFY(f.values().at(0) == "Agilent Technologies");
  QVERIFY(f.values().at(1) == "U3606A");
  QVERIFY(f.values().at(2) == "MY00000000");
  QVERIFY(f.values().at(3) == "02.00-02.00-02.01");

  // Decoding error
  data = " -133,\"Undefined header\"";
  QVERIFY(f.decodeError(data));
  QVERIFY(f.values().size() == 2);
  QVERIFY(f.values().at(0).isValid());
  QVERIFY(f.values().at(0) == -133);
  QVERIFY(f.values().at(1) == "\"Undefined header\"");

  // Decode single value: format int
  data = "1\n";
  value = f.decodeSingleValueInt(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.toInt(), 1);
  QVERIFY(f.values().size() == 1);
  QCOMPARE(f.values().at(0).toInt(), 1);

  // Decode single value: format int
  data = "-255\n";
  value = f.decodeSingleValueInt(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.toInt(), -255);
  QVERIFY(f.values().size() == 1);
  QCOMPARE(f.values().at(0).toInt(), -255);

  // Decode single value: format int
  data = "+0\n";
  value = f.decodeSingleValueInt(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.toInt(), 0);
  QVERIFY(f.values().size() == 1);
  QCOMPARE(f.values().at(0).toInt(), 0);

  // Decode single value: format int
  data = "+793\n";
  value = f.decodeSingleValueInt(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.toInt(), 793);
  QVERIFY(f.values().size() == 1);
  QCOMPARE(f.values().at(0).toInt(), 793);

  // Tests with floating values
/*
  data = "+3.197000E-06\n";
  QVERIFY(f.decode(data));
  QVERIFY(f.values().size() == 1);
  QVERIFY(f.values().at(0).isValid());
  QCOMPARE(f.values().at(0).toDouble(), 3.197000E-06);
*/
  
  // Definite-Length block data - NOTE: not implemented
  //data = "#8000000021111111100000000\n";
  //f.decode(data);
}

void mdtFrameCodecTest::mdtFrameCodecScpiTestU3606ATest()
{
  mdtFrameCodecScpiU3606A f;
  QByteArray data;
  QByteArray eofSeq = "\n";
  QVariant value;

  // Ihnitial states
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_UNKNOW);
  QVERIFY(!f.range().isValid());
  QVERIFY(!f.resolution().isValid());

  // Decode single value
  data = "+3.197000E-06\n";
  value = f.decodeSingleValueDouble(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.toDouble(), 3.197000E-06);
  QVERIFY(f.values().size() == 1);
  QCOMPARE(f.values().at(0).toDouble(), 3.197000E-06);

  // Decode single value: OL
  data = "+9.900000E+37\n";
  value = f.decodeSingleValueDouble(data);
  QVERIFY(!value.isValid());
  QCOMPARE(value.toDouble(), 0.0);
  QVERIFY(f.values().size() == 0);

  // Decode single value: -OL
  data = "-9.900000E+37\n";
  value = f.decodeSingleValueDouble(data);
  QVERIFY(!value.isValid());
  QCOMPARE(value.toDouble(), 0.0);
  QVERIFY(f.values().size() == 0);

  // Decode single value: invalid format
  data = "1.90000A0E+6\n";
  value = f.decodeSingleValueDouble(data);
  QVERIFY(!value.isValid());
  QCOMPARE(value.toDouble(), 0.0);
  QVERIFY(f.values().size() == 0);

  // Decoding CONF? query answer
  data = "VOLT +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_DC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-6);

  // Decoding CONF? query answer
  data = "VOLT:DC +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_DC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-6);

  // Decoding CONF? query answer
  data = "VOLT:AC +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_AC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-6);

  // Decoding CONF? query answer
  data = " CURR +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_DC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-5);

  // Decoding CONF? query answer
  data = " CURR:DC +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_DC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-5);

  // Decoding CONF? query answer
  data = " CURR:AC +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_AC);
  QVERIFY(f.range().isValid());
  QCOMPARE(f.range().toDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  QCOMPARE(f.resolution().toDouble(), 1e-5);

  // Decoding FUNC? query answer
  data = "VOLT:AC\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_AC);
  QVERIFY(!f.range().isValid());
  QVERIFY(!f.resolution().isValid());

  // Calibration secure string
  data = "CAL: 27 Nov 2009\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.values().size() == 2);
  QVERIFY(f.values().at(0) == "CAL");
  QVERIFY(f.values().at(1) == " 27 Nov 2009");

}
