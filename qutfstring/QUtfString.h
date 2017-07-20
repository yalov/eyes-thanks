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

#include <QString>
#include "QUtfChar.h"
class QUtfRegExp;
class QUtfStringList;


/**
 * The QUtfString class provides a Unicode character string which supports easy handling of all characters defined by the Unicode standard.
 *
 * This class is introduced since QString can only easily handle Unicode characters of the so-called Basic Multilingual Plane (i.e., Unicode characters of up to U+FFFF).
 * QString encodes characters above U+FFFF as so-called surrogate pairs, using two QChars to represent them in memory (i.e., UTF-16 encoding). This must be taken into
 * account when processing a QString which contains such characters. As an example, QString::size() will return 2 if a string contains of a single Unicode character beyond
 * the Basic Multilingual Plane.
 *
 * QUtfString eliminates the need to care about surrogate pairs by using 32-bit characters and true character indexing throughout its entire API. QUtfString however doesn't
 * need any more memory than QString does and is also nearly as fast as QString is. Using QUtfString is generally neither a memory nor a performance problem in almost all
 * situations. For details, see the complexity discussion below.
 *
 * QUtfString is fully compatible with QString in the sense that conversions between those two always happen implicitly and are always lossless. QUtfString can therefore be
 * used everywhere a QString is required, and vice versa.
 *
 *
 *
 * The following section covers complexity details of QUtfString. This is only important for understanding the internals of QUtfString, but not for its usage.
 *
 * QUtfString internally uses a QString instance to represent the string data as UTF-16, but also keeps track of the surrogate pairs within the string. Thus QUtfString doesn't
 * need any more memory for holding the string data than QString does. Instead, Unicode character indices are translated into QString indices on demand. This approach generally
 * saves (much) memory at the cost of (not so much) performance - see below.
 *
 * A QUtfString instance can be copy-constructed and assigned in constant time thanks to QString's implicit sharing. When constructing a new QUtfString instance, a small linear
 * time overhead is added (that is, linear to the length of the string) compared to the construction of a QString. This is because the constructed string has to be scanned for
 * surrogate pairs. A once constructed QUtfString instance is as fast as a QString instance if it contains only Basic Multilingual Plane characters. Therefore, if no Unicode
 * characters beyond U+FFFF are used, QUtfString can be considered to have the same performance as QString (one could possibly benchmark the differences, but it won't affect
 * any real world applications). You therefore only pay for what you actually need when using QUtfString.
 *
 * When using characters beyond U+FFFF within a QUtfString instance, all functions which take or return a character index (like e.g. the access operator []) are usually, but not
 * necessarily, slower than their corresponding QString functions (linear time vs. constant time in the worst case). Accessing characters near the beginning, the middle or the
 * end of the string or near the last access position however usually happens much faster (amortized constant time). Iterating through the entire string by using a simple "for"
 * loop therefore still only takes linear time. Functions which don't take or return a character index (like e.g. contains()) are still as fast as their QString equivalents.
 *
 * A QUtfString instance can further always be converted into a QString in constant time thanks to implicit sharing.
 */
class QUtfString
{
private: /*types*/

	/**
	 * The QUtfRegExp class needs to be a friend in order to enable an easy implementation of e.g. the indexOf() functions.
	 */
	friend class QUtfRegExp;

private: /*variables*/

	/**
	 * The internally used QString of this QUtfString implementation.
	 */
	QString s;

	/**
	 * The pair count holds the number of surrogate pairs which are currently contained within this string. This counter must always be kept up to date. The functions countSurrogates(),
	 * addSurrogateRange() and removeSurrogateRange() should be used for that purpose.
	 */
	int pairCount;

	/**
	 * Stores the index of the last accessed character.
	 */
	int lastCharIndex;

	/**
	 * Stores the index of the last accessed code unit.
	 */
	int lastQtIndex;

private: /*functions*/

	/**
	 * Constructs an QUtfString from the QString @a other, and counts the surrogates if @a countSurrogates is true. @a countSurrogates must be set to true unless it is known that the source
	 * string doesn't contain any surrogates.
	 */
	QUtfString(const QString &other, bool countSurrogates);

	/**
	 * Constructs an QUtfString from the QString @a other which contains exactly @a surrogateCount surrogates.
	 */
	QUtfString(const QString &other, int surrogateCount);

	/**
	 * Determines the total number of surrogates within this string.
	 */
	void countSurrogates();

	/**
	 * Adds the surrogates within a specific range of this string to the surrogate count. Using this function can avoid re-scanning the entire string for surrogates.
	 * @param qtIndex The Qt string index pointing to the start of the range.
	 * @param qtCount The Qt string length of the range.
	 */
	void addSurrogateRange(int qtIndex, int qtCount);

	/**
	 * Removes the surrogates within a specific range of the string from the surrogate count. Using this function can avoid re-scanning the entire string for surrogates.
	 * @param qtIndex The Qt string index pointing to the start of the range.
	 * @param qtCount The Qt string length of the range.
	 */
	void removeSurrogateRange(int qtIndex, int qtCount);

	/**
	 * Maps the given @a qtIndex to a character index within this string and returns it.
	 */
	int mapToCharIndex(int qtIndex) const;

	/**
	 * Maps the given @a charIndex to a Qt index within this string and returns it.
	 */
	int mapToQtIndex(int charIndex) const;

public: /*functions*/

	/**
	 * Constructs an empty string.
	 */
	QUtfString();

	/**
	 * Constructs a string of size 1 containing the character @a ch.
	 */
	QUtfString(QUtfChar ch);

	/**
	 * Constructs a string of size @a size containing the character @a ch.
	 */
	QUtfString(int size, QUtfChar ch);

	/**
	 * Constructs a copy of @a other.
	 */
	QUtfString(const QUtfString &other);

	/**
	 * Constructs a copy of @a other.
	 */
	QUtfString(const QString &other);

	/**
	 * Constructs a string initialized with the string @a str.
	 */
	QUtfString(const char *str);

	/**
	 * Appends the string @a str to the end of this string.
	 */
	QUtfString &append(const QUtfString &str);

	/**
	 * Appends the string @a str to the end of this string.
	 */
	QUtfString &append(const char *str);

	/**
	 * Appends the character @a ch to the end of this string.
	 */
	QUtfString &append(QUtfChar ch);

	/**
	 * Returns a copy of this string with the lowest numbered place marker replaced by string @a a, i.e., %1, %2, ..., %99. @a minFieldWidth specifies the minimum amount of space that argument @a a
	 * shall occupy. If @a a requires less space than @a minFieldWidth, it is padded to @a minFieldWidth with character @a fillChar. A positive value produces right-aligned text. A negative value
	 * produces left-aligned text.
	 */
	QUtfString arg(const QUtfString &a, int minFieldWidth = 0, QUtfChar fillChar = QUtfChar(' ')) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7, const QUtfString &a8) const;

	/**
	 * This function overloads arg(). It replaces the place markers of all given strings in a single pass.
	 */
	QUtfString arg(const QUtfString &a1, const QUtfString &a2, const QUtfString &a3, const QUtfString &a4, const QUtfString &a5, const QUtfString &a6, const QUtfString &a7, const QUtfString &a8, const QUtfString &a9) const;

	/**
	 * This function overloads arg(). The @a a argument is expressed in base @a base, which is 10 by default and must be between 2 and 36. For bases other than 10, @a a is treated as an unsigned
	 * integer. @a minFieldWidth specifies the minimum amount of space that @a a is padded to and filled with the character @a fillChar. A positive value produces right-aligned text; a negative value
	 * produces left-aligned text. The '%' can be followed by an 'L', in which case the sequence is replaced with a localized representation of @a a. The conversion uses the default locale. The
	 * 'L' flag is ignored if @a base is not 10.
	 */
	QUtfString arg(int a, int minFieldWidth = 0, int base = 10, QUtfChar fillChar = QUtfChar('0')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(uint a, int minFieldWidth = 0, int base = 10, QUtfChar fillChar = QUtfChar('0')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(qlonglong a, int minFieldWidth = 0, int base = 10, QUtfChar fillChar = QUtfChar('0')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(qulonglong a, int minFieldWidth = 0, int base = 10, QUtfChar fillChar = QUtfChar('0')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(QUtfChar a, int minFieldWidth = 0, QUtfChar fillChar = QUtfChar(' ')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(char a, int minFieldWidth = 0, QUtfChar fillChar = QUtfChar(' ')) const;

	/**
	 * This function overloads arg().
	 */
	QUtfString arg(double a, int minFieldWidth = 0, char format = 'g', int precision = -1, QUtfChar fillChar = QUtfChar('0')) const;

	/**
	 * Returns the character at the given index @a position in the string. The @a position must be a valid index position in the string (i.e., 0 <= @a position < size()).
	 */
	const QUtfChar at(int position) const;

	/**
	 * Returns the maximum number of UTF-16 code units (not characters!) that can be stored in the string without forcing a reallocation.
	 */
	int capacity() const;

	/**
	 * Removes @a n characters from the end of the string. If @a n is greater than size(), the result is an empty string.
	 */
	void chop(int n);

	/**
	 * Clears the contents of the string and makes it empty.
	 */
	void clear();

	/**
	 * Lexically compares this string with the @a other string, using the case sensitivity setting @a cs, and returns an integer less than, equal to, or greater than zero if this string is less
	 * than, equal to, or greater than the @a other string. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what a human would
	 * expect. Consider sorting user-visible strings with localeAwareCompare().
	 */
	int compare(const QUtfString &other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Lexically compares the string @a s1 with the string @a s2, using the case sensitivity setting @a cs, and returns an integer less than, equal to, or greater than zero if the string @a s1
	 * is less than, equal to, or greater than the string @a s2. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what a human
	 * would expect. Consider sorting user-visible strings with localeAwareCompare().
	 */
	static int compare(const QUtfString &s1, const QUtfString &s2, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Returns true if this string contains an occurrence of the string @a str, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool contains(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns true if this string contains an occurrence of the character @a ch, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool contains(QUtfChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns true if the regular expression @a rx matches somewhere in this string; otherwise returns false.
	 */
	bool contains(const QUtfRegExp &rx) const;

	/**
	 * Returns true if the regular expression @a rx matches somewhere in this string; otherwise returns false. If there is a match, the @a rx regular expression will contain the matched captures.
	 */
	bool contains(QUtfRegExp &rx) const;

	/**
	 * Returns the number of (potentially overlapping) occurrences of the string @a str in this string, using the case sensitivity setting @a cs.
	 */
	int count(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns the number of occurrences of the character @a ch in this string, using the case sensitivity setting @a cs.
	 */
	int count(QUtfChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns the number of times the regular expression @a rx matches in the string. This function counts overlapping matches.
	 */
	int count(const QUtfRegExp &rx) const;

	/**
	 * Returns a pointer to the UTF-16 data stored in the string. The pointer can be used to access the Unicode code units that compose the string. For convenience, the data is '\0'-terminated.
	 * Note that the pointer remains valid only as long as the string is not modified.
	 */
	const quint16 *data() const;

	/**
	 * Returns true if this string ends with the string @a str, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool endsWith(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns true if this string ends with the character @a ch, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool endsWith(const QUtfChar &ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Lexically compares this string with the @a other string, using the case sensitivity setting @a cs, and returns true if this string is equal to the given @a other string, or false otherwise.
	 * The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what a human would expect. Consider comparing user-visible strings with
	 * localeAwareEquals().
	 */
	bool equals(const QUtfString &other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Lexically compares the string @a s1 with the string @a s2, using the case sensitivity setting @a cs, and returns true if string @a s1 is equal to string @a s2, or false otherwise. The
	 * comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what a human would expect. Consider comparing user-visible strings with
	 * localeAwareEquals().
	 */
	static bool equals(const QUtfString &s1, const QUtfString &s2, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Sets every character in this string to character @a ch. If @a size is different from the default of -1, the string is resized to @a size beforehand.
	 */
	QUtfString &fill(QUtfChar ch, int size = -1);

	/**
	 * Constructs a string that uses the first @a size UTF-16 code units in the array @a utf16. The data in @a utf16 is not copied. The caller must be able to guarantee that @a utf16 will not be
	 * deleted or modified as long as the constructed string (or an unmodified copy of it) exists. Any attempts to modify the constructed string or copies of it will cause it to create a deep copy
	 * of the data, ensuring that @a utf16 isn't modified.
	 */
	static QUtfString fromRawData(const quint16 *utf16, int size);

	/**
	 * This function overloads fromRawData(). The caller must provide the correct number of surrogate pairs within @a surrogateCount, enabling this function to execute faster than the other
	 * fromRawData() overload.
	 */
	static QUtfString fromRawData(const quint16 *utf16, int size, int surrogateCount);

	/**
	 * Returns the index position of the first occurrence of the string @a str in this string, searching forward from index position @a from, using the case sensitivity setting @a cs. Returns -1
	 * if @a str is not found.
	 */
	int indexOf(const QUtfString &str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads indexOf().
	 */
	int indexOf(QUtfChar ch, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads indexOf().
	 */
	int indexOf(const QUtfRegExp &rx, int from = 0) const;

	/**
	 * This function overloads indexOf(). If there is a match, the @a rx regular expression will contain the matched captures.
	 */
	int indexOf(QUtfRegExp &rx, int from = 0) const;

	/**
	 * Inserts the string @a str at the given index @a position and returns a reference to this string.
	 */
	QUtfString &insert(int position, const QUtfString &str);

	/**
	 * Inserts the character @a ch at the given index @a position and returns a reference to this string.
	 */
	QUtfString &insert(int position, QUtfChar ch);

	/**
	 * Returns true if this string has no characters; otherwise returns false.
	 */
	bool isEmpty() const;

	/**
	 * Returns true if this string is numeric; otherwise returns false. A string is considered numeric if it can be parsed as an integer using base @a base, which is 10 by default and must be
	 * between 2 and 36, or 0. If @a base is 0, the C language convention is used: If the string begins with "0x", base 16 is used; if the string begins with "0", base 8 is used; otherwise, base
	 * 10 is used.
	 */
	bool isNumeric(int base = 10) const;

	/**
	 * Returns the index position of the last occurrence of the string @a str in this string, searching backward from index position @a from, using the case sensitivity setting @a cs. If @a from
	 * is -1 (the default), the search starts at the last character; if @a from is -2, at the next to last character and so on. Returns -1 if @a str is not found.
	 */
	int lastIndexOf(const QUtfString &str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads lastIndexOf().
	 */
	int lastIndexOf(QUtfChar ch, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads lastIndexOf().
	 */
	int lastIndexOf(const QUtfRegExp &rx, int from = -1) const;

	/**
	 * This function overloads lastIndexOf(). If there is a match, the @a rx regular expression will contain the matched captures.
	 */
	int lastIndexOf(QUtfRegExp &rx, int from = -1) const;

	/**
	 * Returns a substring that contains the @a n leftmost characters of this string. The entire string is returned if @a n is greater than size().
	 */
	QUtfString left(int n) const;

	/**
	 * Returns a string of size @a width that contains this string padded by the @a fillChar character. If @a truncate is false and the size() of the string is more than @a width, then the
	 * returned string is a copy of this string. If @a truncate is true and the size() of this string is more than @a width, then any characters in the copy of this string after position
	 * @a width are removed, and the copy is returned.
	 */
	QUtfString leftJustified(int width, QUtfChar fillChar = QUtfChar(' '), bool truncate = false) const;

	/**
	 * Works in the same way as left(), but always takes just constant time since the returned string just references the data of this string. The caller guarantees that this string will not
	 * be deleted or modified as long as the returned string or any copies of it exist that have not been modified.
	 */
	QUtfString leftRef(int n) const;

	/**
	 * Compares this string with the string @a other and returns an integer less than, equal to, or greater than zero if this string is less than, equal to, or greater than the string @a other.
	 * The comparison is performed in a locale- and also platform-dependent manner. Use this function to present sorted strings to the user.
	 */
	int localeAwareCompare(const QUtfString &other) const;

	/**
	 * This function overloads localeAwareCompare(). Compares the string @a s1 with the string @a s2.
	 */
	static int localeAwareCompare(const QUtfString &s1, const QUtfString &s2);

	/**
	 * Compares this string with the string @a other, and returns true if this string is equal to the given @a other string, or false otherwise. The comparison is performed in a locale- and
	 * also platform-dependent manner. Use this function to compare strings which are presented to the user.
	 */
	bool localeAwareEquals(const QUtfString &other) const;

	/**
	 * This function overloads localeAwareEquals(). Compares the string @a s1 with the string @a s2.
	 */
	static bool localeAwareEquals(const QUtfString &s1, const QUtfString &s2);

	/**
	 * Returns a string that contains @a n characters of this string, starting at the specified @a position index. Returns an empty string if the @a position index exceeds the length of the string.
	 * If there are less than @a n characters available in the string starting at the given @a position, or if @a n is -1 (the default), the function returns all characters that are available from
	 * the specified @a position.
	 */
	QUtfString mid(int position, int n = -1) const;

	/**
	 * Works in the same way as mid(), but always takes just constant time since the returned string just references the data of this string. The caller guarantees that this string will not
	 * be deleted or modified as long as the returned string or any copies of it exist that have not been modified.
	 */
	QUtfString midRef(int position, int n = -1) const;

	/**
	 * Returns this string in the given Unicode normalization @a mode.
	 */
	QUtfString normalized(QString::NormalizationForm mode) const;

	/**
	 * This function overloads normalized(). Returns this string in the given Unicode normalization @a mode, according to the given @a version of the Unicode standard.
	 */
	QUtfString normalized(QString::NormalizationForm mode, QChar::UnicodeVersion version) const;

	/**
	 * Returns a string equivalent of the number @a n, expressed in base @a base, which is 10 by default and must be between 2 and 36. For bases other than 10, @a n is treated as an unsigned
	 * integer.
	 */
	static QUtfString number(int n, int base = 10);

	/**
	 * This function overloads number().
	 */
	static QUtfString number(uint n, int base = 10);

	/**
	 * This function overloads number().
	 */
	static QUtfString number(qlonglong n, int base = 10);

	/**
	 * This function overloads number().
	 */
	static QUtfString number(qulonglong n, int base = 10);

	/**
	 * This function overloads number(). Returns a string equivalent of the number @a n, formatted according to the specified @a format and @a precision. This function does not honor the user's
	 * locale settings.
	 */
	static QUtfString number(double n, char format = 'g', int precision = 6);

	/**
	 * Prepends the string @a str to the beginning of this string and returns a reference to this string.
	 */
	QUtfString &prepend(const QUtfString &str);

	/**
	 * Prepends the string @a str to the beginning of this string and returns a reference to this string.
	 */
	QUtfString &prepend(const char *str);

	/**
	 * Prepends the character @a ch to the beginning of this string and returns a reference to this string.
	 */
	QUtfString &prepend(QUtfChar ch);

	/**
	 * Removes @a n characters from this string, starting at the given @a position index, and returns a reference to this string.
	 */
	QUtfString &remove(int position, int n);

	/**
	 * Removes every occurrence of the character @a ch in this string, using the case sensitivity setting @a cs, and returns a reference to this string.
	 */
	QUtfString &remove(QUtfChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Removes every occurrence of the string @a str in this string, using the case sensitivity setting @a cs, and returns a reference to this string.
	 */
	QUtfString &remove(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Removes every occurrence of the regular expression @a rx in this string, and returns a reference to this string.
	 */
	QUtfString &remove(const QUtfRegExp &rx);

	/**
	 * Returns a copy of this string repeated the specified number of @a times. If @a times is less than 1, an empty string is returned.
	 */
	QUtfString repeated(int times) const;

	/**
	 * Replaces @a n characters beginning at index @a position with the string @a after and returns a reference to this string.
	 */
	QUtfString &replace(int position, int n, const QUtfString &after);

	/**
	 * Replaces @a n characters beginning at index @a position with the character @a after and returns a reference to this string.
	 */
	QUtfString &replace(int position, int n, QUtfChar after);

	/**
	 * Replaces every occurrence of the string @a before with the string @a after, using the case sensitivity setting @a cs, and returns a reference to this string.
	 */
	QUtfString &replace(const QUtfString &before, const QUtfString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Replaces every occurrence of the character @a ch with the string @a after, using the case sensitivity setting @a cs, and returns a reference to this string.
	 */
	QUtfString &replace(QUtfChar ch, const QUtfString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Replaces every occurrence of the character @a before with the character @a after, using the case sensitivity setting @a cs, and returns a reference to this string.
	 */
	QUtfString &replace(QUtfChar before, QUtfChar after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

	/**
	 * Replaces every occurrence of the regular expression @a rx with the string @a after, and returns a reference to this string.
	 */
	QUtfString &replace(const QUtfRegExp &rx, const QUtfString &after);

	/**
	 * Attempts to allocate memory for at least @a size UTF-16 code units (not characters!).
	 */
	void reserve(int size);

	/**
	 * Sets the size of the string to @a size characters. If @a size is greater than the current size, the string is extended to make it @a size characters long with the extra characters added to
	 * the end. The new characters are initialized with 0. If @a size is less than the current size, characters are removed from the end.
	 */
	void resize(int size);

	/**
	 * Returns a substring that contains the @a n rightmost characters of the string. The entire string is returned if @a n is greater than size().
	 */
	QUtfString right(int n) const;

	/**
	 * Returns a string of size @a width that contains the @a fillChar character followed by this string. If @a truncate is false and the size of this string is more than @a width, then the returned
	 * string is a copy of this string. If @a truncate is true and the size of this string is more than @a width, then the resulting string is truncated at position @a width.
	 */
	QUtfString rightJustified(int width, QUtfChar fillChar = QUtfChar(' '), bool truncate = false) const;

	/**
	 * Works in the same way as right(), but always takes just constant time since the returned string just references the data of this string. The caller guarantees that this string will not
	 * be deleted or modified as long as the returned string or any copies of it exist that have not been modified.
	 */
	QUtfString rightRef(int n) const;

	/**
	 * This function returns a section of this string. This string is treated as a sequence of fields separated by the character @a sep. The returned string consists of the fields from position
	 * @a start to position @a end, inclusive. If @a end is set to -1, all fields from position @a start to the end of the string are included. Fields are numbered 0, 1, 2, etc., counting
	 * from the left, and -1, -2, etc., counting from right to left. The @a flags argument can be used to affect some aspects of the function's behavior.
	 */
	QUtfString section(QUtfChar sep, int start, int end, QString::SectionFlags flags) const;

	/**
	 * This function overloads section().
	 */
	QUtfString section(const QUtfString &sep, int start, int end, QString::SectionFlags flags) const;

	/**
	 * This function overloads section().
	 */
	QUtfString section(const QUtfRegExp &reg, int start, int end, QString::SectionFlags flags) const;

	/**
	 * Sets the character at the specified @a position index to @a ch.
	 */
	void setAt(int position, QUtfChar ch);

	/**
	 * Resets this string to use the first @a size UTF-16 code units of the array @a utf16. The data in @a utf16 is not copied. The caller must be able to guarantee that @a utf16 will not be
	 * deleted or modified as long as this string (or an unmodified copy of it) exists. This function can be used instead of fromRawData() to re-use existings QUtfString objects to save memory
	 * re-allocations.
	 */
	QUtfString &setRawData(const quint16 *utf16, int size);

	/**
	 * This function overloads setRawData(). The caller must provide the correct number of surrogate pairs within @a utf16, enabling this function to execute faster.
	 */
	QUtfString &setRawData(const quint16 *utf16, int size, int surrogateCount);

	/**
	 * Returns a string that has whitespace removed from the start and the end, and that has each sequence of internal whitespace replaced with a single space.
	 */
	QUtfString simplified() const;

	/**
	 * Returns the number of characters in this string.
	 */
	int size() const;

	/**
	 * Returns the number of UTF-16 code units in this string.
	 */
	int sizeCodeUnits() const;

	/**
	 * Splits this string into substrings wherever @a sep occurs, and returns the list of those strings. If @a sep does not match anywhere in this string, a single-element list containing just
	 * this string is returned. @a maxSplitCount defines the maximum amount of substrings which are returned. @a cs specifies whether @a sep should be matched case sensitively or case insensitively.
	 * If @a behavior is QString::SkipEmptyParts, empty entries don't appear in the result. By default, empty entries are kept.
	 */
	QUtfStringList split(const QUtfString &sep, int maxSplitCount = -1, QString::SplitBehavior behavior = QString::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads split().
	 */
	QUtfStringList split(const QUtfChar &sep, int maxSplitCount = -1, QString::SplitBehavior behavior = QString::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * This function overloads split().
	 */
	QUtfStringList split(const QUtfRegExp &rx, int maxSplitCount = -1, QString::SplitBehavior behavior = QString::KeepEmptyParts) const;

	/**
	 * Releases any memory not required to store the character data.
	 */
	void squeeze();

	/**
	 * Returns true if this string starts with the string @a str, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool startsWith(const QUtfString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns true if this string starts with the character @a ch, using the case sensitivity setting @a cs; otherwise returns false.
	 */
	bool startsWith(const QUtfChar &ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

	/**
	 * Returns the case folded equivalent of this string. For most Unicode strings this is the same as toLower().
	 */
	QUtfString toCaseFolded() const;

	/**
	 * Returns the string converted to a double value.
	 */
	double toDouble(bool *success = 0) const;

	/**
	 * Returns this string converted to an integer using base @a base, which is 10 by default and must be between 2 and 36, or 0. If @a base
	 * is 0, the C language convention is used: If the string begins with "0x", base 16 is used; if the string begins with "0", base 8 is used; otherwise, base 10 is used.
	 */
	int toInt(bool *success = 0, int base = 10) const;

	/**
	 * See toInt(). Returns this string converted to a long long integer.
	 */
	qlonglong toLongLong(bool *success = 0, int base = 10) const;

	/**
	 * Returns a lowercase copy of this string.
	 */
	QUtfString toLower() const;

	/**
	 * See toInt(). Returns this string converted to an unsigned integer.
	 */
	uint toUInt(bool *success = 0, int base = 10) const;

	/**
	 * See toInt(). Returns this string converted to an unsigned long long integer.
	 */
	qulonglong toULongLong(bool *success = 0, int base = 10) const;

	/**
	 * Returns an uppercase copy of this string.
	 */
	QUtfString toUpper() const;

	/**
	 * Returns a copy of this string that has whitespace removed from the start and the end.
	 */
	QUtfString trimmed() const;

	/**
	 * Truncates this string at the given @a position index. If the specified @a position index is beyond the end of the string, nothing happens.
	 */
	void truncate(int position);

	/**
	 * Converts this QUtfString into a QString. This operation takes constant time.
	 */
	operator QString() const;

	/**
	 * Returns true if this string is not lexically equal to string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very
	 * fast, but is not what a human would expect. Consider comparing user-interface strings with localeAwareCompare().
	 */
	bool operator!=(const QUtfString &other) const;

	/**
	 * This function overloads operator!=().
	 */
	bool operator!=(const char *other) const;

	/**
	 * Appends the string @a other to the end of this string and returns a reference to this string.
	 */
	QUtfString &operator+=(const QUtfString &other);

	/**
	 * This function overloads operator+=().
	 */
	QUtfString &operator+=(const char *str);

	/**
	 * This function overloads operator+=().
	 */
	QUtfString &operator+=(char ch);

	/**
	 * This function overloads operator+=().
	 */
	QUtfString &operator+=(QUtfChar ch);

	/**
	 * Returns true if this string is lexically less than string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is
	 * very fast, but is not what a human would expect. Consider sorting user-interface strings with localeAwareCompare().
	 */
	bool operator<(const QUtfString &other) const;

	/**
	 * This function overloads operator<().
	 */
	bool operator<(const char *other) const;

	/**
	 * Returns true if this string is lexically less than or equal to string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters
	 * and is very fast, but is not what a human would expect. Consider sorting user-interface strings with localeAwareCompare().
	 */
	bool operator<=(const QUtfString &other) const;

	/**
	 * This function overloads operator<=().
	 */
	bool operator<=(const char *other) const;

	/**
	 * Assigns the string @a other to this string and returns a reference to this string.
	 */
	QUtfString &operator=(const QUtfString &other);

	/**
	 * This function overloads operator=().
	 */
	QUtfString &operator=(const char *str);

	/**
	 * This function overloads operator=().
	 */
	QUtfString &operator=(char ch);

	/**
	 * This function overloads operator=().
	 */
	QUtfString &operator=(QUtfChar ch);

	/**
	 * Returns true if this string is lexically equal to string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very
	 * fast, but is not what a human would expect. Consider comparing user-interface strings with localeAwareCompare().
	 */
	bool operator==(const QUtfString &other) const;

	/**
	 * This function overloads operator==().
	 */
	bool operator==(const char *other) const;

	/**
	 * Returns true if this string is lexically greater than string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is
	 * very fast, but is not what a human would expect. Consider sorting user-interface strings with localeAwareCompare().
	 */
	bool operator>(const QUtfString &other) const;

	/**
	 * This function overloads operator>().
	 */
	bool operator>(const char *other) const;

	/**
	 * Returns true if this string is lexically greater than or equal to string @a other; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters
	 * and is very fast, but is not what a human would expect. Consider sorting user-interface strings with localeAwareCompare().
	 */
	bool operator>=(const QUtfString &other) const;

	/**
	 * This function overloads operator>=().
	 */
	bool operator>=(const char *other) const;

	/**
	 * Returns the character at the specified @a position in the string. Note that this function is read-only; you have to call setAt() in order to modify the characters of the string.
	 */
	const QUtfChar operator[](int position) const;
};



/*******************Related non-members*******************/

/**
 * Returns true if @a s1 is not equal to @a s2; otherwise returns false.
 */
bool operator!=(const char *s1, const QUtfString &s2);

/**
 * Returns a string which is the result of concatenating @a s1 and @a s2.
 */
const QUtfString operator+(const QUtfString &s1, const QUtfString &s2);

/**
 * This function overloads operator+().
 */
const QUtfString operator+(const QUtfString &s1, const char *s2);

/**
 * This function overloads operator+().
 */
const QUtfString operator+(const char *s1, const QUtfString &s2);

/**
 * This function overloads operator+().
 */
const QUtfString operator+(char ch, const QUtfString &str);

/**
 * This function overloads operator+().
 */
const QUtfString operator+(const QUtfString &str, char ch);

/**
 * Returns true if @a s1 is lexically less than @a s2; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what
 * a human would expect. Consider sorting user-interface strings using the localeAwareCompare() function.
 */
bool operator<(const char *s1, const QUtfString &s2);

/**
 * Returns true if @a s1 is lexically less than or equal to @a s2; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but
 * is not what a human would expect. Consider sorting user-interface strings using the localeAwareCompare() function.
 */
bool operator<=(const char *s1, const QUtfString &s2);

/**
 * Returns true if @a s1 is lexically equal to @a s2; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not what
 * a human would expect. Consider sorting user-interface strings using the localeAwareCompare() function.
 */
bool operator==(const char *s1, const QUtfString &s2);

/**
 * Returns true if @a s1 is lexically greater than @a s2; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast, but is not
 * what a human would expect. Consider sorting user-interface strings using the localeAwareCompare() function.
 */
bool operator>(const char *s1, const QUtfString &s2);

/**
 * Returns true if @a s1 is lexically greater than or equal to @a s2; otherwise returns false. The comparison is based exclusively on the numeric Unicode values of the characters and is very fast,
 * but is not what a human would expect. Consider sorting user-interface strings using the localeAwareCompare() function.
 */
bool operator>=(const char *s1, const QUtfString &s2);
