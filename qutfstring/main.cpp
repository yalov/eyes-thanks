#include "QUtfString.h"

int main(int argc, char *argv[])
{
	QUtfString str;

	str.append(0x1307C);						// Construct a string which contains some Egypt hieroglyph
	Q_ASSERT(str.size() == 1);

	str += 'a';									// Append an 'a'

	Q_ASSERT(str[0] == 0x1307C);
	Q_ASSERT(str[1] == 'a');
	Q_ASSERT(str.size() == 2);
	Q_ASSERT(str.indexOf('a') == 1);

	str.setAt(0, 'a');							// Replace the hieroglyph with an 'a'
	Q_ASSERT(str[0] == 'a');
	Q_ASSERT(str[1] == 'a');
	Q_ASSERT(str.size() == 2);

	str += QUtfString(3, 0x1307C);				// Append three hieroglyphs again
	Q_ASSERT(str.indexOf(0x1307C, -3) == 2);
	Q_ASSERT(str.indexOf(0x1307C, -1) == 4);

	str.replace(0x1307C, "xyz");				// Replace each hieroglyph with the string "xyz"
	Q_ASSERT(str.size() == 11);

	str.setAt(9, 0x1307C);						// Replace a letter with a hieroglyph
	Q_ASSERT(str.size() == 11);

	str.setAt(6, 0x1307C);						// Replace another letter with a hieroglyph
	Q_ASSERT(str.size() == 11);

	Q_ASSERT(str.right(2) == QUtfString(0x1307C) + 'z');

	str.clear();
	Q_ASSERT(str.isEmpty());

	str += QUtfString("ab") + QUtfChar(0x1307C) + QUtfString("defgh");
	Q_ASSERT(str.size() == 8);
	Q_ASSERT(str[2] == 0x1307C);
	Q_ASSERT(str[4] == 'e');
	Q_ASSERT(str[3] == 'd');

	str.truncate(3);
	Q_ASSERT(str.size() == 3);
	Q_ASSERT(str.sizeCodeUnits() == 4);
	Q_ASSERT(str.repeated(2).mid(3, 2) == "ab");



	// Testing QUtfChar::canConvert
	Q_ASSERT(QUtfChar::canConvert(QChar('a')));
	Q_ASSERT(!QUtfChar::canConvert(QUtfChar(0x1307C)));

	return 0;
}
