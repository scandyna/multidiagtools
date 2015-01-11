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
#include "mdtCodecTest.h"
#include "mdtApplication.h"
#include "mdtCodecScpi.h"
#include <QString>

/*
 * SCPI codec
 */

void mdtCodecTest::scpiDecodeIdnTest()
{
  mdtCodecScpi codec;
  mdtScpiIdnResponse r;

  r = codec.decodeIdn("Agilent Technologies,U3606A,MY00000000,02.00-02.00-02.01\n");
  QCOMPARE(r.manufacturer, QString("Agilent Technologies"));
  QCOMPARE(r.model, QString("U3606A"));
  QCOMPARE(r.serial, QString("MY00000000"));
  QCOMPARE(r.firmwareLevel, QString("02.00-02.00-02.01"));
  r = codec.decodeIdn("jki");
  QVERIFY(r.manufacturer.isEmpty());
  QVERIFY(r.model.isEmpty());
  QVERIFY(r.serial.isEmpty());
  QVERIFY(r.firmwareLevel.isEmpty());
  r = codec.decodeIdn("");
  QVERIFY(r.manufacturer.isEmpty());
  QVERIFY(r.model.isEmpty());
  QVERIFY(r.serial.isEmpty());
  QVERIFY(r.firmwareLevel.isEmpty());

}

void mdtCodecTest::scpiDecodeIdnBenchmark()
{
  mdtScpiIdnResponse r;

  QBENCHMARK{
    mdtCodecScpi codec;
    r = codec.decodeIdn("Agilent Technologies,U3606A,MY00000000,02.00-02.00-02.01\n");
  }
  if(r.manufacturer.isEmpty()){
    QFAIL("Decoding failed");
  }
}

void mdtCodecTest::scpiDecodeBoolTest()
{
  mdtCodecScpi codec;
  mdtValueBool v;

  v = codec.decodeValueBool("0");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), false);
  v = codec.decodeValueBool(" 0");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), false);
  v = codec.decodeValueBool("0 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), false);
  v = codec.decodeValueBool(" 0 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), false);
  v = codec.decodeValueBool("0\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), false);
  v = codec.decodeValueBool("1");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), true);
  v = codec.decodeValueBool(" 1\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), true);
  v = codec.decodeValueBool("A");
  QVERIFY(v.isNull());
  v = codec.decodeValueBool("");
  QVERIFY(v.isNull());

}

void mdtCodecTest::scpiDecodeBoolBenchmark()
{
  mdtValueBool v;

  QBENCHMARK{
    mdtCodecScpi codec;
    v = codec.decodeValueBool("1");
  }
  if(v.isNull()){
    QFAIL("Decoding failed");
  }
}

void mdtCodecTest::scpiDecodeIntTest()
{
  mdtCodecScpi codec;
  mdtValueInt v;

  v = codec.decodeValueInt("2");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 2);
  v = codec.decodeValueInt(" 3");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 3);
  v = codec.decodeValueInt("4 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 4);
  v = codec.decodeValueInt(" 5\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 5);
  v = codec.decodeValueInt("+2");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 2);
  v = codec.decodeValueInt(" +3");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 3);
  v = codec.decodeValueInt("+4 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 4);
  v = codec.decodeValueInt(" +5\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 5);
  v = codec.decodeValueInt("0+2");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 2);
  v = codec.decodeValueInt(" 0+3");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 3);
  v = codec.decodeValueInt("0+4 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 4);
  v = codec.decodeValueInt(" 0+5\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), 5);
  v = codec.decodeValueInt("-2");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), -2);
  v = codec.decodeValueInt(" -3");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), -3);
  v = codec.decodeValueInt("-4 ");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), -4);
  v = codec.decodeValueInt(" -5\n");
  QVERIFY(!v.isNull());
  QCOMPARE(v.value(), -5);
  v = codec.decodeValueInt(" -5k\n");
  QVERIFY(v.isNull());
  v = codec.decodeValueInt("");
  QVERIFY(v.isNull());

}

void mdtCodecTest::scpiDecodeIntBenchmark()
{
  mdtValueInt v;

  QBENCHMARK{
    mdtCodecScpi codec;
    v = codec.decodeValueInt("1");
  }
  if(v.isNull()){
    QFAIL("Decoding failed");
  }
}

void mdtCodecTest::scpiDecodeDoubleTest()
{
  mdtCodecScpi codec;
  mdtValueDouble x;

  x = codec.decodeValueDouble("1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" 1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble("1 ");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" 1\n");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble("-1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), -1.0);
  x = codec.decodeValueDouble(" -1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), -1.0);
  x = codec.decodeValueDouble("-1 ");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), -1.0);
  x = codec.decodeValueDouble(" -1\n");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), -1.0);
  x = codec.decodeValueDouble("+1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" +1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble("+1 ");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" +1\n");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble("0+1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" 0+1");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble("0+1 ");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" 0+1\n");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.0);
  x = codec.decodeValueDouble(" +1.007850E+01\n");
  QVERIFY(!x.isNull());
  QCOMPARE(x.value(), 1.007850E+01);
  x = codec.decodeValueDouble("9.9e37");
  QVERIFY(!x.isNull());
  QVERIFY(x.isPlusOl());
  x = codec.decodeValueDouble(" 9.9e37\n");
  QVERIFY(!x.isNull());
  QVERIFY(x.isPlusOl());
  x = codec.decodeValueDouble(" 0+9.9e37\n");
  QVERIFY(!x.isNull());
  QVERIFY(x.isPlusOl());
  x = codec.decodeValueDouble("-9.9e37");
  QVERIFY(!x.isNull());
  QVERIFY(x.isMinusOl());
  x = codec.decodeValueDouble(" -9.9e37\n");
  QVERIFY(!x.isNull());
  QVERIFY(x.isMinusOl());
  x = codec.decodeValueDouble("9.91e37");
  QVERIFY(!x.isNull());
  QVERIFY(x.isNaN());
  x = codec.decodeValueDouble("A");
  QVERIFY(x.isNull());
  QVERIFY(x.isNaN());
  x = codec.decodeValueDouble("");
  QVERIFY(x.isNull());
  QVERIFY(x.isNaN());
}

void mdtCodecTest::scpiDecodeDoubleBenchmark()
{
  mdtValueDouble x;

  QBENCHMARK{
    mdtCodecScpi codec;
    x = codec.decodeValueDouble("1");
  }
  if(x.isNull()){
    QFAIL("Decoding failed");
  }
}

void mdtCodecTest::scpiDecodeErrorTest()
{
  mdtCodecScpi codec;
  mdtError e;

  e = codec.decodeDeviceError(" -133,\"Undefined header\"\n");
  QVERIFY(e.level() == mdtError::Error);
  QCOMPARE(e.systemNumber(), -133);
  QCOMPARE(e.systemText(), QString("Undefined header"));
  e = codec.decodeDeviceError("-131,\"Invalid suffix;FREQuency:CENT 2.0E+5 dbuV\"");
  QVERIFY(e.level() == mdtError::Error);
  QCOMPARE(e.systemNumber(), -131);
  QCOMPARE(e.systemText(), QString("Invalid suffix;FREQuency:CENT 2.0E+5 dbuV"));
  e = codec.decodeDeviceError("");
  QVERIFY(e.level() == mdtError::NoError);
}

void mdtCodecTest::scpiDecodeErrorBenchmark()
{
  mdtError e;

  QBENCHMARK{
    mdtCodecScpi codec;
    e = codec.decodeDeviceError("-133,\"Undefined header\"\n");
  }
  if(e.level() == mdtError::NoError){
    QFAIL("Decoding failed");
  }
}

void mdtCodecTest::scpiDecodeFunctionParameterTest()
{
  mdtCodecScpi codec;
  mdtScpiFunctionFarameter fp;

  /*
   * Check Function parameter's function part decoding
   */
  fp.functionRaw = "";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Unknown);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Unknown);
  fp.functionRaw = "VOLT";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "VOLTage";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "VOLT:DC";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "VOLT:AC";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac);
  fp.functionRaw = "CURR";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Current);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "CURRent";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Current);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "CURR:DC";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Current);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  fp.functionRaw = "CURR:AC";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Current);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac);
  fp.functionRaw = "VOLT:FREQ";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Frequency);
  fp.functionRaw = "VOLT:FREQ:BURS";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::FrequencyBurst);
  fp.functionRaw = "VOLT:FREQ:PRF";
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::FrequencyPrf);
  /*
   * Check complete decoding
   */
  fp = codec.decodeFunctionParameters("");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Unknown);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Unknown);
  QCOMPARE(fp.parametersDouble.size(), 0);
  fp = codec.decodeFunctionParameters(" VOLT +1.000000E+01,+1.000000E-06\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  QCOMPARE(fp.parametersDouble.size(), 2);
  QCOMPARE(fp.parametersDouble.at(0), 10.0);
  QCOMPARE(fp.parametersDouble.at(1), 1e-6);
  fp = codec.decodeFunctionParameters(" VOLT:AC +1.000000E+01,+1.000000E-06\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac);
  QCOMPARE(fp.parametersDouble.size(), 2);
  QCOMPARE(fp.parametersDouble.at(0), 10.0);
  QCOMPARE(fp.parametersDouble.at(1), 1e-6);
  fp = codec.decodeFunctionParameters(" VOLT\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  QCOMPARE(fp.parametersDouble.size(), 0);
  fp = codec.decodeFunctionParameters("VOLT\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc);
  QCOMPARE(fp.parametersDouble.size(), 0);
  fp = codec.decodeFunctionParameters(" VOLT:AC\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac);
  QCOMPARE(fp.parametersDouble.size(), 0);
  fp = codec.decodeFunctionParameters(" VOLT:AC +1.000000E+01,+1.000000E-06,@1,@3\n");
  QVERIFY(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage);
  QVERIFY(fp.simpleMeasurementCommand() == mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac);
  QCOMPARE(fp.parametersDouble.size(), 2);
  QCOMPARE(fp.parametersDouble.at(0), 10.0);
  QCOMPARE(fp.parametersDouble.at(1), 1e-6);


}

void mdtCodecTest::scpiDecodeFunctionParameterBenchmark()
{
  mdtScpiFunctionFarameter fp;

  QBENCHMARK{
    mdtCodecScpi codec;
    fp = codec.decodeFunctionParameters(" VOLT +1.000000E+01,+1.000000E-06\n");
  }
  if(fp.fundamentalMeasurementCommand() == mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Unknown){
    QFAIL("Decoding failed");
  }
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtCodecTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
