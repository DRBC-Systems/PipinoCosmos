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
        emit errorOccurred(reply->errorString());
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
void AIService::ocr(const QString &imagePath)
{
    QUrl url(baseUrl + "/ocr");
    QNetworkRequest request(url);

    // Prepare multipart form
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(imagePath).fileName() + "\""));

    QFile *file = new QFile(imagePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit errorOccurred("Failed to open image: " + imagePath);
        delete multiPart;
        return;
    }

    imagePart.setBodyDevice(file);
    file->setParent(multiPart); // file will be deleted with multiPart
    multiPart->append(imagePart);

    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply); // ensure cleanup
    connect(reply, &QNetworkReply::finished, [=]() { handleReply(reply); });
}

QString AIService::ocrSync(const QString &imagePath, int timeoutMs)
{
    QUrl url(baseUrl + "/ocr");
    QNetworkRequest request(url);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(imagePath).fileName() + "\""));

    QFile *file = new QFile(imagePath);
    if (!file->open(QIODevice::ReadOnly)) {
        return "Error: Could not open file " + imagePath;
    }

    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    // Same blocking logic as your promptSync
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QString result;
    bool finished = false;

    connect(reply, &QNetworkReply::finished, [&]() {
        if (reply->error() == QNetworkReply::NoError) {
            result = QString(reply->readAll());
        } else {
            result = QString("Error: %1").arg(reply->errorString());
        }
        finished = true;
        loop.quit();
    });

    connect(&timer, &QTimer::timeout, [&]() {
        result = "Timeout: No response from OCR service within " + QString::number(timeoutMs) + "ms";
        reply->abort();
        finished = true;
        loop.quit();
    });

    timer.start(timeoutMs);
    if (!finished) {
        loop.exec();
    }

    reply->deleteLater();
    return result;
}
