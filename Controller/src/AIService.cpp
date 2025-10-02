#include "AIService.h"
#include <QUrlQuery>
#include <QJsonArray>
#include <QDebug>

AIService::AIService(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void AIService::prompt(const QString &message)
{
    QUrl url(baseUrl + "/prompt");
    QUrlQuery query;
    query.addQueryItem("message", message);
    url.setQuery(query);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, [=]() { handleReply(reply); });
}

void AIService::chat(const QJsonArray &messages)
{
    QUrl url(baseUrl + "/chat");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    payload["messages"] = messages;

    QNetworkReply *reply = manager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, [=]() { handleReply(reply); });
}

void AIService::embeddings(const QString &text)
{
    QUrl url(baseUrl + "/embeddings");
    QUrlQuery query;
    query.addQueryItem("message", text);
    url.setQuery(query);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, [=]() { handleReply(reply); });
}

void AIService::rag(const QString &queryText)
{
    QUrl url(baseUrl + "/rag");
    QUrlQuery query;
    query.addQueryItem("message", queryText);
    url.setQuery(query);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, [=]() { handleReply(reply); });
}

void AIService::handleReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        emit finished(QString(response));
    }
    else
    {
        qDebug() << "AIService Error:" << reply->errorString();
        emit finished(QString("Error: %1").arg(reply->errorString()));
    }
    reply->deleteLater();
}
