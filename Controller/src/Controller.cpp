#include "Controller.h"
#include "Model.h"
#include "View.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
    , model(nullptr)
    , view(nullptr)
{
}

void Controller::setModel(Model* mdl)
{
    model = mdl;
}

void Controller::setView(View* vw)
{
    view = vw;
    if (view) {
        connectSignals();
    }
}

void Controller::initializeApplication()
{
    if (model && view) {
        view->setModel(model);
        view->setController(this);
        qDebug() << "Application initialized with" << model->getUnitCount() << "units";
    }
}

void Controller::handleProblemSelection(int unitIndex, int problemIndex)
{
    if (!model) return;
    
    const Unit* unit = model->getUnit(unitIndex);
    if (unit && problemIndex >= 0 && problemIndex < unit->problems.size()) {
        const Problem& problem = unit->problems[problemIndex];
        
        qDebug() << "Selected problem:" << problem.name 
                 << "from unit:" << unit->name
                 << "difficulty:" << problem.difficulty;
        
        // Here you can add logic to handle the selected problem
        // For example: open a problem-solving interface, load problem data, etc.
    }
}

void Controller::addNewUnit(const QString& name, const QString& description)
{
    if (model) {
        Unit newUnit(name, description);
        model->addUnit(newUnit);
        qDebug() << "Added new unit:" << name;
    }
}

void Controller::addProblemToUnit(int unitIndex, const QString& problemName, 
                                 const QString& description, const QString& difficulty)
{
    if (model) {
        Problem newProblem(problemName, description, difficulty);
        model->addProblemToUnit(unitIndex, newProblem);
        qDebug() << "Added problem" << problemName << "to unit" << unitIndex;
    }
}

void Controller::onProblemSelected(int unitIndex, int problemIndex)
{
    handleProblemSelection(unitIndex, problemIndex);
}

void Controller::connectSignals()
{
    if (view) {
        connect(view, &View::problemSelected, this, &Controller::onProblemSelected);
    }
}
