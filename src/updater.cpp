#include "updater.h"
#include "QMessageBox"
#include "QDebug"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkReply"

#include <QElapsedTimer>
#include <QRegularExpression>
#include <QDesktopServices>





UpdateAction::UpdateAction(const QIcon &icon, const QString &text, QObject *parent) :
    QAction(icon,text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(update()));
}

void UpdateAction::update()
{
    //qDebug() << "sslsupport " << QSslSocket::supportsSsl();

    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl(
        "https://bitbucket.org/flart/eyes-thanks/raw/master/eyes-thanks.pro"
                                     )));

}



void UpdateAction::replyFinished (QNetworkReply *reply)
{
    QList<int> new_version = {0,0,0};
    QList<int> curr_version = {0,0,0};

    QRegularExpression re("(\\d+)\\.(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(QString(APP_VERSION));

    if (match.hasMatch())
    {
        curr_version[0] = match.captured(1).toInt();
        curr_version[1] = match.captured(2).toInt();
        curr_version[2] = match.captured(3).toInt();
    }


   if(reply->error() != QNetworkReply::NoError)
    {
        //qDebug() << reply->errorString();
       QMessageBox::warning(this->parentWidget(), tr("Eyes' Thanks"),
          tr("There was an error connecting to %1.")
          .arg("<a href='https://bitbucket.org/flart/eyes-thanks'>https://bitbucket.org/flart/eyes-thanks</a>"),
          QMessageBox::Ok);
    }
    else
    {
       /*
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        */

        QString ProFileText = QString(reply->readAll());

        QRegularExpression re("VERSION.+(\\d+)\\.(\\d+)\\.(\\d+)");
        QRegularExpressionMatch match = re.match(ProFileText);

        if (match.hasMatch())
        {
            new_version[0] = match.captured(1).toInt();
            new_version[1] = match.captured(2).toInt();
            new_version[2] = match.captured(3).toInt();
        }

        if (curr_version == new_version)
            QMessageBox::information(this->parentWidget(), tr("Eyes' Thanks"),
                                        tr("You are already running the most recent version of <b>Eyes' Thanks</b>."), QMessageBox::Ok);

        else if ( (curr_version[0] < new_version[0])||
                  (curr_version[0] == new_version[0] && curr_version[1] < new_version[1]) ||
                  (curr_version[0] == new_version[0] && curr_version[1] == new_version[1] && curr_version[2] < new_version[2]) )
        {
            QMessageBox::information(this->parentWidget(), tr("Eyes' Thanks"),
               tr("A new version of <b>Eyes' Thanks</b> has been released! Version <b>%1</b> is available at %2.<br><br>"
               "You can download this version using the link:<br>%3")
                 .arg(QString::number(new_version[0]) + "." + QString::number(new_version[1]) + "." + QString::number(new_version[2]))
                 .arg("<a href='https://bitbucket.org/flart/eyes-thanks'>https://bitbucket.org/flart/eyes-thanks</a>")
                 .arg("<a href='https://bitbucket.org/flart/eyes-thanks/downloads/EyesThanks_v"+QString::number(new_version[0]) + "." + QString::number(new_version[1]) + "." + QString::number(new_version[2])+ ".7z'>"
                      "https://bitbucket.org/flart/eyes-thanks/downloads/EyesThanks_v"+QString::number(new_version[0]) + "." + QString::number(new_version[1]) + "." + QString::number(new_version[2])+ ".7z</a>"),
               QMessageBox::Ok);
        }


        else
        {
            QMessageBox::information(this->parentWidget(), tr("Eyes' Thanks"),
               tr("Fantastic! You are have <b>Eyes' Thanks %1</b>, but last available version is <b>%2</b>.<br><br>"
               "Please, upload new version to %3.")
                 .arg(QString::number(curr_version[0]) + "." + QString::number(curr_version[1]) + "." + QString::number(curr_version[2]))
                 .arg(QString::number(new_version[0]) + "." + QString::number(new_version[1]) + "." + QString::number(new_version[2]))
                 .arg("<a href='https://bitbucket.org/flart/eyes-thanks'>https://bitbucket.org/flart/eyes-thanks</a>"),
               QMessageBox::Ok);
        }

    }

    reply->deleteLater();
}
