#include "AIService.h"
#include <QUrlQuery>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

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

QString AIService::promptSync(const QString &message, int timeoutMs)
{
    QUrl url(baseUrl + "/prompt");
    QUrlQuery query;
    query.addQueryItem("message", message);
    url.setQuery(query);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    
    // Create event loop to wait for response
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    QString result;
    bool finished = false;
    
    // Connect to handle successful response
    connect(reply, &QNetworkReply::finished, [&]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            result = QString(response);
            qDebug() << "AIService Sync Response:" << result;
        } else {
            result = QString("Error: %1").arg(reply->errorString());
            qDebug() << "AIService Sync Error:" << reply->errorString();
        }
        finished = true;
        loop.quit();
    });
    
    // Connect timeout handler
    connect(&timer, &QTimer::timeout, [&]() {
        result = "Timeout: No response from AI service within " + QString::number(timeoutMs) + "ms";
        qDebug() << "AIService Sync Timeout";
        finished = true;
        reply->abort(); // Cancel the request
        loop.quit();
    });
    
    // Start timeout timer
    timer.start(timeoutMs);
    
    // Block until response or timeout
    if (!finished) {
        loop.exec();
    }
    
    reply->deleteLater();
    return result;
}

void AIService::handleReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        // Success case - only emit finished signal
        QByteArray response = reply->readAll();
        emit finished(QString(response));
    }
    else
    {
        // Error case - emit errorOccurred signal
        qDebug() << "AIService Error:" << reply->errorString();
        emit errorOccurred(reply->errorString());
        emit finished(QString("Error: %1").arg(reply->errorString()));
    }
    reply->deleteLater();
}
