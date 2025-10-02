#include "Example.h"
#include "AIService.h"
#include <QDebug>

Example::Example(QObject *parent) : QObject(parent)
{
    ai = new AIService(this);  // create AI service
}

void Example::askAI(const QString &message)
{
    // Connect finished signal to print only the answer
    connect(ai, &AIService::finished, this, [](const QString &response){
        qDebug() << "AI Answer:" << response;  // only the AI's answer
    });

    // Optional: connect error signal
    connect(ai, &AIService::errorOccurred, this, [](const QString &err){
        qDebug() << "AI Error:" << err;
    });

    // Send the message as a prompt
    ai->prompt(message);

    // Or if you want to use chat:
    // QJsonArray messages;
    // messages.append(QJsonObject{{"type","Human"},{"text", message}});
    // ai->chat(messages);
}
