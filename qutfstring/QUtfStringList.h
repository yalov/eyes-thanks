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

#pragma once

#include "QUtfString.h"
#include <QList>


/**
 * The QUtfStringList class provides a list of strings.
 *
 * You can use an QUtfStringList everywhere you need a QStringList and vice versa, i.e. the conversion between those two classes is lossless. You should consider, however, that the conversion from
 * QStringList to QUtfStringList adds a small linear time overhead. On the other hand, conversion from QUtfStringList to QStringList is very fast.
 */
class QUtfStringList : public QList<QUtfString>
{
public: /*functions*/

	/**
	 * Constructs an empty string list.
	 */
	QUtfStringList();

	/**
	 * Constructs a string list that contains the given string @a str.
	 */
	QUtfStringList(const QUtfString &str);

	/**
	 * Constructs a copy of the @a other string list.
	 */
	QUtfStringList(const QUtfStringList &other);

	/**
	 * Constructs a copy of the @a other string list.
	 */
	QUtfStringList(const QStringList &other);

	/**
	 * Constructs a copy of the @a other list of strings.
	 */
	QUtfStringList(const QList<QUtfString> &other);

	/**
	 * Returns true if this list contains the string @a str, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool contains(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns a list of all strings within this list which contain the substring @a str, using the case sensitivity setting @a cs.
	 */
	QUtfStringList filter(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads filter(). Returns a list of all strings within this list which match the regular expression @a rx.
	 */
	QUtfStringList filter(const QUtfRegExp &rx) const;

	/**
	 * Returns the index position of the first occurrence of @a value in the list, searching forward from index position @a from, using the case sensitivity setting @a cs. Returns -1 if no
	 * item matched.
	 */
	int indexOf(const QUtfString &value, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads indexOf(). Returns the index position of the first exact match of @a rx in the list, searching forward from index position @a from. Returns -1 if no item matched.
	 */
	int indexOf(const QUtfRegExp &rx, int from = 0) const;

	/**
	 * This function overloads indexOf(). Returns the index position of the first exact match of @a rx in the list, searching forward from index position @a from. Returns -1 if no item matched.
	 * If an item matched, the @a rx regular expression will contain the matched objects.
	 */
	int indexOf(QUtfRegExp &rx, int from = 0) const;

	/**
	 * Joins all the strings contained within this list into a single string with each element separated by the given @a separator string.
	 */
	QUtfString join(const QUtfString &separator) const;

	/**
	 * Returns the index position of the last occurrence of @a value in the list, searching backward from index position @a from, using the case sensitivity setting @a cs. If @a from is -1 (the
	 * default), the search starts at the last item. Returns -1 if no item matched.
	 */
	int lastIndexOf(const QUtfString &value, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads lastIndexOf(). Returns the index position of the last exact match of @a rx in the list, searching backward from index position @a from. If @a from is -1 (the default),
	 * the search starts at the last item. Returns -1 if no item matched.
	 */
	int lastIndexOf(const QUtfRegExp &rx, int from = -1) const;

	/**
	 * This function overloads lastIndexOf(). Returns the index position of the last exact match of @a rx in the list, searching backward from index position @a from. If @a from is -1 (the default),
	 * the search starts at the last item. Returns -1 if no item matched. If an item matched, the @a rx regular expression will contain the matched objects.
	 */
	int lastIndexOf(QUtfRegExp &rx, int from = -1) const;

	/**
	 * Converts this QUtfStringList into a QStringList.
	 */
	operator QStringList() const;

	/**
	 * This function removes duplicate strings from this list, using case sensitive comparisions. The strings do not have to be sorted, and will retain their original order. Returns the number of
	 * removed strings.
	 *
	 */
	int removeDuplicates();

	/**
	 * Replaces the string @a before with the string @a after within every string of this string list, using the case sensitivity setting @a cs, and returns a reference to this string list.
	 */
	QUtfStringList &replaceInStrings(const QUtfString &before, const QUtfString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * This function overloads replaceInStrings(). Replaces the regular expression @rx with the string @a after within every string of this string list, and returns a reference to this string list.
	 */
	QUtfStringList &replaceInStrings(const QUtfRegExp &rx, const QUtfString &after);

	/**
	 * Returns a string list that is the concatenation of this string list with the @a other string list.
	 */
	QUtfStringList operator+(const QUtfStringList &other) const;

	/**
	 * Appends the given string @a str to this string list and returns a reference to this string list.
	 */
	QUtfStringList &operator<<(const QUtfString &str);

	/**
	 * Appends the given string list @a other to this string list and returns a reference to this string list.
	 */
	QUtfStringList &operator<<(const QUtfStringList &other);
};
