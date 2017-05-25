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

#ifndef TIMER
#define TIMER


#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QFile>


class Timer: public QObject
{
    Q_OBJECT

private:
    QTimer qtimer;
    QElapsedTimer elatimer;
    qint64 elapsed_summand;
    bool Logging;

public:
    qint64 interval;
    bool isActive;

signals:
    void update();
    void finished();

private slots:
    void slot_timeout()
    {

        if (elapsed() > interval) {
            //qDebug() <<"Timer "<< Logging;
            if (Logging) {
                QFile file("Logging.txt");
                if (file.open(QIODevice::Append)) {
                    QTextStream out(&file);
                    out << QString("%1, start view, elapsed = %2, elapsed_summand = %5\n")
                        .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(elapsed()).arg(elapsed_summand);
                    file.close();
                }
            }

            emit finished();
            restart();
        }
    }

public slots:
    void  stop()
    {
        isActive = false;
        qtimer.stop();
        elapsed_summand = 0;
    }

    void start()
    {
        isActive = true;
        qtimer.start();
        elatimer.start();
        elapsed_summand = 0;
    }


public:
    Timer(int updateinterval, int finishedinterval, bool _Logging = false): isActive(false)
    {
        qtimer.setInterval(updateinterval);
        connect(&qtimer, SIGNAL(timeout()), this, SLOT(slot_timeout()));
        connect(&qtimer, SIGNAL(timeout()), this, SIGNAL(update()));
        interval = finishedinterval;
        Logging = _Logging;
    }

    void Init(int updateinterval, int finishedinterval)
    {
        qtimer.setInterval(updateinterval);
        interval = finishedinterval;
    }


    void pause()
    {
        isActive = false;
        qtimer.stop();
        elapsed_summand += elatimer.elapsed();
    }

    void unpause()
    {
        isActive = true;
        qtimer.start();
        elatimer.restart();
    }

    void  restart(int updateinterval, int finishedinterval)
    {
        qtimer.stop();
        interval = finishedinterval;
        qtimer.setInterval(updateinterval);

        isActive = true;
        elapsed_summand = 0;
        elatimer.restart();
        qtimer.start();
    }

    void  restart()
    {
        isActive = true;
        elapsed_summand = 0;
        elatimer.restart();
    }



    qint64 elapsed()
    {
        return elapsed_summand + ((isActive) ? elatimer.elapsed() : 0);
    }

    qint64 remains()
    {
        return interval - elapsed();
    }

    double ratio()
    {
        return (double)elapsed() / interval;
    }

    QString remains_str(bool isDeciSec)
    {
        qint64 r = remains();

        if (isDeciSec) return QString("%1:%2.%3")
                                  .arg(r / 60000, 2, 10, QLatin1Char(' '))
                                  .arg(r / 1000 % 60, 2, 10, QLatin1Char('0'))
                                  .arg(r / 100 % 10);

        else return QString("%1:%2")
                        .arg((r + 500) / 60000, 2, 10, QLatin1Char(' '))
                        .arg((r + 500) / 1000 % 60, 2, 10, QLatin1Char('0'));
    }


};


#endif // TIMER
