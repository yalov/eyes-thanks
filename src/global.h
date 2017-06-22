//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//                                                                                  //
//      This file is part of Eyes' Thanks.                                          //
//                                                                                  //
//      Eyes' Thanks is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License either                 //
//      version 3 of the License, or (at your option) any later version.            //
//                                                                                  //
//      Eyes' Thanks is distributed in the hope that it will be useful,             //
//      but WITHOUT ANY WARRANTY; without even the implied warranty of              //
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               //
//      GNU General Public License for more details.                                //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with Eyes' Thanks.  If not, see <http://www.gnu.org/licenses/>.       //
//----------------------------------------------------------------------------------//

#include <QFile>
#include <QString>
#include <QTextStream>


#ifndef GLOBAL_H
#define GLOBAL_H

enum IconsMode {
    light,
    dark
};

enum ImageAspectMode {
    Auto,
    Outside,
    Inside
};


struct Setting {
    int counter;
    int pauseInterval;
    int pauseContinuous;
    QString imagesPath;
    QString imagesPathAlternative;
    ImageAspectMode imageAspectMode;
    IconsMode iconsMode;
    bool isLogging;
    bool isText;
    bool isClock;
    bool isMessage30sec;
    bool isPrettyFont;
    bool isStartupLink;
    QString text;

    Setting()
    {}

    Setting(int _counter, int pauseinterval, int pausecontinuous,
            QString imagespath, QString imagespath_alt, ImageAspectMode imageaspectmode, IconsMode iconsmode,
            bool islogging, bool istext, bool isclock, bool ismessage30sec, bool isprettyfont, bool isstartuplink, QString _text
           )
    {
        counter = _counter;
        pauseInterval = pauseinterval;
        pauseContinuous = pausecontinuous;
        imagesPath = imagespath;
        imagesPathAlternative = imagespath_alt;
        imageAspectMode = imageaspectmode;
        iconsMode = iconsmode;
        isLogging = islogging;
        isText = istext;
        isClock = isclock;
        isMessage30sec = ismessage30sec;
        isPrettyFont = isprettyfont;
        isStartupLink = isstartuplink;
        text = _text;
    }

};

inline bool operator==(const Setting &lhs, const Setting &rhs)
{
    return
        lhs.counter               == rhs.counter               &&
        lhs.pauseInterval         == rhs.pauseInterval         &&
        lhs.pauseContinuous       == rhs.pauseContinuous       &&
        lhs.imagesPath            == rhs.imagesPath            &&
        lhs.imagesPathAlternative == rhs.imagesPathAlternative &&
        lhs.imageAspectMode       == rhs.imageAspectMode       &&
        lhs.iconsMode             == rhs.iconsMode             &&
        lhs.isLogging             == rhs.isLogging             &&
        lhs.isText                == rhs.isText                &&
        lhs.isClock               == rhs.isClock               &&
        lhs.isMessage30sec        == rhs.isMessage30sec        &&
        lhs.isPrettyFont          == rhs.isPrettyFont          &&
        lhs.isStartupLink         == rhs.isStartupLink         &&
        lhs.text                  == rhs.text;
}

inline bool operator!=(const Setting &lhs, const Setting &rhs) { return !operator==(lhs, rhs);}


inline static void LogToFile(QString filepath, QString text, bool append = true)
{
    QFile file(filepath);
    if (file.open(append ? QIODevice::Append : QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << text << "\n";
        file.close();
    }
}
#endif // GLOBAL_H
