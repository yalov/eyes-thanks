//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include <QDebug>
#include <QDateTime>
#include <QFile>

#include "timer.h"

Timer::Timer(int updateinterval, int finishedinterval, bool logging): isActive(false)
{
    init(updateinterval, finishedinterval);
    Logging = logging;

    connect(&qtimer, SIGNAL(timeout()), this, SLOT(qtimer_timeout()));
    //connect(&qtimer, SIGNAL(timeout()), this, SIGNAL(update()));
}

void  Timer::stop()
{
    isActive = false;
    qtimer.stop();
    elapsed_summand = 0;
}

void Timer::start()
{
    isActive = true;
    qtimer.start();
    elatimer.start();
    elapsed_summand = 0;
}

void Timer::init(int updateinterval, int finishedinterval)
{
    qtimer.setInterval(updateinterval);
    interval = finishedinterval;
}

void  Timer::restart()
{
    start();
}

void  Timer::restart(int updateinterval, int finishedinterval)
{
    stop();
    init(updateinterval, finishedinterval);
    start();
}

void Timer::qtimer_timeout()
{
    if (elapsed() > interval) {
        if (Logging) {
            QFile file("LoggingTimer.txt");
            if (file.open(QIODevice::Append)) {
                QTextStream out(&file);
                out << QString("%1, start view, elapsed = %2, elapsed_summand = %3\n")
                    .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(elapsed()).arg(elapsed_summand);
                file.close();
            }
        }
        stop();
        emit finished();
    }
    else
        emit update();
}



void Timer::pause()
{
    isActive = false;
    qtimer.stop();
    elapsed_summand += elatimer.elapsed();
}

void Timer::unpause()
{
    isActive = true;
    qtimer.start();
    elatimer.start();
}



qint64 Timer::elapsed()
{
    return elapsed_summand + ((isActive) ? elatimer.elapsed() : 0);
}

qint64 Timer::remains()
{
    return interval - elapsed();
}

qreal Timer::ratio()
{
    return qreal(elapsed()) / interval;
}

QString Timer::remains_str()
{
    qint64 r = remains();

    return QString("%1:%2")
                    .arg((r + 500) / 60000, 2, 10, QLatin1Char(' '))
                    .arg((r + 500) / 1000 % 60, 2, 10, QLatin1Char('0'));
}

QString Timer::remains_str(bool isDeciSec)
{
    qint64 r = remains();

    if (isDeciSec) return QString("%1:%2.%3")
                              .arg(r / 60000, 2, 10, QLatin1Char(' '))
                              .arg(r / 1000 % 60, 2, 10, QLatin1Char('0'))
                              .arg(r / 100 % 10);

    else return remains_str();
}
