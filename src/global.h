//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QFile>
#include <QTextStream>

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
