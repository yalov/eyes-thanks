#include "charactersets.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>

CharacterSets::CharacterSets(const QString& filepath):sets(new QList<Set>())
{
   read(filepath);
//   for (int i=0;i < sets->size(); i++)
//       qDebug()<< i << sets->at(i).title;
}

void CharacterSets::read(const QString &filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(file.readAll());
        file.close();

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "set")
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
                    if (xml.name() == "comment") c.comment = xml.readElementText();
                    if (xml.name() == "title") c.title = xml.readElementText();
                    if (xml.name() == "characters") c.characters = xml.readElementText();
                }

               sets->push_back(c);
            }
        }
        if (xml.hasError()) {
            qDebug() << "do error handling";
        }
    }
}

int CharacterSets::size() {
    return sets->size();
}

QUtfString CharacterSets::get_title(int index){
    return sets->at(index).title;
}
QUtfString CharacterSets::get_characters(int index){
    return sets->at(index).characters;
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
            stream.writeEndElement();
        }
        stream.writeEndElement();
        stream.writeEndDocument();
        file.close();
    }
}
