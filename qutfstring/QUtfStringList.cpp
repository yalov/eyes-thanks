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

#include "QUtfStringList.h"
#include "QUtfRegExp.h"
#include <QSet>
#include <QStringList>


QUtfStringList::QUtfStringList()
{ }

QUtfStringList::QUtfStringList(const QUtfString &str)
{
	append(str);
}

QUtfStringList::QUtfStringList(const QUtfStringList &other) :
	QList<QUtfString>(other)
{ }

QUtfStringList::QUtfStringList(const QStringList &other)
{
	reserve(other.size());
	for (int i = 0; i < other.size(); i++)
		append(other[i]);
}

QUtfStringList::QUtfStringList(const QList<QUtfString> &other) :
	QList<QUtfString>(other)
{ }

bool QUtfStringList::contains(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	for (int i = 0; i < size(); i++)
	{
		if (str.equals((*this)[i], cs))
			return true;
	}
	return false;
}

QUtfStringList QUtfStringList::filter(const QUtfString &str, Qt::CaseSensitivity cs) const
{
	QUtfStringList returnList;
	for (int i = 0; i < size(); i++)
	{
		if ((*this)[i].contains(str, cs))
			returnList.append((*this)[i]);
	}
	return returnList;
}

QUtfStringList QUtfStringList::filter(const QUtfRegExp &rx) const
{
	QUtfStringList returnList;
	for (int i = 0; i < size(); i++)
	{
		if ((*this)[i].contains(rx))
			returnList.append((*this)[i]);
	}
	return returnList;
}

int QUtfStringList::indexOf(const QUtfString &value, int from, Qt::CaseSensitivity cs) const
{
	if (from < 0)
		from = std::max(from + size(), 0);

	for (int i = from; i < size(); i++)
	{
		if (value.equals((*this)[i], cs))
			return i;
	}
	return -1;
}

int QUtfStringList::indexOf(const QUtfRegExp &rx, int from) const
{
	QUtfRegExp regExpCopy(rx);

	if (from < 0)
		from = std::max(from + size(), 0);

	for (int i = from; i < size(); i++)
	{
		if (regExpCopy.exactMatch((*this)[i]))
			return i;
	}
	return -1;
}

int QUtfStringList::indexOf(QUtfRegExp &rx, int from) const
{
	if (from < 0)
		from = std::max(from + size(), 0);

	for (int i = from; i < size(); i++)
	{
		if (rx.exactMatch((*this)[i]))
			return i;
	}
	return -1;
}

QUtfString QUtfStringList::join(const QUtfString &separator) const
{
	int codeUnits = 0;
	for (int i = 0; i < size(); i++)
		codeUnits += (*this)[i].sizeCodeUnits();

	if (size() > 1)
		codeUnits += separator.sizeCodeUnits() * (size() - 1);

	QUtfString returnString;
	if (codeUnits == 0)
		return returnString;

	returnString.reserve(codeUnits);
	for (int i = 0; i < size(); i++)
	{
		if (i > 0)
			returnString += separator;
		returnString += (*this)[i];
	}
	return returnString;
}

int QUtfStringList::lastIndexOf(const QUtfString &value, int from, Qt::CaseSensitivity cs) const
{
	if (from < 0)
		from += size();
	else if (from >= size())
		from = size() - 1;

	for (int i = from; i >= 0; i--)
	{
		if (value.equals((*this)[i], cs))
			return i;
	}
	return -1;
}

int QUtfStringList::lastIndexOf(const QUtfRegExp &rx, int from) const
{
	QUtfRegExp regExpCopy(rx);

	if (from < 0)
		from += size();
	else if (from >= size())
		from = size() - 1;

	for (int i = from; i >= 0; i--)
	{
		if (regExpCopy.exactMatch((*this)[i]))
			return i;
	}
	return -1;
}

int QUtfStringList::lastIndexOf(QUtfRegExp &rx, int from) const
{
	if (from < 0)
		from += size();
	else if (from >= size())
		from = size() - 1;

	for (int i = from; i >= 0; i--)
	{
		if (rx.exactMatch((*this)[i]))
			return i;
	}
	return -1;
}

QUtfStringList::operator QStringList() const
{
	QStringList returnList;
	returnList.reserve(size());
	for (int i = 0; i < size(); i++)
		returnList.append((*this)[i]);
	return returnList;
}

int QUtfStringList::removeDuplicates()
{
	int n = size();
	int j = 0;

	QSet<QUtfString> seen;
	seen.reserve(n);

	for (int i = 0; i < n; i++)
	{
		const QUtfString &str = (*this)[i];
		if (seen.contains(str))
			continue;
		seen.insert(str);
		if (j != i)
			(*this)[j] = str;
		j++;
	}

	if (n != j)
		erase(begin() + j, begin() + n);

	return n - j;
}

QUtfStringList &QUtfStringList::replaceInStrings(const QUtfString &before, const QUtfString &after, Qt::CaseSensitivity cs)
{
	for (int i = 0; i < size(); i++)
		(*this)[i].replace(before, after, cs);
	return *this;
}

QUtfStringList &QUtfStringList::replaceInStrings(const QUtfRegExp &rx, const QUtfString &after)
{
	for (int i = 0; i < size(); i++)
		(*this)[i].replace(rx, after);
	return *this;
}

QUtfStringList QUtfStringList::operator+(const QUtfStringList &other) const
{
	QUtfStringList returnList = *this;
	returnList += other;
	return returnList;
}

QUtfStringList &QUtfStringList::operator<<(const QUtfString &str)
{
	append(str);
	return *this;
}

QUtfStringList &QUtfStringList::operator<<(const QUtfStringList &other)
{
	append(other);
	return *this;
}
