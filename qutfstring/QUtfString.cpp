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

#include "QUtfString.h"
#include "QUtfRegExp.h"
#include "QUtfStringList.h"


QUtfString::QUtfString(const QString &other, bool countSurrogates) :
	s(other)
{
	if (countSurrogates)
		this->countSurrogates();
	else
	{
		pairCount = 0;
		lastCharIndex = 0;
		lastQtIndex = 0;
	}
}

QUtfString::QUtfString(const QString &other, int surrogateCount) :
	s(other),
	pairCount(surrogateCount),
	lastCharIndex(0),
	lastQtIndex(0)
{ }

void QUtfString::countSurrogates()
{
	// Initialize values with 0.
	pairCount = 0;

	// Scan the first half of the string.
	int mid = s.size() / 2;
	int i = 0;
	for (; i < mid; i++)
	{
		if (s.at(i).isHighSurrogate())
		{
			pairCount++;
			i++;
		}
	}

	// Store the indices of the current character, which is located in the middle of the string data.
	lastCharIndex = i - pairCount;
	lastQtIndex = i;

	// Scan the second half of the string. We use a separate loop for this due to performance reasons.
	for (; i < s.size(); i++)
	{
		if (s.at(i).isHighSurrogate())
		{
			pairCount++;
			i++;
		}
	}
}

void QUtfString::addSurrogateRange(int qtIndex, int qtCount)
{
	int addedPairs = 0;
	for (int i = qtIndex; i < qtIndex + qtCount; i++)
	{
		if (s.at(i).isHighSurrogate())
		{
			addedPairs++;
			i++;
		}
	}
	pairCount += addedPairs;

	if (qtIndex <= lastQtIndex)
	{
		lastQtIndex += qtCount;
		lastCharIndex += qtCount - addedPairs;
	}
}

void QUtfString::removeSurrogateRange(int qtIndex, int qtCount)
{
	if (pairCount == 0)
		return;

	int removedPairs = 0;
	for (int i = qtIndex; i < qtIndex + qtCount; i++)
	{
		if (s.at(i).isHighSurrogate())
		{
			removedPairs++;
			i++;
		}
	}
	pairCount -= removedPairs;

	if (qtIndex + qtCount <= lastQtIndex)
	{
		// Full deletion. Shift the indices.
		lastQtIndex -= qtCount;
		lastCharIndex -= qtCount + removedPairs;
	}
	else if (qtIndex <= lastQtIndex)
	{
		// Partial deletion. Reset the indices.
		lastQtIndex = 0;
		lastCharIndex = 0;
	}
}

int QUtfString::mapToCharIndex(int qtIndex) const
{
	// We don't have to do any mapping if there are no surrogate pairs present at all.
	if (pairCount == 0 || qtIndex == 0 || qtIndex == -1)
		return qtIndex;

	// Negative indices can be used to indicate that we should start counting characters from the end of the string instead of the beginning. Convert the index to positive in this case.
	bool isNegative = qtIndex < 0;
	if (isNegative)
		qtIndex += s.size();

	// We must also support indices which are out of bounds. This can be the case with both positive and negative indices.
	if (qtIndex >= s.size())
		return qtIndex - pairCount;
	else if (qtIndex < 0)
		return qtIndex - s.size() + pairCount;			// Convert the index back to negative.

	// Scan the string from either the beginning, the end, or the last access position, whichever is fastest.
	int back = s.size() - 1 - qtIndex;
	int last = qAbs(lastQtIndex - qtIndex);

	int charIndex;
	int loopIndex;
	bool forward;
	if (qtIndex < back && qtIndex < last)
	{
		// Scan the string from the beginning.
		charIndex = 0;
		loopIndex = 0;
		forward = true;
	}
	else if (back < qtIndex && back < last)
	{
		// Scan the string from the end.
		charIndex = s.size() - pairCount - 1;
		loopIndex = s.size() - 1;
		forward = false;
	}
	else
	{
		// Scan the string from the last access position.
		charIndex = lastCharIndex;
		loopIndex = lastQtIndex;
		forward = qtIndex > lastQtIndex;
	}

	if (forward)
	{
		// Scan the string forwards.
		for (; loopIndex < qtIndex; loopIndex++)
		{
			if (!s.at(loopIndex).isHighSurrogate())
				charIndex++;
		}
	}
	else
	{
		// Scan the string backwards.
		for (; loopIndex > qtIndex; loopIndex--)
		{
			if (!s.at(loopIndex).isLowSurrogate())
				charIndex--;
		}
	}

	// Store the last access position. Since we also have to update those values in const functions, we unfortunately have to use const_cast here.
	// This could be avoided by using a data pointer instead of integer members.
	const_cast<QUtfString *>(this)->lastQtIndex = qtIndex;
	const_cast<QUtfString *>(this)->lastCharIndex = charIndex;

	// Convert the index back to negative, if required.
	if (!isNegative)
		return charIndex;
	else
		return charIndex - s.size() + pairCount;
}

int QUtfString::mapToQtIndex(int charIndex) const
{
	// We don't have to do any mapping if there are no surrogate pairs present at all.
	if (pairCount == 0 || charIndex == 0)
		return charIndex;

	// Negative indices can be used to indicate that we should start counting characters from the end of the string instead of the beginning. Convert the index to positive in this case.
	bool isNegative = charIndex < 0;
	if (isNegative)
		charIndex += s.size() - pairCount;

	// We must also support indices which are out of bounds. This can be the case with both positive and negative indices.
	if (charIndex >= s.size() - pairCount)
		return charIndex + pairCount;
	else if (charIndex < 0)
		return charIndex - s.size();			// Convert the index back to negative.

	// Scan the string from either the beginning, the end, or the last access position, whichever is fastest.
	int back = s.size() - pairCount - 1 - charIndex;
	int last = qAbs(lastCharIndex - charIndex);

	int qtIndex;
	int loopIndex;
	bool forward;
	if (charIndex < back && charIndex < last)
	{
		// Scan the string from the beginning.
		qtIndex = 0;
		loopIndex = 0;
		forward = true;
	}
	else if (back < charIndex && back < last)
	{
		// Scan the string from the end.
		qtIndex = s.size() - 1;
		loopIndex = s.size() - pairCount - 1;
		forward = false;
	}
	else
	{
		// Scan the string from the last access position.
		qtIndex = lastQtIndex;
		loopIndex = lastCharIndex;
		forward = charIndex > lastCharIndex;
	}

	if (forward)
	{
		// Scan the string forwards.
		for (; loopIndex < charIndex; loopIndex++)
		{
			if (!s.at(qtIndex).isHighSurrogate())
				qtIndex++;
			else
				qtIndex += 2;
		}
	}
	else
	{
		// Scan the string backwards.
		for (; loopIndex > charIndex; loopIndex--)
		{
			if (!s.at(qtIndex).isLowSurrogate())
				qtIndex--;
			else
				qtIndex -= 2;
		}

		if (s.at(qtIndex).isLowSurrogate())
			qtIndex--;
	}

	// Store the last access position. Since we also have to update those values in const functions, we unfortunately have to use const_cast here.
	// This could be avoided by using a data pointer instead of integer members.
	const_cast<QUtfString *>(this)->lastQtIndex = qtIndex;
	const_cast<QUtfString *>(this)->lastCharIndex = charIndex;

	// Convert the index back to negative, if required.
	if (!isNegative)
		return qtIndex;
	else
		return qtIndex - s.size();
}

QUtfString::QUtfString() :
	pairCount(0),
	lastCharIndex(0),
	lastQtIndex(0)
{ }

QUtfString::QUtfString(QUtfChar ch) :
	pairCount(0),
	lastCharIndex(0),
	lastQtIndex(0)
{
	append(ch);
}

QUtfString::QUtfString(int size, QUtfChar ch) :
	pairCount(0),
	lastCharIndex(size / 2)
{
	if (ch.isBmpCharacter())
	{
		s.reserve(size);
		lastQtIndex = lastCharIndex;
	}
	else
	{
		s.reserve(size * 2);
		lastQtIndex = lastCharIndex * 2;
	}
	for (int i = 0; i < size; i++)
		append(ch);
}

QUtfString::QUtfString(const QUtfString &other) :
	s(other.s),
	pairCount(other.pairCount),
	lastCharIndex(other.lastCharIndex),
	lastQtIndex(other.lastQtIndex)
{ }

QUtfString::QUtfString(const QString &other) :
	s(other)
{
	countSurrogates();
}

QUtfString::QUtfString(const char *str) :
	s(str)
{
	countSurrogates();
}

QUtfString &QUtfString::append(const QUtfString &str)
{
	s.append(str.s);
	pairCount += str.pairCount;
	return *this;
}

QUtfString &QUtfString::append(const char *str)
{
	int qtSize = s.size();
	s.append(str);
	addSurrogateRange(qtSize, s.size() - qtSize);
	return *this;
}

QUtfString &QUtfString::append(QUtfChar ch)
{
	if (ch.isBmpCharacter())
		s.append(ch);
	else
	{
		s.append(QChar(QChar::highSurrogate((uint)ch.codePoint())));
		s.append(QChar(QChar::lowSurrogate((uint)ch.codePoint())));
		pairCount++;
	}
	return *this;
}

QUtfString QUtfString::arg(const QUtfString &a, int minFieldWidth, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a.s, minFieldWidth, fillChar),
		pairCount > 0 ||
		a.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2) const
{
	return QUtfString(s.arg(a1.s, a2.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s, a5.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0 ||
		a5.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s, a5.s, a6.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0 ||
		a5.pairCount > 0 ||
		a6.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s, a5.s, a6.s, a7.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0 ||
		a5.pairCount > 0 ||
		a6.pairCount > 0 ||
		a7.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7, const QUtfString &a8) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s, a5.s, a6.s, a7.s, a8.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0 ||
		a5.pairCount > 0 ||
		a6.pairCount > 0 ||
		a7.pairCount > 0 ||
		a8.pairCount > 0);
}

QUtfString QUtfString::arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7, const QUtfString &a8, const QUtfString &a9) const
{
	return QUtfString(s.arg(a1.s, a2.s, a3.s, a4.s, a5.s, a6.s, a7.s, a8.s, a9.s),
		pairCount > 0 ||
		a1.pairCount > 0 ||
		a2.pairCount > 0 ||
		a3.pairCount > 0 ||
		a4.pairCount > 0 ||
		a5.pairCount > 0 ||
		a6.pairCount > 0 ||
		a7.pairCount > 0 ||
		a8.pairCount > 0 ||
		a9.pairCount > 0);
}

QUtfString QUtfString::arg(int a, int minFieldWidth, int base, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, base, fillChar), pairCount);
}

QUtfString QUtfString::arg(uint a, int minFieldWidth, int base, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, base, fillChar), pairCount);
}

QUtfString QUtfString::arg(qlonglong a, int minFieldWidth, int base, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, base, fillChar), pairCount);
}

QUtfString QUtfString::arg(qulonglong a, int minFieldWidth, int base, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, base, fillChar), pairCount);
}

QUtfString QUtfString::arg(QUtfChar a, int minFieldWidth, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	if (a.isBmpCharacter())
		return QUtfString(s.arg(a, minFieldWidth, fillChar), pairCount);
	else
		return QUtfString(s.arg(QUtfString(a).s, minFieldWidth, fillChar), true);
}

QUtfString QUtfString::arg(char a, int minFieldWidth, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, fillChar), pairCount);
}

QUtfString QUtfString::arg(double a, int minFieldWidth,  char format, int precision, QUtfChar fillChar) const
{
	if (!fillChar.isBmpCharacter())
		fillChar = ' ';
	return QUtfString(s.arg(a, minFieldWidth, format, precision, fillChar), pairCount);
}

const QUtfChar QUtfString::at(int position) const
{
	if (pairCount == 0)
		return QUtfChar(s.at(position));

	int qtIndex = mapToQtIndex(position);
	QChar c = s.at(qtIndex);
	if (!c.isHighSurrogate())
		return QUtfChar(c);
	else
		return QUtfChar(QChar::surrogateToUcs4(c, s.at(qtIndex + 1)));
}

int QUtfString::capacity() const
{
	return s.capacity();
}

void QUtfString::chop(int n)
{
	if (n >= size())
		clear();
	else if (n > 0)
		resize(size() - n);
}

void QUtfString::clear()
{
	s.clear();
	pairCount = 0;
	lastCharIndex = 0;
	lastQtIndex = 0;
}

int QUtfString::compare(const QUtfString &other, Qt::CaseSensitivity cs) const
{
	return s.compare(other.s, cs);
}

int QUtfString::compare(const QUtfString &s1, const QUtfString &s2, Qt::CaseSensitivity cs)
{
	return QString::compare(s1.s, s2.s, cs);
}

bool QUtfString::contains(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	return s.contains(str.s, cs);
}

bool QUtfString::contains(QUtfChar ch, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return s.contains(ch, cs);
	else
		return s.contains(QUtfString(ch).s, cs);
}

bool QUtfString::contains(const QUtfRegExp &rx) const
{
	return s.contains(rx);
}

bool QUtfString::contains(QUtfRegExp &rx) const
{
	return s.contains(rx.regex);
}

int QUtfString::count(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	return s.count(str.s, cs);
}

int QUtfString::count(QUtfChar ch, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return s.count(ch, cs);
	else
		return s.count(QUtfString(ch).s, cs);
}

int QUtfString::count(const QUtfRegExp &rx) const
{
	return s.count(rx);
}

const quint16 *QUtfString::data() const
{
	return (const quint16 *)s.data();
}

bool QUtfString::endsWith(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	return s.endsWith(str.s, cs);
}

bool QUtfString::endsWith(const QUtfChar &ch, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return s.endsWith(ch, cs);
	else
		return s.endsWith(QUtfString(ch).s, cs);
}

bool QUtfString::equals(const QUtfString &other, Qt::CaseSensitivity cs) const
{
	return compare(other, cs) == 0;
}

bool QUtfString::equals(const QUtfString &s1, const QUtfString &s2, Qt::CaseSensitivity cs)
{
	return compare(s1, s2, cs) == 0;
}

QUtfString &QUtfString::fill(QUtfChar ch, int size)
{
	if (size < 0)
		size = this->size();

	clear();

	lastCharIndex = size / 2;
	if (ch.isBmpCharacter())
	{
		s.reserve(size);
		lastQtIndex = lastCharIndex;
	}
	else
	{
		s.reserve(size * 2);
		lastQtIndex = lastCharIndex * 2;
	}
	for (int i = 0; i < size; i++)
		append(ch);

	return *this;
}

QUtfString QUtfString::fromRawData(const quint16 *utf16, int size)
{
	return QUtfString(QString::fromRawData((const QChar *)utf16, size), true);
}

QUtfString QUtfString::fromRawData(const quint16 *utf16, int size, int surrogateCount)
{
	return QUtfString(QString::fromRawData((const QChar *)utf16, size), surrogateCount);
}

int QUtfString::indexOf(const QUtfString &str, int from, Qt::CaseSensitivity cs) const
{
	return mapToCharIndex(s.indexOf(str.s, mapToQtIndex(from), cs));
}

int QUtfString::indexOf(QUtfChar ch, int from, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return mapToCharIndex(s.indexOf(ch, mapToQtIndex(from), cs));
	else
		return mapToCharIndex(s.indexOf(QUtfString(ch).s, mapToQtIndex(from), cs));
}

int QUtfString::indexOf(const QUtfRegExp &rx, int from) const
{
	return mapToCharIndex(s.indexOf(rx, mapToQtIndex(from)));
}

int QUtfString::indexOf(QUtfRegExp &rx, int from) const
{
	return mapToCharIndex(s.indexOf(rx.regex, mapToQtIndex(from)));
}

QUtfString &QUtfString::insert(int position, const QUtfString &str)
{
	s.insert(mapToQtIndex(position), str.s);
	pairCount += str.pairCount;
	if (position <= lastCharIndex)
	{
		lastCharIndex += str.size();
		lastQtIndex += str.s.size();
	}
	return *this;
}

QUtfString &QUtfString::insert(int position, QUtfChar ch)
{
	if (ch.isBmpCharacter())
		s.insert(mapToQtIndex(position), ch);
	else
	{
		s.insert(mapToQtIndex(position), QUtfString(ch).s);
		pairCount++;
	}
	if (position <= lastCharIndex)
	{
		lastCharIndex++;
		if (ch.isBmpCharacter())
			lastQtIndex++;
		else
			lastQtIndex += 2;
	}
	return *this;
}

bool QUtfString::isEmpty() const
{
	return s.isEmpty();
}

bool QUtfString::isNumeric(int base) const
{
	bool success;
	s.toInt(&success, base);
	return success;
}

int QUtfString::lastIndexOf(const QUtfString &str, int from, Qt::CaseSensitivity cs) const
{
	return mapToCharIndex(s.lastIndexOf(str.s, mapToQtIndex(from), cs));
}

int QUtfString::lastIndexOf(QUtfChar ch, int from, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return mapToCharIndex(s.lastIndexOf(ch, mapToQtIndex(from), cs));
	else
		return mapToCharIndex(s.lastIndexOf(QUtfString(ch).s, mapToQtIndex(from), cs));
}

int QUtfString::lastIndexOf(const QUtfRegExp &rx, int from) const
{
	return mapToCharIndex(s.lastIndexOf(rx, mapToQtIndex(from)));
}

int QUtfString::lastIndexOf(QUtfRegExp &rx, int from) const
{
	return mapToCharIndex(s.lastIndexOf(rx.regex, mapToQtIndex(from)));
}

QUtfString QUtfString::left(int n) const
{
	if (n <= 0 || size() == 0)
		return QUtfString();
	else if (n >= size())
		return *this;
	else
		return QUtfString(s.left(mapToQtIndex(n)), pairCount > 0);
}

QUtfString QUtfString::leftJustified(int width, QUtfChar fillChar, bool truncate) const
{
	if (size() > width)
	{
		if (!truncate)
			return *this;
		else
			return left(width);
	}
	else
		return *this + QUtfString(width - size(), fillChar);
}

QUtfString QUtfString::leftRef(int n) const
{
	if (n <= 0 || size() == 0)
		return QUtfString();
	else if (n >= size())
		return *this;
	else
		return QUtfString(QString::fromRawData(s.constData(), mapToQtIndex(n)), pairCount > 0);
}

int QUtfString::localeAwareCompare(const QUtfString &other) const
{
	return s.localeAwareCompare(other.s);
}

int QUtfString::localeAwareCompare(const QUtfString &s1, const QUtfString &s2)
{
	return QString::localeAwareCompare(s1.s, s2.s);
}

bool QUtfString::localeAwareEquals(const QUtfString &other) const
{
	return localeAwareCompare(other) == 0;
}

bool QUtfString::localeAwareEquals(const QUtfString &s1, const QUtfString &s2)
{
	return localeAwareCompare(s1, s2) == 0;
}

QUtfString QUtfString::mid(int position, int n) const
{
	if (position < 0)
		position = 0;
	if (position >= size() || n == 0)
		return QUtfString();

	if (n < 0)
		return QUtfString(s.mid(mapToQtIndex(position)), pairCount > 0);
	else
	{
		int qtPosition = mapToQtIndex(position);
		int qtN = mapToQtIndex(position + n) - qtPosition;
		return QUtfString(s.mid(qtPosition, qtN), pairCount > 0);
	}
}

QUtfString QUtfString::midRef(int position, int n) const
{
	if (position < 0)
		position = 0;
	if (position >= size() || n == 0)
		return QUtfString();
	if (n < 0 || position + n > size())
		n = size() - position;

	int qtPosition = mapToQtIndex(position);
	int qtN = mapToQtIndex(position + n) - qtPosition;
	return QUtfString(QString::fromRawData(s.constData() + qtPosition, qtN), pairCount > 0);
}

QUtfString QUtfString::normalized(QString::NormalizationForm mode) const
{
	return QUtfString(s.normalized(mode));
}

QUtfString QUtfString::normalized(QString::NormalizationForm mode, QChar::UnicodeVersion version) const
{
	return QUtfString(s.normalized(mode, version));
}

QUtfString QUtfString::number(int n, int base)
{
	return QUtfString(QString::number(n, base), false);
}

QUtfString QUtfString::number(uint n, int base)
{
	return QUtfString(QString::number(n, base), false);
}

QUtfString QUtfString::number(qlonglong n, int base)
{
	return QUtfString(QString::number(n, base), false);
}

QUtfString QUtfString::number(qulonglong n, int base)
{
	return QUtfString(QString::number(n, base), false);
}

QUtfString QUtfString::number(double n, char format, int precision)
{
	return QUtfString(QString::number(n, format, precision), false);
}

QUtfString &QUtfString::prepend(const QUtfString &str)
{
	s.prepend(str.s);
	pairCount += str.pairCount;
	lastCharIndex += str.size();
	lastQtIndex += str.s.size();
	return *this;
}

QUtfString &QUtfString::prepend(const char *str)
{
	int qtSize = s.size();
	s.prepend(str);
	addSurrogateRange(0, s.size() - qtSize);
	return *this;
}

QUtfString &QUtfString::prepend(QUtfChar ch)
{
	if (ch.isBmpCharacter())
		s.prepend(ch);
	else
	{
		s.prepend(QUtfString(ch).s);
		pairCount++;
	}
	lastCharIndex++;
	if (ch.isBmpCharacter())
		lastQtIndex++;
	else
		lastQtIndex += 2;
	return *this;
}

QUtfString &QUtfString::remove(int position, int n)
{
	int qtPosition = mapToQtIndex(position);
	int qtN = mapToQtIndex(position + n) - qtPosition;
	removeSurrogateRange(qtPosition, qtN);
	s.remove(qtPosition, qtN);
	return *this;
}

QUtfString &QUtfString::remove(QUtfChar ch, Qt::CaseSensitivity cs)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	if (ch.isBmpCharacter())
		s.remove(ch, cs);
	else
	{
		int qtSize = s.size();
		s.remove(QUtfString(ch).s, cs);
		if (pairCount > 0 && s.size() < qtSize)
			countSurrogates();
	}
	return *this;
}

QUtfString &QUtfString::remove(const QUtfString &str, Qt::CaseSensitivity cs)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	int qtSize = s.size();
	s.remove(str.s, cs);
	if (pairCount > 0 && str.pairCount > 0 && s.size() < qtSize)
		countSurrogates();
	return *this;
}

QUtfString &QUtfString::remove(const QUtfRegExp &rx)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	int qtSize = s.size();
	s.remove(rx);
	if (pairCount > 0 && s.size() < qtSize)
		countSurrogates();
	return *this;
}

QUtfString QUtfString::repeated(int times) const
{
	return QUtfString(s.repeated(times), pairCount * qMax(0, times));
}

QUtfString &QUtfString::replace(int position, int n, const QUtfString &after)
{
	int qtPosition = mapToQtIndex(position);
	int qtN = mapToQtIndex(position + n) - qtPosition;
	removeSurrogateRange(qtPosition, qtN);
	s.replace(qtPosition, qtN, after.s);
	pairCount += after.pairCount;
	if (position <= lastCharIndex)
	{
		lastCharIndex += after.size();
		lastQtIndex += after.s.size();
	}
	return *this;
}

QUtfString &QUtfString::replace(int position, int n, QUtfChar after)
{
	int qtPosition = mapToQtIndex(position);
	int qtN = mapToQtIndex(position + n) - qtPosition;
	removeSurrogateRange(qtPosition, qtN);
	if (after.isBmpCharacter())
		s.replace(qtPosition, qtN, after);
	else
	{
		s.replace(qtPosition, qtN, QUtfString(after).s);
		pairCount++;
	}
	if (position <= lastCharIndex)
	{
		lastCharIndex++;
		if (after.isBmpCharacter())
			lastQtIndex++;
		else
			lastQtIndex += 2;
	}
	return *this;
}

QUtfString &QUtfString::replace(const QUtfString &before, const QUtfString &after, Qt::CaseSensitivity cs)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	s.replace(before.s, after.s, cs);
	bool a = pairCount > 0;
	bool b = before.pairCount > 0;
	bool c = after.pairCount > 0;
	if ((!a && !b && c) || (a && (b || c)))
		countSurrogates();
	return *this;
}

QUtfString &QUtfString::replace(QUtfChar ch, const QUtfString &after, Qt::CaseSensitivity cs)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	if (ch.isBmpCharacter())
		s.replace(ch, after.s, cs);
	else
		s.replace(QUtfString(ch).s, after.s, cs);
	bool a = pairCount > 0;
	bool b = !ch.isBmpCharacter();
	bool c = after.pairCount > 0;
	if ((!a && !b && c) || (a && (b || c)))
		countSurrogates();
	return *this;
}

QUtfString &QUtfString::replace(QUtfChar before, QUtfChar after, Qt::CaseSensitivity cs)
{
	lastCharIndex = 0;
	lastQtIndex = 0;
	s.replace(QUtfString(before).s, QUtfString(after).s, cs);
	bool a = pairCount > 0;
	bool b = !before.isBmpCharacter();
	bool c = !after.isBmpCharacter();
	if ((!a && !b && c) || (a && (b || c)))
		countSurrogates();
	return *this;
}

QUtfString &QUtfString::replace(const QUtfRegExp &rx, const QUtfString &after)
{
	s.replace(rx, after.s);
	countSurrogates();
	return *this;
}

void QUtfString::reserve(int size)
{
	s.reserve(size);
}

void QUtfString::resize(int size)
{
	if (size < this->size())
	{
		int qtPosition = mapToQtIndex(size);
		removeSurrogateRange(qtPosition, s.size() - qtPosition);
		s.resize(qtPosition);
	}
	else if (size > this->size())
	{
		int qtSize = s.size();
		s.resize(mapToQtIndex(size));

		// We have to initialize the new characters with 0, since uninitialized characters might contain invalid surrogate codepoints.
		QChar *data = s.data();
		for (int i = qtSize; i < s.size(); i++)
			data[i] = 0;
	}
}

QUtfString QUtfString::right(int n) const
{
	if (n <= 0 || size() == 0)
		return QUtfString();
	else if (n >= size())
		return *this;
	else
		return QUtfString(s.right(s.size() - mapToQtIndex(size() - n)), pairCount > 0);
}

QUtfString QUtfString::rightJustified(int width, QUtfChar fillChar, bool truncate) const
{
	if (size() > width)
	{
		if (!truncate)
			return *this;
		else
			return left(width);
	}
	else
		return QUtfString(width - size(), fillChar) + *this;
}

QUtfString QUtfString::rightRef(int n) const
{
	if (n <= 0 || size() == 0)
		return QUtfString();
	else if (n >= size())
		return *this;
	else
	{
		int qtPosition = mapToQtIndex(size() - n);
		int qtN = sizeCodeUnits() - qtPosition;
		return QUtfString(QString::fromRawData(s.constData() + qtPosition, qtN), pairCount > 0);
	}
}

QUtfString QUtfString::section(QUtfChar sep, int start, int end, QString::SectionFlags flags) const
{
	if (sep.isBmpCharacter())
		return QUtfString(s.section(sep, start, end, flags), pairCount > 0);
	else
		return QUtfString(s.section(QUtfString(sep).s, start, end, flags), pairCount > 0);
}

QUtfString QUtfString::section(const QUtfString &sep, int start, int end, QString::SectionFlags flags) const
{
	return QUtfString(s.section(sep.s, start, end, flags), pairCount > 0);
}

QUtfString QUtfString::section(const QUtfRegExp &reg, int start, int end, QString::SectionFlags flags) const
{
	return QUtfString(s.section(reg, start, end, flags), pairCount > 0);
}

void QUtfString::setAt(int position, QUtfChar ch)
{
	int qtIndex = mapToQtIndex(position);
	bool oldSurrogate = s.at(qtIndex).isHighSurrogate();
	bool newSurrogate = !ch.isBmpCharacter();

	if (!oldSurrogate && !newSurrogate)
		s[qtIndex] = ch;
	else if (!oldSurrogate && newSurrogate)
	{
		s[qtIndex] = QChar(QChar::highSurrogate((uint)ch.codePoint()));
		s.insert(qtIndex + 1, QChar(QChar::lowSurrogate((uint)ch.codePoint())));
		pairCount++;
		if (position < lastCharIndex)
			lastQtIndex++;
	}
	else if (oldSurrogate && !newSurrogate)
	{
		s[qtIndex] = ch;
		s.remove(qtIndex + 1, 1);
		pairCount--;
		if (position < lastCharIndex)
			lastQtIndex--;
	}
	else // if (oldSurrogate && newSurrogate)
	{
		s[qtIndex] = QChar(QChar::highSurrogate((uint)ch.codePoint()));
		s[qtIndex + 1] = QChar(QChar::lowSurrogate((uint)ch.codePoint()));
	}
}

QUtfString &QUtfString::setRawData(const quint16 *utf16, int size)
{
	s.setRawData((const QChar *)utf16, size);
	countSurrogates();
	return *this;
}

QUtfString &QUtfString::setRawData(const quint16 *utf16, int size, int surrogateCount)
{
	s.setRawData((const QChar *)utf16, size);
	pairCount = surrogateCount;
	lastCharIndex = 0;
	lastQtIndex = 0;
	return *this;
}

QUtfString QUtfString::simplified() const
{
	return QUtfString(s.simplified(), pairCount > 0);
}

int QUtfString::size() const
{
	return s.size() - pairCount;
}

int QUtfString::sizeCodeUnits() const
{
	return s.size();
}

QUtfStringList QUtfString::split(const QUtfString &sep, int maxSplitCount, QString::SplitBehavior behavior, Qt::CaseSensitivity cs) const
{
	if (maxSplitCount == 0)
		return QUtfStringList();
	else if (maxSplitCount == 1)
		return QUtfStringList(*this);

	QUtfStringList returnList;

	int startIndex = 0, searchIndex = 0;
	while (true)
	{
		int endIndex = indexOf(sep, searchIndex, cs);
		bool isLast = endIndex == -1;
		if (!isLast)
		{
			returnList.append(mid(startIndex, endIndex - startIndex));
			searchIndex = endIndex + std::max(sep.size(), 1);
			startIndex = endIndex + sep.size();
			isLast = returnList.size() + 1 == maxSplitCount;
		}
		if (isLast)
		{
			returnList.append(mid(startIndex));
			break;
		}
	}

	if (behavior == QString::SkipEmptyParts)
	{
		for (int i = returnList.size() - 1; i >= 0; i--)
		{
			if (returnList.at(i).isEmpty())
				returnList.removeAt(i);
		}
	}

	return returnList;
}

QUtfStringList QUtfString::split(const QUtfChar &sep, int maxSplitCount, QString::SplitBehavior behavior, Qt::CaseSensitivity cs) const
{
	if (maxSplitCount == 0)
		return QUtfStringList();
	else if (maxSplitCount == 1)
		return QUtfStringList(*this);

	QUtfStringList returnList;

	int startIndex = 0;
	while (true)
	{
		int endIndex = indexOf(sep, startIndex, cs);
		bool isLast = endIndex == -1;
		if (!isLast)
		{
			returnList.append(mid(startIndex, endIndex - startIndex));
			startIndex = endIndex + 1;
			isLast = returnList.size() + 1 == maxSplitCount;
		}
		if (isLast)
		{
			returnList.append(mid(startIndex));
			break;
		}
	}

	if (behavior == QString::SkipEmptyParts)
	{
		for (int i = returnList.size() - 1; i >= 0; i--)
		{
			if (returnList.at(i).isEmpty())
				returnList.removeAt(i);
		}
	}

	return returnList;
}

QUtfStringList QUtfString::split(const QUtfRegExp &rx, int maxSplitCount, QString::SplitBehavior behavior) const
{
	if (maxSplitCount == 0)
		return QUtfStringList();
	else if (maxSplitCount == 1)
		return QUtfStringList(*this);

	QUtfStringList returnList;
	QUtfRegExp frx = rx;

	int startIndex = 0, searchIndex = 0;
	while (true)
	{
		int endIndex = indexOf(frx, searchIndex);
		bool isLast = endIndex == -1;
		if (!isLast)
		{
			returnList.append(mid(startIndex, endIndex - startIndex));
			searchIndex = endIndex + std::max(frx.matchedLength(), 1);
			startIndex = endIndex + frx.matchedLength();
			isLast = returnList.size() + 1 == maxSplitCount;
		}
		if (isLast)
		{
			returnList.append(mid(startIndex));
			break;
		}
	}

	if (behavior == QString::SkipEmptyParts)
	{
		for (int i = returnList.size() - 1; i >= 0; i--)
		{
			if (returnList.at(i).isEmpty())
				returnList.removeAt(i);
		}
	}

	return returnList;
}

void QUtfString::squeeze()
{
	s.squeeze();
}

bool QUtfString::startsWith(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	return s.startsWith(str.s, cs);
}

bool QUtfString::startsWith(const QUtfChar &ch, Qt::CaseSensitivity cs) const
{
	if (ch.isBmpCharacter())
		return s.startsWith(ch, cs);
	else
		return s.startsWith(QUtfString(ch).s, cs);
}

QUtfString QUtfString::toCaseFolded() const
{
	return QUtfString(s.toCaseFolded(), pairCount);
}

double QUtfString::toDouble(bool *success) const
{
	return s.toDouble(success);
}

int QUtfString::toInt(bool *success, int base) const
{
	return s.toInt(success, base);
}

qlonglong QUtfString::toLongLong(bool *success, int base) const
{
	return s.toLongLong(success, base);
}

QUtfString QUtfString::toLower() const
{
	return QUtfString(s.toLower(), pairCount);
}

uint QUtfString::toUInt(bool *success, int base) const
{
	return s.toUInt(success, base);
}

qulonglong QUtfString::toULongLong(bool *success, int base) const
{
	return s.toULongLong(success, base);
}

QUtfString QUtfString::toUpper() const
{
	return QUtfString(s.toUpper(), pairCount);
}

QUtfString QUtfString::trimmed() const
{
	return QUtfString(s.trimmed(), pairCount > 0);
}

void QUtfString::truncate(int position)
{
	int qtPosition = mapToQtIndex(position);
	removeSurrogateRange(qtPosition, s.size() - qtPosition);
	s.truncate(qtPosition);
}

QUtfString::operator QString() const
{
	return s;
}

bool QUtfString::operator!=(const QUtfString &other) const
{
	return s != other.s;
}

bool QUtfString::operator!=(const char *other) const
{
	return s != other;
}

QUtfString &QUtfString::operator+=(const QUtfString &other)
{
	return append(other);
}

QUtfString &QUtfString::operator+=(const char *str)
{
	return append(str);
}

QUtfString &QUtfString::operator+=(char ch)
{
	// There is no append() overload for char, so use QString functionality instead here.
	s += ch;
	return *this;
}

QUtfString &QUtfString::operator+=(QUtfChar ch)
{
	return append(ch);
}

bool QUtfString::operator<(const QUtfString &other) const
{
	return s < other.s;
}

bool QUtfString::operator<(const char *other) const
{
	return s < other;
}

bool QUtfString::operator<=(const QUtfString &other) const
{
	return s <= other.s;
}

bool QUtfString::operator<=(const char *other) const
{
	return s <= other;
}

QUtfString &QUtfString::operator=(const QUtfString &other)
{
	s = other.s;
	pairCount = other.pairCount;
	lastCharIndex = other.lastCharIndex;
	lastQtIndex = other.lastQtIndex;
	return *this;
}

QUtfString &QUtfString::operator=(const char *str)
{
	s = str;
	countSurrogates();
	return *this;
}

QUtfString &QUtfString::operator=(char ch)
{
	s = ch;
	pairCount = 0;
	lastCharIndex = 0;
	lastQtIndex = 0;
	return *this;
}

QUtfString &QUtfString::operator=(QUtfChar ch)
{
	clear();
	append(ch);
	return *this;
}

bool QUtfString::operator==(const QUtfString &other) const
{
	return s == other.s;
}

bool QUtfString::operator==(const char *other) const
{
	return s == other;
}

bool QUtfString::operator>(const QUtfString &other) const
{
	return s > other.s;
}

bool QUtfString::operator>(const char *other) const
{
	return s > other;
}

bool QUtfString::operator>=(const QUtfString &other) const
{
	return s >= other.s;
}

bool QUtfString::operator>=(const char *other) const
{
	return s >= other;
}

const QUtfChar QUtfString::operator[](int position) const
{
	return at(position);
}

bool operator!=(const char *s1, const QUtfString &s2)
{
	return s1 != (QString)s2;
}

const QUtfString operator+(const QUtfString &s1, const QUtfString &s2)
{
	return QUtfString(s1) += s2;
}

const QUtfString operator+(const QUtfString &s1, const char *s2)
{
	return QUtfString(s1) += s2;
}

const QUtfString operator+(const char *s1, const QUtfString &s2)
{
	return QUtfString(s1) += s2;
}

const QUtfString operator+(char ch, const QUtfString &str)
{
	return QUtfString(QUtfChar(ch)) += str;
}

const QUtfString operator+(const QUtfString &str, char ch)
{
	return QUtfString(str) += ch;
}

bool operator<(const char *s1, const QUtfString &s2)
{
	return s1 < (QString)s2;
}

bool operator<=(const char *s1, const QUtfString &s2)
{
	return s1 <= (QString)s2;
}

bool operator==(const char *s1, const QUtfString &s2)
{
	return s1 == (QString)s2;
}

bool operator>(const char *s1, const QUtfString &s2)
{
	return s1 > (QString)s2;
}

bool operator>=(const char *s1, const QUtfString &s2)
{
	return s1 >= (QString)s2;
}
