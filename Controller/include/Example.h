#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QObject>

class Example : public QObject
{
    Q_OBJECT
public:
    explicit Example(QObject *parent = nullptr);

    void testAI();  // function to run AI tests
};

#endif // EXAMPLE_H
