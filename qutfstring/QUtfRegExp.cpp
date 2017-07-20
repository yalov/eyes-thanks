/****************************************************************************
**
** Copyright (C) 2012 Mathias Kunter.
** Contact: mathiaskunter@gmail.com
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation. Please
** review the following information to ensure the GNU Lesser General Public
** License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "QUtfRegExp.h"
#include "QUtfString.h"
#include "QUtfStringList.h"
#include <QStringList>


QUtfRegExp::QUtfRegExp() :
	regex()
{ }

QUtfRegExp::QUtfRegExp(const QUtfString &pattern, Qt::CaseSensitivity cs, QRegExp::PatternSyntax syntax) :
	regex(pattern, cs, syntax)
{ }

QUtfRegExp::QUtfRegExp(const QUtfRegExp &rx) :
	regex(rx.regex)
{ }

QUtfRegExp::QUtfRegExp(const QRegExp &rx) :
	regex(rx)
{ }

QUtfString QUtfRegExp::cap(int nth) const
{
	return regex.cap(nth);
}

int QUtfRegExp::captureCount() const
{
	return regex.captureCount();
}

QUtfStringList QUtfRegExp::capturedTexts() const
{
	return regex.capturedTexts();
}

Qt::CaseSensitivity QUtfRegExp::caseSensitivity() const
{
	return regex.caseSensitivity();
}

QUtfString QUtfRegExp::errorString() const
{
	return regex.errorString();
}

QUtfString QUtfRegExp::escape(const QUtfString &str)
{
	return QRegExp::escape(str);
}

bool QUtfRegExp::exactMatch(const QUtfString &str) const
{
	return regex.exactMatch(str);
}

int QUtfRegExp::indexIn(const QUtfString &str, int offset, QRegExp::CaretMode caretMode) const
{
	return str.mapToCharIndex(regex.indexIn(str, str.mapToQtIndex(offset), caretMode));
}

bool QUtfRegExp::isEmpty() const
{
	return regex.isEmpty();
}

bool QUtfRegExp::isMinimal() const
{
	return regex.isMinimal();
}

bool QUtfRegExp::isValid() const
{
	return regex.isValid();
}

int QUtfRegExp::lastIndexIn(const QUtfString &str, int offset, QRegExp::CaretMode caretMode) const
{
	return str.mapToCharIndex(regex.lastIndexIn(str, str.mapToQtIndex(offset), caretMode));
}

int QUtfRegExp::matchedLength() const
{
	return regex.matchedLength();
}

QUtfString QUtfRegExp::pattern() const
{
	return regex.pattern();
}

QRegExp::PatternSyntax QUtfRegExp::patternSyntax() const
{
	return regex.patternSyntax();
}

int QUtfRegExp::pos(int nth) const
{
	return regex.pos(nth);
}

void QUtfRegExp::setCaseSensitivity(Qt::CaseSensitivity cs)
{
	regex.setCaseSensitivity(cs);
}

void QUtfRegExp::setMinimal(bool minimal)
{
	regex.setMinimal(minimal);
}

void QUtfRegExp::setPattern(const QUtfString &pattern)
{
	regex.setPattern(pattern);
}

void QUtfRegExp::setPatternSyntax(QRegExp::PatternSyntax syntax)
{
	regex.setPatternSyntax(syntax);
}

QUtfRegExp::operator QRegExp() const
{
	return regex;
}

bool QUtfRegExp::operator!=(const QUtfRegExp &rx) const
{
	return regex != rx.regex;
}

QUtfRegExp &QUtfRegExp::operator=(const QUtfRegExp &rx)
{
	regex = rx.regex;
	return *this;
}

bool QUtfRegExp::operator==(const QUtfRegExp &rx) const
{
	return regex == rx.regex;
}
