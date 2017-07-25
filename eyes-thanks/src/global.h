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
    dark,
    white
};

enum ImageAspectMode {
    Auto,
    Outside,
    Inside
};


struct Setting {
    int running_counter;
    int pauseInterval;
    int pauseDuration;
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
    bool isSpectrum      ;
    bool isTiling        ;
    bool isStripes       ;
    bool isCircle ;
    bool isCircles;
    bool isNeo           ;
};

inline bool operator==(const Setting &lhs, const Setting &rhs)
{
    return
        lhs.running_counter               == rhs.running_counter               &&
        lhs.pauseInterval         == rhs.pauseInterval         &&
        lhs.pauseDuration       == rhs.pauseDuration       &&
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
        lhs.text                  == rhs.text                  &&
        lhs.isSpectrum            == rhs.isSpectrum            &&
        lhs.isTiling              == rhs.isTiling              &&
        lhs.isStripes             == rhs.isStripes             &&
        lhs.isCircle       == rhs.isCircle       &&
        lhs.isCircles      == rhs.isCircles      &&
        lhs.isNeo                 == rhs.isNeo
            ;
}

inline bool operator!=(const Setting &lhs, const Setting &rhs) { return !operator==(lhs, rhs);}


inline static void LogToFile(QString filepath, QString text, bool append = true)
{
    QFile file(filepath);
    if (file.open(append ? QIODevice::Append : QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << text << "\n";
        file.close();
    }
}
#endif // GLOBAL_H
