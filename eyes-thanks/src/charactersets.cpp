#include "charactersets.h"
#include "global.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>


struct CharacterSets::Set {
    QString name{};
    QString comment{};
    QString title{};
    QString characters{};
    QList<QString> compositecharacters{};
    int index = 0;
};


CharacterSets::CharacterSets(const QString& filepath):sets(new QList<Set>())
{
   read(filepath);
}


CharacterSets::~CharacterSets()
{
    delete sets;
}


void CharacterSets::read(const QString &filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(file.readAll());
        file.close();

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == QLatin1String("set"))
            {
                Set c;
                for(auto &attr: xml.attributes()) {
                    if (attr.name().toString() == QLatin1String("index"))
                        c.index = attr.value().toInt();
                    if (attr.name().toString() == QLatin1String("name"))
                        c.name = attr.value().toString();
                }
                while (xml.readNextStartElement())
                {
                    if (xml.name() == QLatin1String("comment")) c.comment = xml.readElementText();
                    if (xml.name() == QLatin1String("title")) c.title = xml.readElementText();
                    if (xml.name() == QLatin1String("characters"))
                    {
                        c.characters = xml.readElementText();
                    }
                    if (xml.name() == QLatin1String("character"))
                    {
                        c.compositecharacters.append(xml.readElementText());
                    }
                }
               sets->append(c);
            }
        }
        if (xml.hasError()) {
            qDebug() << "xml has some error";
        }
    }
}


int CharacterSets::size() {
    return sets->size();
}


QString CharacterSets::get_title(int index){
    return sets->at(index).title;
}


QString CharacterSets::get_characters(int index){
    QString chars = "";
    chars +=  sets->at(index).characters;

    for(const auto & ch : sets->at(index).compositecharacters)
    {
        chars.append(ch);
    }

    return chars;
}

int CharacterSets::ChooseRandomCurrentSet()
{
    randomSetIndex = Random(size());
    //randomSetIndex = 5; // Gothic Alphabet (surrogate pair)

    currentCharacters = {};
    currentTitle = {};

    for(const auto & ucs4 : sets->at(randomSetIndex).characters.toUcs4())
    {

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        auto ch = QChar::fromUcs4(ucs4);
        QString s; s += ch;
#else
        auto s = QString::fromUcs4(&ucs4,1);
#endif

        currentCharacters.append(s);
    }

    for(const auto & ch : sets->at(randomSetIndex).compositecharacters)
    {
        currentCharacters.append(ch);
    }

    for(const auto & ucs4 : sets->at(randomSetIndex).title.toUcs4())
    {

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        auto ch = QChar::fromUcs4(ucs4);
        QString s; s += ch;
#else
        auto s = QString::fromUcs4(&ucs4,1);
#endif
        currentTitle.append(s);
    }

    return randomSetIndex;
}


void CharacterSets::write(const QString& filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {

        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("sets");
        for (int i=0; i< sets->size(); i++) {
            stream.writeStartElement("set");
            stream.writeAttribute("index", QString::number(i));
            stream.writeAttribute("name", sets->at(i).name);
            stream.writeTextElement("comment", sets->at(i).comment);
            stream.writeTextElement("title", sets->at(i).title);
            stream.writeTextElement("characters", sets->at(i).characters);
            for (const auto& ch: sets->at(i).compositecharacters)
                stream.writeTextElement("character", ch);
            stream.writeEndElement();
        }
        stream.writeEndElement();
        stream.writeEndDocument();
        file.close();
    }
}
