//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef TIMER
#define TIMER
#include <QTimer>
#include <QElapsedTimer>


class Timer: public QObject {
    Q_OBJECT

public:
    Timer(int updateinterval, int finishedinterval, bool logging = false);
    void init(int updateinterval, int finishedinterval);
    void pause();
    void unpause();
    void  restart(int updateinterval, int finishedinterval);
    void  restart();
    qint64 elapsed();
    qint64 remains();
    double ratio();
    QString remains_str(bool isDeciSec);
    qint64 interval;
    bool isActive;

public slots:
    void  stop();
    void start();

signals:
    void update();
    void finished();

private slots:
    void qtimer_timeout();

private:
    QTimer qtimer;
    QElapsedTimer elatimer;
    qint64 elapsed_summand;
    bool Logging;
};


#endif // TIMER
