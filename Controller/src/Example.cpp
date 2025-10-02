#include "Example.h"
#include "AIService.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

Example::Example(QObject *parent) : QObject(parent)
{
    // Optional: run tests immediately when object is created
    // testAI();
}

void Example::testAI()
{
    // --- Create AIService instance ---
    AIService *ai = new AIService(this);  // 'this' is valid because we are in a QObject

    // -----------------------------
    // Test 1: Prompt
    // -----------------------------
    connect(ai, &AIService::finished, [](const QString &response){
        qDebug() << "AI prompt response:" << response;
    });

    ai->prompt("What is the capital of Greece?");

    // -----------------------------
    // Test 2: Chat
    // -----------------------------
    QJsonArray messages;
    messages.append(QJsonObject{{"type", "Human"}, {"text", "Hello AI"}});

    connect(ai, &AIService::finished, [](const QString &response){
        qDebug() << "AI chat response:" << response;
    });

    ai->chat(messages);

    // You can add more tests: embeddings, RAG, etc.
    ai->embeddings("Artificial intelligence in education");
    ai->rag("Did the Queen turn purple?");
}
