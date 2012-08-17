/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtFrameCodecTest.h"
#include "mdtFrameCodec.h"
#include "mdtFrameCodecAscii.h"
#include "mdtFrameCodecScpi.h"
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtFrameCodecModbus.h"
#include "mdtFrameCodecK8055.h"
#include "mdtApplication.h"
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

void mdtFrameCodecTest::mdtFrameCodecModbusTest()
{
  mdtFrameCodecModbus c;
  QByteArray pdu;
  
  // Initial states
  QVERIFY(c.values().size() == 0);
  
  // Check encode of ReadCoils
  pdu = c.encodeReadCoils(0x0203, 0x0408);
  QVERIFY(pdu.size() == 5);
  QVERIFY(pdu.at(0) == 0x01); // Function code
  QVERIFY(pdu.at(1) == 0x02); // Start address H
  QVERIFY(pdu.at(2) == 0x03); // Start address L
  QVERIFY(pdu.at(3) == 0x04); // Coils count H
  QVERIFY(pdu.at(4) == 0x08); // Coils count L
  
  // Check decode of ReadCoils
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(3);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 0100 0101
  pdu.append(0x81);   // 1000 0001
  QVERIFY(c.decode(pdu) == 0x01);
  QVERIFY(c.values().size() == 24);
  QVERIFY(c.values().at(0).toBool() == true);
  QVERIFY(c.values().at(1).toBool() == true);
  QVERIFY(c.values().at(2).toBool() == true);
  QVERIFY(c.values().at(3).toBool() == false);
  QVERIFY(c.values().at(4).toBool() == false);
  QVERIFY(c.values().at(5).toBool() == false);
  QVERIFY(c.values().at(6).toBool() == false);
  QVERIFY(c.values().at(7).toBool() == false);
  QVERIFY(c.values().at(8).toBool() == true);
  QVERIFY(c.values().at(9).toBool() == false);
  QVERIFY(c.values().at(10).toBool() == true);
  QVERIFY(c.values().at(11).toBool() == false);
  QVERIFY(c.values().at(12).toBool() == false);
  QVERIFY(c.values().at(13).toBool() == false);
  QVERIFY(c.values().at(14).toBool() == true);
  QVERIFY(c.values().at(15).toBool() == false);
  QVERIFY(c.values().at(16).toBool() == true);
  QVERIFY(c.values().at(17).toBool() == false);
  QVERIFY(c.values().at(18).toBool() == false);
  QVERIFY(c.values().at(19).toBool() == false);
  QVERIFY(c.values().at(20).toBool() == false);
  QVERIFY(c.values().at(21).toBool() == false);
  QVERIFY(c.values().at(22).toBool() == false);
  QVERIFY(c.values().at(23).toBool() == true);

  // Check decode of ReadCoils with errors in PDU
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(3);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 1000 0101
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(1);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 1000 0101
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);
  
  // Check the encode of WriteSingleCoil
  pdu = c.encodeWriteSingleCoil(0x1258, true);
  QVERIFY(pdu.at(0) == 0x05);         // Function code
  QVERIFY(pdu.at(1) == 0x12);         // Address H
  QVERIFY(pdu.at(2) == 0x58);         // Address L
  QVERIFY((quint8)pdu.at(3) == 0xFF); // Value H
  QVERIFY(pdu.at(4) == 0x00);         // Value L

  // Check decode of WriteSingleCoil
  pdu.clear();
  pdu.append(0x05);     // Function code
  pdu.append(0x25);     // Address H
  pdu.append(0x06);     // Address L
  pdu.append(0xFF);     // Value H
  pdu.append((char)0);  // Value L
  QVERIFY(c.decode(pdu) == 0x05);
  QVERIFY(c.values().size() == 1);
  QVERIFY(c.values().at(0).toBool() == true);
  pdu.clear();
  pdu.append(0x05);     // Function code
  pdu.append(0x25);     // Address H
  pdu.append(0x06);     // Address L
  pdu.append((char)0);  // Value H
  pdu.append((char)0);  // Value L
  QVERIFY(c.decode(pdu) == 0x05);
  QVERIFY(c.values().size() == 1);
  QVERIFY(c.values().at(0).toBool() == false);

}

/*
  k8055 frame formats, taken from libk8055:
   http://libk8055.sourceforge.net/

  Input packet format
  +---+---+---+---+---+---+---+---+
  |DIn|Sta|A1 |A2 |   C1  |   C2  |
  +---+---+---+---+---+---+---+---+
  DIn = Digital input in high nibble, except for input 3 in 0x01
  Sta = Status,x01 = OK ?
  A1  = Analog input 1, 0-255
  A2  = Analog input 2, 0-255
  C1  = Counter 1, 16 bits (lsb)
  C2  = Counter 2, 16 bits (lsb)

  Output packet format
  +---+---+---+---+---+---+---+---+
  |CMD|DIG|An1|An2|Rs1|Rs2|Dbv|Dbv|
  +---+---+---+---+---+---+---+---+
  CMD = Command 
  DIG = Digital output bitmask
  An1 = Analog output 1 value, 0-255
  An2 = Analog output 2 value, 0-255
  Rs1 = Reset counter 1, command 3
  Rs2 = Reset counter 3, command 4
  Dbv = Debounce value for counter 1 and 2, command 1 and 2

  Or split by commands

  Cmd 0, Reset ??
  Cmd 1, Set debounce Counter 1
  +---+---+---+---+---+---+---+---+
  |CMD|   |   |   |   |   |Dbv|   |
  +---+---+---+---+---+---+---+---+
  Cmd 2, Set debounce Counter 2
  +---+---+---+---+---+---+---+---+
  |CMD|   |   |   |   |   |   |Dbv|
  +---+---+---+---+---+---+---+---+
  Cmd 3, Reset counter 1
  +---+---+---+---+---+---+---+---+
  | 3 |   |   |   | 00|   |   |   |
  +---+---+---+---+---+---+---+---+
  Cmd 4, Reset counter 2
  +---+---+---+---+---+---+---+---+
  | 4 |   |   |   |   | 00|   |   |
  +---+---+---+---+---+---+---+---+
  cmd 5, Set analog/digital
  +---+---+---+---+---+---+---+---+
  | 5 |DIG|An1|An2|   |   |   |   |
  +---+---+---+---+---+---+---+---+
*/
void mdtFrameCodecTest::mdtFrameCodecK8055Test()
{
  mdtFrameCodecK8055 codec;
  QByteArray frame;

  /*
   * Initial states
   */
  QVERIFY(codec.values().size() == 0);

  /*
   * Decode test
   */
  frame.clear();
  // Set input 0, 3, 5 to 1
  frame.append((char)0x29);
  // Set statu to 1
  frame.append((char)0x01);
  // Set analog 1 to 89
  frame.append((char)0x59);
  // Set analog 2 to 253
  frame.append((char)0xFD);
  // Set counter 1 to 260
  frame.append((char)0x01);
  frame.append((char)0x04);
  // Set counter 2 to 4512
  frame.append((char)0x11);
  frame.append((char)0xA0);
  // Decode and check values
  QVERIFY(codec.decode(frame));
  QVERIFY(codec.values().size() == 12);
  QVERIFY(codec.values().at(0).type() == QVariant::Bool);
  QVERIFY(codec.values().at(0) == true);
  QVERIFY(codec.values().at(1).type() == QVariant::Bool);
  QVERIFY(codec.values().at(1) == false);
  QVERIFY(codec.values().at(2).type() == QVariant::Bool);
  QVERIFY(codec.values().at(2) == false);
  QVERIFY(codec.values().at(3).type() == QVariant::Bool);
  QVERIFY(codec.values().at(3) == true);
  QVERIFY(codec.values().at(4).type() == QVariant::Bool);
  QVERIFY(codec.values().at(4) == false);
  QVERIFY(codec.values().at(5).type() == QVariant::Bool);
  QVERIFY(codec.values().at(5) == true);
  QVERIFY(codec.values().at(6).type() == QVariant::Bool);
  QVERIFY(codec.values().at(6) == false);  QVERIFY(codec.values().at(5).type() == QVariant::Bool);
  QVERIFY(codec.values().at(7) == false);  QVERIFY(codec.values().at(5).type() == QVariant::Bool);
  QVERIFY(codec.values().at(7) == false);
  QVERIFY(codec.values().at(8).type() == QVariant::Int);
  QVERIFY(codec.values().at(8) == 89);
  QVERIFY(codec.values().at(9).type() == QVariant::Int);
  QVERIFY(codec.values().at(9) == 253);
  QVERIFY(codec.values().at(10).type() == QVariant::Int);
  QVERIFY(codec.values().at(10) == 260);
  QVERIFY(codec.values().at(11).type() == QVariant::Int);
  QVERIFY(codec.values().at(11) == 4512);

  /*
   * Encode reset
   */
  frame.clear();
  frame = codec.encodeReset();
  QVERIFY(frame.size() == 8);
  QVERIFY(frame.at(0) == 0x00);
  QVERIFY(frame.at(1) == 0x00);
  QVERIFY(frame.at(2) == 0x00);
  QVERIFY(frame.at(3) == 0x00);
  QVERIFY(frame.at(4) == 0x00);
  QVERIFY(frame.at(5) == 0x00);
  QVERIFY(frame.at(6) == 0x00);
  QVERIFY(frame.at(7) == 0x00);

  /*
   * Encode set counter debounce value
   */
  frame.clear();
  frame = codec.encodeSetCounterDebounceValue(1, 128);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x01);
  QVERIFY((quint8)frame.at(1) == 0x00);
  QVERIFY((quint8)frame.at(2) == 0x00);
  QVERIFY((quint8)frame.at(3) == 0x00);
  QVERIFY((quint8)frame.at(4) == 0x00);
  QVERIFY((quint8)frame.at(5) == 0x00);
  QVERIFY((quint8)frame.at(6) == 128);
  QVERIFY((quint8)frame.at(7) == 0x00);
  frame.clear();
  frame = codec.encodeSetCounterDebounceValue(2, 233);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x01);
  QVERIFY((quint8)frame.at(1) == 0x00);
  QVERIFY((quint8)frame.at(2) == 0x00);
  QVERIFY((quint8)frame.at(3) == 0x00);
  QVERIFY((quint8)frame.at(4) == 0x00);
  QVERIFY((quint8)frame.at(5) == 0x00);
  QVERIFY((quint8)frame.at(6) == 0x00);
  QVERIFY((quint8)frame.at(7) == 233);

  /*
   * Encode reset counter
   */
  frame.clear();
  frame = codec.encodeResetCounter(1);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x03);
  frame.clear();
  frame = codec.encodeResetCounter(2);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x04);

  /*
   * Encode Analog/Digital output
   */
  frame.clear();
  codec.setDigitalOut(1, true);
  codec.setDigitalOut(2, true);
  codec.setDigitalOut(7, true);
  codec.setAnalogOut(1, 57);
  codec.setAnalogOut(2, 254);
  frame = codec.encodeSetOutputs();
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x05);
  QVERIFY((quint8)frame.at(1) == 0x43);
  QVERIFY((quint8)frame.at(2) == 57);
  QVERIFY((quint8)frame.at(3) == 254);
  QVERIFY((quint8)frame.at(4) == 0x00);
  QVERIFY((quint8)frame.at(5) == 0x00);
  QVERIFY((quint8)frame.at(6) == 0x00);
  QVERIFY((quint8)frame.at(7) == 0x00);
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtFrameCodecTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
