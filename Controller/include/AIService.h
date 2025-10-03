#ifndef AISERVICE_H
#define AISERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QFile>

class AIService : public QObject
{
    Q_OBJECT
public:
    explicit AIService(QObject *parent = nullptr);

    // Endpoints
    void prompt(const QString &message);
    void chat(const QJsonArray &messages);
    void embeddings(const QString &text);
    void rag(const QString &query);

    // OCR endpoints
    void ocr(const QString &imagePath);                          // async
    QString ocrSync(const QString &imagePath, int timeoutMs = 10000); // blocking

    // Synchronous method that blocks and returns response
    QString promptSync(const QString &message, int timeoutMs = 10000);

signals:
    void finished(const QString &response);
    void errorOccurred(const QString &error);

private slots:
    void handleReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    const QString baseUrl = "http://localhost:3000/genai";
};

#endif // AISERVICE_H
