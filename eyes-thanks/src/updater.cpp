//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "updater.h"

#include <QRegularExpression>
#include <QCoreApplication>
#include <QGridLayout>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDir>

struct UpdateAction::Asset
{
    QString version{};
    QList<QString> links{};
    QString toString(){
        return version +", "+ links.join(", ");
    }
};

// parse json json_text and return Asset (last tag and assets links)
UpdateAction::Asset UpdateAction::getAsset(const QByteArray & json_text)
{
    Asset asset;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(json_text));

    if (jsonDocument.isNull() || jsonDocument.isEmpty() || !jsonDocument.isArray())
    {
        qDebug() << "JSON failed";
        return asset;
    }

    const QJsonArray &json = jsonDocument.array();

    for (auto json_value: json)
    {
        if (!json_value.isObject()) continue;

        auto json_release = json_value.toObject();
        if (json_release.contains("assets"))
        {
            if (!json_release["assets"].isArray()) continue;
            auto json_assets = json_release["assets"].toArray();
            for (auto json_asset_vref: json_assets){
                if (!json_asset_vref.isObject()) continue;
                auto json_asset = json_asset_vref.toObject();
                if (!json_asset.contains("browser_download_url")) continue;
                QString link = json_asset["browser_download_url"].toString();
                QString name = json_asset["name"].toString();
                QString linkhtml = QString("<a href=%1>%2</a>").arg( link, name);
                asset.links.append(linkhtml);
            }

            if (!asset.links.empty())
            {
                if (json_release.contains("tag_name"))
                    asset.version = json_release["tag_name"].toString();

                return asset;
            }
        }

    }

    return asset;
}

UpdateAction::UpdateAction(const QIcon &icon, const QString &text, QObject *parent) :
    QAction(icon, text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(showmessagebox()));
    connect(this, SIGNAL(triggered()), this, SLOT(update()));
}

void UpdateAction::showmessagebox()
{
    mbx = new QMessageBox(QMessageBox::Icon::NoIcon, qApp->translate("App", "Eyes' Thanks"), tr("Checking update..."),
                                      QMessageBox::StandardButton::Cancel);
    QFontMetrics fm(mbx->font());
    int height =  fm.boundingRect("A").height() * 6;
    mbx->setStyleSheet(QString("QLabel{min-height: %1px;}").arg(height));

    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = qobject_cast<QGridLayout *>(mbx->layout());
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    mbx->show();
    QCoreApplication::processEvents();
}

void UpdateAction::update()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));

    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/yalov/eyes-thanks/releases")));
}

QVector<int> UpdateAction::parseVersion(QString version_str)
{
    QVector<int> version = {0, 0, 0};
    QRegularExpression re1(R"((\d+)\.(\d+)\.(\d+))");
    QRegularExpressionMatch match1 = re1.match(version_str);
    for (int i = 0; i < match1.lastCapturedIndex(); ++i) // 0..2
        version[i] = match1.captured(i+1).toInt();

    return version;

}

void UpdateAction::replyFinished(QNetworkReply *reply)
{
    mbx->setStandardButtons(QMessageBox::Ok);

    if (reply->error() != QNetworkReply::NoError) {
        mbx->setIcon(QMessageBox::Critical);
        mbx->setText(tr("There was an error connecting to <a href='%1'>%1</a>.").arg(REPO_URL));
    }
    else {
        Asset asset = getAsset(reply->readAll());

        QVector<int> newVersion =  parseVersion(asset.version);
        QVector<int> currVersion = parseVersion(QString(APP_VERSION));

        qDebug() << "current: "<< currVersion  << "new: " << newVersion;

        QString text;
        if (currVersion == newVersion)
            text = tr("You are already running the most recent version <b>Eyes' Thanks %1</b>.").arg(APP_VERSION);
        else if (currVersion < newVersion)
            text = tr("A new version of <b>Eyes' Thanks</b> has been released! "
                      "Version <b>%1</b> is available at <a href=%2>%2</a>.<br><br>"
                      "Download:<br>"
                      "<a href=%3>%3</a>").arg(asset.version,REPO_URL,asset.links.join("<br>"));
        else
            text = tr("Fantastic! You have <b>Eyes' Thanks %1</b>, "
                      "but last available version is <b>%2</b>.<br><br>"
                      "Please, upload new version to <a href=%3>%3</a>.").arg(APP_VERSION,asset.version,REPO_URL);

        mbx->setText(text);
    }
    reply->deleteLater();
}
