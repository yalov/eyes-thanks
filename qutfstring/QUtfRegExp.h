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

#include <QRegExp>
class QUtfString;
class QUtfStringList;


/**
 * The QUtfRegExp class provides pattern matching using regular expressions.
 */
class QUtfRegExp
{
private: /*types*/

	/**
	 * The QUtfString class needs to be a friend class in order to enable access to the index mapping functions.
	 */
	friend class QUtfString;

private: /*variables*/

	/**
	 * The internally used QRegExp of this QUtfRegExp implementation.
	 */
	QRegExp regex;

public: /*functions*/

	/**
	 * Constructs an empty regular expression.
	 */
	QUtfRegExp();

	/**
	 * Constructs a regular expression for the given @a pattern string, using the case sensitivity setting @a cs. The pattern must be given using wildcard notation if @a syntax is
	 * UtfRegExpPatternSyntax_Wildcard; the default is UtfRegExpPatternSyntax_RegExp. Matching is greedy (maximal), but can be changed by calling setMinimal().
	 */
	QUtfRegExp(const QUtfString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive, QRegExp::PatternSyntax syntax = QRegExp::RegExp);

	/**
	 * Constructs a regular expression as a copy of @a rx.
	 */
	QUtfRegExp(const QUtfRegExp &rx);

	/**
	 * Constructs a regular expression as a copy of @a rx.
	 */
	QUtfRegExp(const QRegExp &rx);

	/**
	 * Returns the text captured by the @a nth subexpression. The entire match has index 0 and the parenthesized subexpressions have indexes starting from 1 (excluding non-capturing parentheses).
	 */
	QUtfString cap(int nth = 0) const;

	/**
	 * Returns the number of captures contained in this regular expression.
	 */
	int captureCount() const;

	/**
	 * Returns a list of the captured text strings. The first string in the list is the entire matched string. Each subsequent list element contains a string that matched a (capturing) subexpression
	 * of the regular expression.
	 */
	QUtfStringList capturedTexts() const;

	/**
	 * Returns the case sensitivity of the string pattern of this regular expression.
	 */
	Qt::CaseSensitivity caseSensitivity() const;

	/**
	 * Returns a text string that explains why a regular expression pattern is invalid the case being; otherwise returns "no error occurred".
	 */
	QUtfString errorString() const;

	/**
	 * Returns the string @a str with every regular expression special character escaped with a backslash. This function is useful to dynamically construct regular expression patterns.
	 */
	static QUtfString escape(const QUtfString &str);

	/**
	 * Returns true if @a str is matched exactly by this regular expression; otherwise returns false. You can determine how much of the string was matched by calling matchedLength().
	 */
	bool exactMatch(const QUtfString &str) const;

	/**
	 * Attempts to find a match in @a str from position @a offset (0 by default). If @a offset is -1, the search starts at the last character; if -2, at the next to last character; etc. Returns the
	 * position of the first match, or -1 if there was no match. The @a caretMode parameter can be used to instruct whether ^ should match at index 0 or at @a offset.
	 */
	int indexIn(const QUtfString &str, int offset = 0, QRegExp::CaretMode caretMode = QRegExp::CaretAtZero) const;

	/**
	 * Returns true if the pattern string is empty; otherwise returns false.
	 */
	bool isEmpty() const;

	/**
	 * Returns true if minimal (non-greedy) matching is enabled; otherwise returns false.
	 */
	bool isMinimal() const;

	/**
	 * Returns true if the regular expression is valid; otherwise returns false. An invalid regular expression never matches. The pattern [a-z is an example of an invalid pattern, since it lacks a
	 * closing square bracket. Note that the validity of a regular expression may also depend on the setting of the wildcard flag, for example *.html is a valid wildcard regular expression but an
	 * invalid full regular expression.
	 */
	bool isValid() const;

	/**
	 * Attempts to find a match backwards in @a str from position @a offset. If @a offset is -1 (the default), the search starts at the last character; if -2, at the next to last character; etc.
	 * Returns the position of the first match, or -1 if there was no match. The @a caretMode parameter can be used to instruct whether ^ should match at index 0 or at @a offset. Note that
	 * searching backwards is much slower than searching forwards.
	 */
	int lastIndexIn(const QUtfString &str, int offset = -1, QRegExp::CaretMode caretMode = QRegExp::CaretAtZero) const;

	/**
	 * Returns the length of the last matched string, or -1 if there was no match.
	 */
	int matchedLength() const;		// TODO Doesn't handle surrogate pairs yet.

	/**
	 * Returns the pattern string of this regular expression. The pattern has either regular expression syntax or wildcard syntax, depending on patternSyntax().
	 */
	QUtfString pattern() const;

	/**
	 * Returns the syntax used by this regular expression. The default is QRegExp::RegExp.
	 */
	QRegExp::PatternSyntax patternSyntax() const;

	/**
	 * Returns the position of the @a nth captured text in the searched string. If @a nth is 0 (the default), pos() returns the position of the whole match. For zero-length matches, pos() always
	 * returns -1. For example, if cap(4) would return an empty string, pos(4) returns -1.
	 */
	int pos(int nth = 0) const;		// TODO Doesn't handle surrogate pairs yet.

	/**
	 * Sets the case sensitivity to @a cs.
	 */
	void setCaseSensitivity(Qt::CaseSensitivity cs);

	/**
	 * Enables or disables minimal matching. If minimal is false, matching is maximal (i.e., greedy).
	 */
	void setMinimal(bool minimal);

	/**
	 * Sets the pattern string to @a pattern. The case sensitivity, wildcard, and minimal matching options are not changed.
	 */
	void setPattern(const QUtfString &pattern);

	/**
	 * Sets the syntax mode for the regular expression. The default is QRegExp::RegExp.
	 */
	void setPatternSyntax(QRegExp::PatternSyntax syntax);

	/**
	 * Converts this QUtfRegExp into a QRegExp.
	 */
	operator QRegExp() const;

	/**
	 * Returns true if this regular expression is not equal to @a rx; otherwise returns false.
	 */
	bool operator!=(const QUtfRegExp &rx) const;

	/**
	 * Assigns the regular expression @a rx to this regular expression, and returns a reference to this regular expression. The case sensitivity, wildcard, and minimal matching options are also
	 * copied.
	 */
	QUtfRegExp &operator=(const QUtfRegExp &rx);

	/**
	 * Returns true if this regular expression is equal to @a rx; otherwise returns false. Two regular expressions are considered equal if they have the same pattern strings and the same settings
	 * for case sensitivity, wildcard and minimal matching.
	 */
	bool operator==(const QUtfRegExp &rx) const;
};
