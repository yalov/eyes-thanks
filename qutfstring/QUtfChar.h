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

#include <QChar>
class QUtfString;


/**
 * The QUtfChar class provides a 32-bit Unicode character which supports all characters defined by the Unicode standard.
 *
 * This class is introduced since QChar can only store Unicode characters of the so-called Basic Multilingual Plane (i.e., Unicode characters of up to U+FFFF). QUtfChar is
 * compatible with QChar in the sense that conversions between those two always happen implicitly and in constant time. Note, however, that such a conversion
 * isn't always possible and may silently fail. The conversion is possible if, and only if, the following conditions are met:
 *
 * --> Converting a QUtfChar to a QChar is possible if the QUtfChar represents a Basic Multilingual Plane character (i.e., Unicode characters of up to U+FFFF).
 * --> Converting a QChar to a QUtfChar is possible if the QChar represents a valid Unicode character (note, especially, that surrogate code points aren't valid Unicode
 *     characters - if you need to convert surrogate code points, you have to convert from QString to QUtfString instead).
 *
 * If the conversion isn't possible, the result of the conversion will be the Unicode U+FFFD replacement character instead. You can use canConvert() to check whether the
 * conversion is possible beforehand.
 */
class QUtfChar
{
private: /*variables*/

	/**
	 * The Unicode code point of this character, stored as 32 bit integer (thus covering the entire Unicode code space from U+0000 to U+10FFFF).
	 */
	uint ucs4;

private: /*functions*/

	/**
	 * Validates the current character code point, and sets the character code to U+FFFD if the code point is invalid.
	 */
	void validate();

public: /*functions*/

	/**
	 * Constructs a null character ('\0').
	 */
	QUtfChar();

	/**
	 * Constructs a character corresponding to @a ch.
	 */
	QUtfChar(char ch);

	/**
	 * Constructs a character corresponding to @a ch.
	 */
	QUtfChar(uchar ch);

	/**
	 * Constructs a character corresponding to @a ch.
	 */
	QUtfChar(QChar ch);

	/**
	 * Constructs a character which has code point @a code.
	 */
	QUtfChar(int code);

	/**
	 * Constructs a character which has code point @a code.
	 */
	QUtfChar(uint code);

	/**
	 * Returns whether the given QChar @a ch can be converted into a QUtfChar.
	 */
	static bool canConvert(QChar ch);

	/**
	 * Returns whether the given QUtfChar @a ch can be converted into a QChar.
	 */
	static bool canConvert(QUtfChar ch);

	/**
	 * Returns the character's category.
	 */
	QChar::Category category() const;

	/**
	 * Returns the Unicode code point of this character.
	 */
	uint codePoint() const;

	/**
	 * Returns the combining class for this character, as defined within the Unicode standard. This is mainly useful as a positioning hint for marks attached to a base
	 * character. A text rendering engine may use this information to correctly position non-spacing marks around base characters.
	 */
	uchar combiningClass() const;

	/**
	 * Decomposes this character into its parts. Returns an empty string if no decomposition exists.
	 */
	QUtfString decomposition() const;

	/**
	 * Returns the tag defining the composition of this character. Returns UtfCharDecomposition_None if no decomposition exists.
	 */
	QChar::Decomposition decompositionTag() const;

	/**
	 * Returns the numeric value of this character, or -1 if this character is not a digit.
	 */
	int digitValue() const;

	/**
	 * Returns the character's direction.
	 */
	QChar::Direction direction() const;

	/**
	 * Returns true if this character should be reversed if the text direction is reversed; otherwise returns false.
	 */
	bool hasMirrored() const;

	/**
	 * Returns true if this character is a Basic Multilingual Plane (BMP) character in Unicode; otherwise returns false.
	 */
	bool isBmpCharacter() const;

	/**
	 * Returns true if this character is defined as a digit character in Unicode; otherwise returns false.
	 */
	bool isDigit() const;

	/**
	 * Returns true if this character is defined as a letter character in Unicode; otherwise returns false.
	 */
	bool isLetter() const;

	/**
	 * Returns true if this character is defined as either letter or number character in Unicode; otherwise returns false.
	 */
	bool isLetterOrNumber() const;

	/**
	 * Returns true if this character is a lowercase letter; otherwise returns false.
	 */
	bool isLower() const;

	/**
	 * Returns true if this character is defined as a mark character in Unicode; otherwise returns false.
	 */
	bool isMark() const;

	/**
	 * Returns true if this character is the null character (i.e. has the Unicode code point U+0000); otherwise returns false.
	 */
	bool isNull() const;

	/**
	 * Returns true if this character is defined as a number character in Unicode; otherwise returns false.
	 */
	bool isNumber() const;

	/**
	 * Returns true if this character is defined as a printable character in Unicode; otherwise returns false.
	 */
	bool isPrint() const;

	/**
	 * Returns true if this character is defined as a punctuation mark character in Unicode; otherwise returns false.
	 */
	bool isPunct() const;

	/**
	 * Returns true if this character is defined as a separator character in Unicode; otherwise returns false.
	 */
	bool isSpace() const;

	/**
	 * Returns true if this character is defined as a symbol character in Unicode; otherwise returns false.
	 */
	bool isSymbol() const;

	/**
	 * Returns true if this character is a titlecase letter; otherwise returns false.
	 */
	bool isTitleCase() const;

	/**
	 * Returns true if this character is an uppercase letter; otherwise returns false.
	 */
	bool isUpper() const;

	/**
	 * Returns information about the joining properties of this character (needed for certain languages such as Arabic).
	 */
	QChar::Joining joining() const;

	/**
	 * Returns the mirrored character if this character has a mirrored character; otherwise returns this character itself.
	 */
	QUtfChar mirroredChar() const;

	/**
	 * Converts this QUtfChar into a QChar. Returns the Unicode U+FFFD replacement character if this QUtfChar can't be converted into a QChar.
	 */
	operator QChar() const;

	/**
	 * Returns the case folded equivalent of this character. For most Unicode characters this is the same as toLowerCase().
	 */
	QUtfChar toCaseFolded() const;

	/**
	 * Returns the lowercase equivalent of this character.
	 */
	QUtfChar toLower() const;

	/**
	 * Returns the title case equivalent of this character.
	 */
	QUtfChar toTitleCase() const;

	/**
	 * Returns the uppercase equivalent of this character.
	 */
	QUtfChar toUpper() const;

	/**
	 * Returns the Unicode version that introduced this character.
	 */
	QChar::UnicodeVersion unicodeVersion() const;
};



/*******************Related non-members*******************/

/**
 * Returns true if @a c1 and @a c2 are not the same character; otherwise returns false.
 */
bool operator!=(QUtfChar c1, QUtfChar c2);

/**
 * Returns true if the numeric Unicode value of @a c1 is less than that of @a c2; otherwise returns false.
 */
bool operator<(QUtfChar c1, QUtfChar c2);

/**
 * Returns true if the numeric Unicode value of @a c1 is less than or equal to that of @a c2; otherwise returns false.
 */
bool operator<=(QUtfChar c1, QUtfChar c2);

/**
 * Returns true if the numeric Unicode value of @a c1 is equal to that of @a c2; otherwise returns false.
 */
bool operator==(QUtfChar c1, QUtfChar c2);

/**
 * Returns true if the numeric Unicode value of @a c1 is greater than that of @a c2; otherwise returns false.
 */
bool operator>(QUtfChar c1, QUtfChar c2);

/**
 * Returns true if the numeric Unicode value of @a c1 is greater than or equal to that of @a c2; otherwise returns false.
 */
bool operator>=(QUtfChar c1, QUtfChar c2);
