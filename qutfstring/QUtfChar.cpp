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

#include "QUtfChar.h"
#include "QUtfString.h"


void QUtfChar::validate()
{
	if (QChar::isLowSurrogate(ucs4) || QChar::isHighSurrogate(ucs4) || ucs4 > 0x10FFFF)
		ucs4 = 0xFFFD;
}

QUtfChar::QUtfChar() :
	ucs4(0)
{ }

QUtfChar::QUtfChar(char ch) :
	ucs4(ch)
{
	validate();
}

QUtfChar::QUtfChar(uchar ch) :
	ucs4(ch)
{
	validate();
}

QUtfChar::QUtfChar(QChar ch) :
	ucs4(ch.unicode())
{
	validate();
}

QUtfChar::QUtfChar(int code) :
	ucs4((uint)code)
{
	validate();
}

QUtfChar::QUtfChar(uint code) :
	ucs4(code)
{
	validate();
}

bool QUtfChar::canConvert(QChar ch)
{
	return !ch.isLowSurrogate() && !ch.isHighSurrogate();
}

bool QUtfChar::canConvert(QUtfChar ch)
{
	return ch.isBmpCharacter();
}

QChar::Category QUtfChar::category() const
{
	return QChar::category(ucs4);
}

uint QUtfChar::codePoint() const
{
	return ucs4;
}

uchar QUtfChar::combiningClass() const
{
	return QChar::combiningClass(ucs4);
}

QUtfString QUtfChar::decomposition() const
{
	return QChar::decomposition(ucs4);
}

QChar::Decomposition QUtfChar::decompositionTag() const
{
	return QChar::decompositionTag(ucs4);
}

int QUtfChar::digitValue() const
{
	return QChar::digitValue(ucs4);
}

QChar::Direction QUtfChar::direction() const
{
	return QChar::direction(ucs4);
}

bool QUtfChar::hasMirrored() const
{
	return QChar::mirroredChar(ucs4) != ucs4;
}

bool QUtfChar::isBmpCharacter() const
{
	return ucs4 <= 0xFFFF;
}

bool QUtfChar::isDigit() const
{
	return category() == QChar::Number_DecimalDigit;
}

bool QUtfChar::isLetter() const
{
	switch (category())
	{
		case QChar::Letter_Uppercase:
		case QChar::Letter_Lowercase:
		case QChar::Letter_Titlecase:
		case QChar::Letter_Modifier:
		case QChar::Letter_Other:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isLetterOrNumber() const
{
	return isLetter() || isNumber();
}

bool QUtfChar::isLower() const
{
	return category() == QChar::Letter_Lowercase;
}

bool QUtfChar::isMark() const
{
	switch (category())
	{
		case QChar::Mark_NonSpacing:
		case QChar::Mark_SpacingCombining:
		case QChar::Mark_Enclosing:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isNull() const
{
	return ucs4 == 0;
}

bool QUtfChar::isNumber() const
{
	switch (category())
	{
		case QChar::Number_DecimalDigit:
		case QChar::Number_Letter:
		case QChar::Number_Other:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isPrint() const
{
	switch (category())
	{
		case QChar::Other_Control:
		case QChar::Other_NotAssigned:
			return false;
		default:
			return true;
	}
}

bool QUtfChar::isPunct() const
{
	switch (category())
	{
		case QChar::Punctuation_Connector:
		case QChar::Punctuation_Dash:
		case QChar::Punctuation_Open:
		case QChar::Punctuation_Close:
		case QChar::Punctuation_InitialQuote:
		case QChar::Punctuation_FinalQuote:
		case QChar::Punctuation_Other:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isSpace() const
{
	if (ucs4 >= 9 && ucs4 <= 13)
		return true;

	switch (category())
	{
		case QChar::Separator_Space:
		case QChar::Separator_Line:
		case QChar::Separator_Paragraph:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isSymbol() const
{
	switch (category())
	{
		case QChar::Symbol_Math:
		case QChar::Symbol_Currency:
		case QChar::Symbol_Modifier:
		case QChar::Symbol_Other:
			return true;
		default:
			return false;
	}
}

bool QUtfChar::isTitleCase() const
{
	return category() == QChar::Letter_Titlecase;
}

bool QUtfChar::isUpper() const
{
	return category() == QChar::Letter_Uppercase;
}

QChar::Joining QUtfChar::joining() const
{
	return QChar::joining(ucs4);
}

QUtfChar QUtfChar::mirroredChar() const
{
	return QChar::mirroredChar(ucs4);
}

QUtfChar::operator QChar() const
{
	if (isBmpCharacter())
		return QChar(ucs4);
	else
		return QChar(0xFFFD);
}

QUtfChar QUtfChar::toCaseFolded() const
{
	return QChar::toCaseFolded(ucs4);
}

QUtfChar QUtfChar::toLower() const
{
	return QChar::toLower(ucs4);
}

QUtfChar QUtfChar::toTitleCase() const
{
	return QChar::toTitleCase(ucs4);
}

QUtfChar QUtfChar::toUpper() const
{
	return QChar::toUpper(ucs4);
}

QChar::UnicodeVersion QUtfChar::unicodeVersion() const
{
	return QChar::unicodeVersion(ucs4);
}

bool operator!=(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() != c2.codePoint();
}

bool operator<(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() < c2.codePoint();
}

bool operator<=(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() <= c2.codePoint();
}

bool operator==(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() == c2.codePoint();
}

bool operator>(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() > c2.codePoint();
}

bool operator>=(QUtfChar c1, QUtfChar c2)
{
	return c1.codePoint() >= c2.codePoint();
}
