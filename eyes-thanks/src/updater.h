//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef UPDATER_H
#define UPDATER_H

#include <QAction>
#include <QMessageBox>
#include <QtNetwork/QNetworkReply>

class UpdateAction : public QAction
{
    Q_OBJECT
public:
    explicit UpdateAction(const QIcon &icon, const QString &text, QObject *parent);

private:
    QNetworkAccessManager *manager{};
    QMessageBox *mbx {};

private slots:
    void showmessagebox();
    void update();
    void replyFinished(QNetworkReply *reply);
};

#endif // UPDATER_H
