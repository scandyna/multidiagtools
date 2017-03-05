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
#include "WidgetStyleSheet.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QFont>
#include <QBrush>
#include <QColor>

namespace Mdt{ namespace ItemEditor{

void WidgetStyleSheet::setTextFont(const QFont & font)
{
  mTextFont = font;
}

void WidgetStyleSheet::setForegroundBrush(const QBrush & brush)
{
  mForegroundBrush = brush;
}

void WidgetStyleSheet::setBackgroundBrush(const QBrush & brush)
{
  mBackgroundBrush = brush;
}

void WidgetStyleSheet::clear()
{
  mTextFont.clear();
  mForegroundBrush.clear();
  mBackgroundBrush.clear();
}

bool WidgetStyleSheet::operator==(const WidgetStyleSheet & other)
{
  /*
   * QVariant operator== does the job the right way, just us it
   */
  return (mTextFont == other.mTextFont)
         && (mForegroundBrush == other.mForegroundBrush)
         && (mBackgroundBrush == other.mBackgroundBrush);
//   if(isNull() && other.isNull()){
//     return true;
//   }
//   return compareFontsEqual(mTextFont, other.mTextFont)
//          && compareBrushesEqual(mForegroundBrush, other.mForegroundBrush)
//          && compareBrushesEqual(mBackgroundBrush, other.mBackgroundBrush);
}

QString WidgetStyleSheet::toCssString() const
{
  QString css;

  if(isNull()){
    return css;
  }
  const auto textFont = getTextFontCssString();
  const auto foreground = getForegroundCssString();
  const auto background = getBackgroundCssString();

  css = QLatin1String("*{") % textFont;
  if(!foreground.isEmpty()){
    if( (css.length() > 2) && !css.endsWith(';') ){
      css += QLatin1String(";");
    }
    css += foreground;
  }
  if(!background.isEmpty()){
    if( (css.length() > 2) && !css.endsWith(';') ){
      css += QLatin1String(";");
    }
    css += background;
  }
  css += QLatin1String("}");

  return css;
}

QString WidgetStyleSheet::getTextFontCssString() const
{
  if(mTextFont.isNull()){
    return QString();
  }
  const auto font = mTextFont.value<QFont>();
  return   QLatin1String("font-family:\"") % font.family() % QLatin1String("\"") \
         % QLatin1String(";font-size:") % fontSizeString(font)     \
         % QLatin1String(";font-style:") % fontStyleString(font)   \
         % QLatin1String(";font-weight:") % fontWeightString(font);
}

QString WidgetStyleSheet::fontSizeString(const QFont & font) const
{
  if(font.pointSize() > 0 ){
    return QString::number(font.pointSize()) % QLatin1String("pt");
  }else if(font.pixelSize() > 0){
    return QString::number(font.pixelSize()) % QLatin1String("px");
  }
  return QString();
}

QString WidgetStyleSheet::fontStyleString(const QFont & font) const
{
  switch(font.style()){
    case QFont::StyleNormal:
      return QLatin1String("normal");
    case QFont::StyleItalic:
      return QLatin1String("italic");
    case QFont::StyleOblique:
      return QLatin1String("oblique");
  }
  return QString();
}

QString WidgetStyleSheet::fontWeightString(const QFont & font) const
{
  switch(font.weight()){
    case QFont::Normal:
      return QLatin1String("normal");
    case QFont::Bold:
      return QLatin1String("bold");
  }
  return QString();
}

QString WidgetStyleSheet::getForegroundCssString() const
{
  if(mForegroundBrush.isNull()){
    return QString();
  }
  const auto color = mForegroundBrush.value<QBrush>().color();
  return QLatin1String("color:") % color.name();
}

QString WidgetStyleSheet::getBackgroundCssString() const
{
  if(mBackgroundBrush.isNull()){
    return QString();
  }
  const auto color = mBackgroundBrush.value<QBrush>().color();
  return QLatin1String("background-color:") % color.name();
}

}} // namespace Mdt{ namespace ItemEditor{
