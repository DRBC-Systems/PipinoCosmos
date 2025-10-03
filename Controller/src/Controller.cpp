#include "Controller.h"
#include "Model.h"
#include "View.h"
#include <QDateTime>
#include <QMessageBox>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , model(nullptr)
    , view(nullptr)
    , currentUnitIndex(-1)
    , currentProblemIndex(-1)
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
        logUserAction("Application Started", QString("Units available: %1").arg(model->getUnitCount()));
        
        // 🧪 TEST THE NEW FUNCTIONS WITH NO SELECTION - Show results in a message box
        std::string noProblem = model->getCurrentProblem();
        std::string noDifficulty = model->getCurrentDifficulty();
        
        QString noSelectionTest = QString("🧪 TESTING WITH NO SELECTION:\n\n"
                                         "getCurrentProblem() returned:\n'%1'\n\n"
                                         "getCurrentDifficulty() returned:\n'%2'\n"
                                         "(This comes from USER SETTINGS, default: Medium)\n\n"
                                         "Now select a problem to test! 🎯")
                                 .arg(QString::fromStdString(noProblem))
                                 .arg(QString::fromStdString(noDifficulty));
        
        // Show the test results in a message box
        QMessageBox::information(nullptr, "Model Function Test - No Selection", noSelectionTest);
    }
}

void Controller::handleProblemSelection(int unitIndex, int problemIndex)
{
    if (!model) return;
    
    currentUnitIndex = unitIndex;
    currentProblemIndex = problemIndex;
    
    // Update the Model's current selection so getCurrentProblem() and getCurrentDifficulty() work
    model->setCurrentSelection(unitIndex, problemIndex);
    
    // 🧪 TEST THE NEW FUNCTIONS - Show results in a message box
    std::string currentProblem = model->getCurrentProblem();
    std::string currentDifficulty = model->getCurrentDifficulty();
    bool isMultipleChoice = model->isCurrentProblemMultipleChoice();
    
    // 🧪 TEST CHANGING USER DIFFICULTY SETTING
    model->setUserDifficulty("Hard");  // Change user setting to Hard
    std::string newDifficulty = model->getCurrentDifficulty();
    
    QString testMessage = QString("🧪 TESTING NEW MODEL FUNCTIONS:\n\n"
                                 "getCurrentProblem() returned:\n'%1'\n\n"
                                 "getCurrentDifficulty() returned:\n'%2'\n"
                                 "(Changed user setting to 'Hard' - see it works!)\n\n"
                                 "isCurrentProblemMultipleChoice() returned:\n%3\n\n"
                                 "All functions are working! ✅")
                         .arg(QString::fromStdString(currentProblem))
                         .arg(QString::fromStdString(newDifficulty))
                         .arg(isMultipleChoice ? "true (Multiple Choice)" : "false (Scan Problem)");
    
    // Show the test results in a message box
    QMessageBox::information(nullptr, "Model Function Test Results", testMessage);
    
    const Unit* unit = model->getUnit(unitIndex);
    if (unit && problemIndex >= 0 && problemIndex < unit->problems.size()) {
        const Problem& problem = unit->problems[problemIndex];
        
        QString logDetails = QString("Unit: %1, Problem: %2, Difficulty: %3")
                           .arg(unit->name, problem.name, problem.difficulty);
        
        if (problemIndex < 3) {
            // First 3 problems use MultipleChoiceWindow
            logUserAction("Multiple Choice Problem Selected", logDetails);
            qDebug() << "Opening Multiple Choice for:" << problem.name;
        } else {
            // Problems 4+ use ScanWindow
            logUserAction("Scan Problem Selected", logDetails);
            qDebug() << "Opening Scan Window for:" << problem.name;
        }
    }
}

void Controller::addNewUnit(const QString& name, const QString& description)
{
    if (model) {
        Unit newUnit(name, description);
        model->addUnit(newUnit);
        logUserAction("Unit Added", QString("Name: %1").arg(name));
        qDebug() << "Added new unit:" << name;
    }
}

void Controller::addProblemToUnit(int unitIndex, const QString& problemName, 
                                 const QString& description, const QString& difficulty)
{
    if (model) {
        Problem newProblem(problemName, description, difficulty);
        model->addProblemToUnit(unitIndex, newProblem);
        logUserAction("Problem Added", QString("Unit %1, Problem: %2").arg(unitIndex).arg(problemName));
        qDebug() << "Added problem" << problemName << "to unit" << unitIndex;
    }
}

void Controller::onProblemSelected(int unitIndex, int problemIndex)
{
    handleProblemSelection(unitIndex, problemIndex);
}

void Controller::onBackButtonClicked()
{
    logUserAction("Back Button Clicked", QString("From Unit %1, Problem %2").arg(currentUnitIndex).arg(currentProblemIndex));
    qDebug() << "Back button clicked - returning to previous window";
}

void Controller::onSettingsButtonClicked()
{
    logUserAction("Settings Opened", QString("From Unit %1, Problem %2").arg(currentUnitIndex).arg(currentProblemIndex));
    qDebug() << "Settings button clicked - opening settings window";
}

void Controller::onScanButtonClicked()
{
    logUserAction("Scan Button Clicked", QString("Unit %1, Problem %2").arg(currentUnitIndex).arg(currentProblemIndex));
    qDebug() << "Scan button clicked - would open camera interface";
    
    // Here you would integrate with camera/scanning functionality
    // For now, just log the action
}

void Controller::onTheoryButtonClicked()
{
    if (model && currentUnitIndex >= 0 && currentProblemIndex >= 0) {
        const Unit* unit = model->getUnit(currentUnitIndex);
        if (unit && currentProblemIndex < unit->problems.size()) {
            const Problem& problem = unit->problems[currentProblemIndex];
            logUserAction("Theory Opened", QString("Unit: %1, Problem: %2").arg(unit->name, problem.name));
            qDebug() << "Theory button clicked for:" << problem.name;
        }
    }
}

void Controller::onChoiceSelected(int choiceIndex)
{
    if (model && currentUnitIndex >= 0 && currentProblemIndex >= 0) {
        int correctIndex = model->getCorrectChoiceIndex(currentUnitIndex, currentProblemIndex);
        bool isCorrect = (choiceIndex == correctIndex);
        
        QString result = isCorrect ? "Correct" : "Incorrect";
        QString logDetails = QString("Choice: %1, Correct: %2, Result: %3")
                           .arg(choiceIndex).arg(correctIndex).arg(result);
        
        logUserAction("Answer Selected", logDetails);
        
        if (isCorrect) {
            qDebug() << "🎉 Correct answer selected! Choice:" << choiceIndex;
        } else {
            qDebug() << "❌ Incorrect answer. Selected:" << choiceIndex << "Correct:" << correctIndex;
        }
        
        // Here you could add scoring, progress tracking, etc.
    }
}

void Controller::connectSignals()
{
    if (view) {
        connect(view, &View::problemSelected, this, &Controller::onProblemSelected);
        connect(view, &View::backButtonClicked, this, &Controller::onBackButtonClicked);
        connect(view, &View::settingsButtonClicked, this, &Controller::onSettingsButtonClicked);
        connect(view, &View::scanButtonClicked, this, &Controller::onScanButtonClicked);
        connect(view, &View::theoryButtonClicked, this, &Controller::onTheoryButtonClicked);
        connect(view, &View::choiceSelected, this, &Controller::onChoiceSelected);
    }
}

void Controller::logUserAction(const QString& action, const QString& details)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logMessage = QString("[%1] %2").arg(timestamp, action);
    
    if (!details.isEmpty()) {
        logMessage += QString(" - %1").arg(details);
    }
    
    qDebug() << logMessage;
    
    // Here you could also log to a file, send to analytics, etc.
}
