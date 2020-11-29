//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>

enum IconsMode { light, dark, white };

enum ImageAspectMode { Auto, Outside, Inside };

struct Setting
{
    int running_counter{};
    int pauseInterval{};
    int pauseDuration{};
    QString imagesPath{};
    QString imagesPathAlternative{};
    ImageAspectMode imageAspectMode{};
    IconsMode iconsMode{};
    bool isLogging{};
    bool isText{};
    bool isClock{};
    bool isMessage30sec{};
    bool isSkipWhenFullScreen{};
    bool isPrettyFont{};
    bool isStartupLink{};
    QString text{};
    bool isSpectrum{};
    bool isTiling{};
    bool isStripes{};
    bool isCircle{};
    bool isCircles{};
    bool isNeo{};
};

inline bool operator==(const Setting &lhs, const Setting &rhs)
{
    return std::tie(lhs.running_counter, lhs.pauseInterval, lhs.pauseDuration, lhs.imagesPath,
                    lhs.imagesPathAlternative, lhs.imageAspectMode, lhs.iconsMode, lhs.isLogging, lhs.isText,
                    lhs.isClock, lhs.isMessage30sec, lhs.isSkipWhenFullScreen, lhs.isPrettyFont, lhs.isStartupLink,
                    lhs.text, lhs.isSpectrum, lhs.isTiling, lhs.isStripes, lhs.isCircle, lhs.isCircles, lhs.isNeo)
            == std::tie(rhs.running_counter, rhs.pauseInterval, rhs.pauseDuration, rhs.imagesPath,
                        rhs.imagesPathAlternative, rhs.imageAspectMode, rhs.iconsMode, rhs.isLogging, rhs.isText,
                        rhs.isClock, rhs.isMessage30sec, rhs.isSkipWhenFullScreen, rhs.isPrettyFont, rhs.isStartupLink,
                        rhs.text, rhs.isSpectrum, rhs.isTiling, rhs.isStripes, rhs.isCircle, rhs.isCircles, rhs.isNeo);
}

inline bool operator!=(const Setting &lhs, const Setting &rhs)
{
    return !operator==(lhs, rhs);
}

inline static void LogToFile(const QString& filepath, const QString& text, bool append = true)
{
    QFile file(filepath);
    if (file.open(append ? QIODevice::Append : QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << text << "\n";
        file.close();
    }
}

template<class T>
inline static T Random(T highest_bound)
{
    return QRandomGenerator::global()->bounded(highest_bound);
}
#endif // GLOBAL_H
