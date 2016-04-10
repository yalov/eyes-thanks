#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <qaction.h>

#include <QtNetwork/QNetworkReply>

class UpdateAction : public QAction
{
    Q_OBJECT
public:
    explicit UpdateAction(const QIcon &icon, const QString &text, QObject *parent);

private:
    QNetworkAccessManager *manager;

private slots:
    void update();
    void replyFinished(QNetworkReply *reply);
};

#endif // UPDATER_H
