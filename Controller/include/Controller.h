#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>

class Model;
class View;

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);
    
    void setModel(Model* model);
    void setView(View* view);
    
    // Application control methods
    void initializeApplication();
    void handleProblemSelection(int unitIndex, int problemIndex);
    
    // Data management methods
    void addNewUnit(const QString& name, const QString& description);
    void addProblemToUnit(int unitIndex, const QString& problemName, 
                         const QString& description, const QString& difficulty);

public slots:
    void onProblemSelected(int unitIndex, int problemIndex);

private:
    Model* model;
    View* view;
    
    void connectSignals();
};

#endif // CONTROLLER_H
