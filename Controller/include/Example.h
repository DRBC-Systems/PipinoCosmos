#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QObject>
#include <QString>
#include <QJsonArray>

class AIService;  // forward declaration

class Example : public QObject
{
    Q_OBJECT

public:
    explicit Example(QObject *parent = nullptr);

    void askAI(const QString &message);

private:
    AIService *ai;
};

#endif // EXAMPLE_H
