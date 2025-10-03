#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "SolutionGrader.h"
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

View::View(QWidget *parent)
    : QMainWindow(parent)
    , mainUI(nullptr)
    , multipleChoiceUI(nullptr)
    , settingsUI(nullptr)
    , scanUI(nullptr)
    , theoryUI(nullptr)
    , scanResultUI(nullptr)
    , scanReviewUI(nullptr)
    , multipleChoiceWindow(nullptr)
    , settingsWindow(nullptr)
    , scanWindow(nullptr)
    , theoryWindow(nullptr)
    , scanResultWindow(nullptr)
    , scanReviewWindow(nullptr)
    , controller(nullptr)
    , model(nullptr)
    , currentWindow(WindowType::MainWindow)
    , previousWindow(WindowType::MainWindow)
    , currentUnitIndex(-1)
    , currentProblemIndex(-1)
    , correctChoiceIndex(-1)
    , unitsLayout(nullptr)
{
    setupUI();
}

View::~View()
{
    delete mainUI;
    delete multipleChoiceUI;
    delete settingsUI;
    delete scanUI;
    delete theoryUI;
    delete scanResultUI;
    delete scanReviewUI;
}

void View::setController(Controller* ctrl)
{
    controller = ctrl;
}

void View::setModel(Model* mdl)
{
    model = mdl;
    if (model) {
        connect(model, &Model::unitsChanged, this, &View::refreshUnits);
        connect(model, &Model::unitProblemsChanged, this, &View::updateUnitProblems);
        refreshUnits();
    }
}

void View::setupUI()
{
    setupMainWindow();
    setupMultipleChoiceWindow();
    setupSettingsWindow();
    setupScanWindow();
    setupScanResultWindow();
    setupScanReviewWindow();
    setupTheoryWindow();
    
    connectSignals();
}

void View::setupMainWindow()
{
    mainUI = new Ui::MainWindow();
    mainUI->setupUi(this); // Use 'this' since View inherits from QMainWindow
    
    // Create main layout for the units container
    unitsLayout = new QVBoxLayout();
    unitsLayout->setSpacing(15);
    unitsLayout->setContentsMargins(10, 10, 10, 10);
    
    // Set the layout to the units container widget
    mainUI->unitsContainer->setLayout(unitsLayout);
    
    // Connect main settings button
    connect(mainUI->mainSettingsButton, &QPushButton::clicked, this, &View::onMainSettingsButtonClicked);
}

void View::setupMultipleChoiceWindow()
{
    multipleChoiceWindow = new QDialog(this);
    multipleChoiceWindow->setModal(true);
    multipleChoiceUI = new Ui::MultipleChoiceWindow();
    multipleChoiceUI->setupUi(multipleChoiceWindow);
}

void View::setupSettingsWindow()
{
    settingsWindow = new QDialog(this);
    settingsWindow->setModal(true);
    settingsUI = new Ui::SettingsWindow();
    settingsUI->setupUi(settingsWindow);
    
    // Set default difficulty to Medium (index 1)
    settingsUI->difficultyComboBox->setCurrentIndex(1);
}

void View::setupScanWindow()
{
    scanWindow = new QDialog(this);
    scanWindow->setModal(true);
    scanUI = new Ui::ScanWindow();
    scanUI->setupUi(scanWindow);
}

void View::setupScanResultWindow()
{
    scanResultWindow = new QDialog(this);
    scanResultWindow->setModal(true);
    scanResultUI = new Ui::scanConfirmWindow();
    scanResultUI->setupUi(scanResultWindow);
}

void View::setupScanReviewWindow()
{
    scanReviewWindow = new QDialog(this);
    scanReviewWindow->setModal(true);
    scanReviewUI = new Ui::scanReviewWindow();
    scanReviewUI->setupUi(scanReviewWindow);
}

void View::setupTheoryWindow()
{
    theoryWindow = new QDialog(this);
    theoryWindow->setModal(true);
    theoryUI = new Ui::TheoryWindow();
    theoryUI->setupUi(theoryWindow);
}

void View::connectSignals()
{
    // Multiple Choice Window signals
    if (multipleChoiceUI) {
        connect(multipleChoiceUI->backButton, &QPushButton::clicked, this, &View::onBackButtonClicked);
        connect(multipleChoiceUI->settingsButton, &QPushButton::clicked, this, &View::onSettingsButtonClicked);
        connect(multipleChoiceUI->theoryButton, &QPushButton::clicked, this, &View::onTheoryButtonClicked);
        connect(multipleChoiceUI->choiceButton1, &QPushButton::clicked, this, &View::onChoiceButtonClicked);
        connect(multipleChoiceUI->choiceButton2, &QPushButton::clicked, this, &View::onChoiceButtonClicked);
        connect(multipleChoiceUI->choiceButton3, &QPushButton::clicked, this, &View::onChoiceButtonClicked);
        connect(multipleChoiceUI->choiceButton4, &QPushButton::clicked, this, &View::onChoiceButtonClicked);
    }
    
    // Settings Window signals
    if (settingsUI) {
        connect(settingsUI->backButton, &QPushButton::clicked, this, &View::onBackButtonClicked);
        connect(settingsUI->difficultyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &View::onDifficultyChanged);
    }
    
    // Scan Window signals
    if (scanUI) {
        connect(scanUI->backButton, &QPushButton::clicked, this, &View::onBackButtonClicked);
        connect(scanUI->settingsButton, &QPushButton::clicked, this, &View::onSettingsButtonClicked);
        connect(scanUI->scanButton, &QPushButton::clicked, this, &View::onScanButtonClicked);
        connect(scanUI->theoryButton, &QPushButton::clicked, this, &View::onTheoryButtonClicked);
    }
    
    // Scan Result Window signals
    if (scanResultUI) {
        connect(scanResultUI->backButton, &QPushButton::clicked, this, &View::onScanResultBackButtonClicked);
        connect(scanResultUI->nextButton, &QPushButton::clicked, this, &View::onScanResultNextButtonClicked);
    }
    
    // Scan Review Window signals
    if (scanReviewUI) {
        connect(scanReviewUI->backButton, &QPushButton::clicked, this, &View::onScanReviewBackButtonClicked);
        connect(scanReviewUI->menuButton, &QPushButton::clicked, this, &View::onScanReviewMenuButtonClicked);
    }
    
    // Theory Window signals
    if (theoryUI) {
        connect(theoryUI->backButton, &QPushButton::clicked, this, &View::onBackButtonClicked);
        connect(theoryUI->settingsButton, &QPushButton::clicked, this, &View::onSettingsButtonClicked);
    }
}

// Window navigation methods
void View::showMainWindow()
{
    currentWindow = WindowType::MainWindow;
    // Main window is always visible - just bring to front if needed
    this->raise();
    this->activateWindow();
}

void View::showMultipleChoiceWindow(int unitIndex, int problemIndex)
{
    previousWindow = currentWindow;
    currentWindow = WindowType::MultipleChoiceWindow;
    currentUnitIndex = unitIndex;
    currentProblemIndex = problemIndex;
    
    populateMultipleChoiceWindow(unitIndex, problemIndex);
    
    // Center the dialog over the main window
    multipleChoiceWindow->move(
        this->x() + (this->width() - multipleChoiceWindow->width()) / 2,
        this->y() + (this->height() - multipleChoiceWindow->height()) / 2
    );
    
    multipleChoiceWindow->exec(); // Show as modal dialog
}

void View::showSettingsWindow(WindowType prevWindow)
{
    previousWindow = prevWindow;
    currentWindow = WindowType::SettingsWindow;
    
    // Center the dialog over the main window
    settingsWindow->move(
        this->x() + (this->width() - settingsWindow->width()) / 2,
        this->y() + (this->height() - settingsWindow->height()) / 2
    );
    
    settingsWindow->exec(); // Show as modal dialog
}

void View::showScanWindow(int unitIndex, int problemIndex)
{
    previousWindow = currentWindow;
    currentWindow = WindowType::ScanWindow;
    currentUnitIndex = unitIndex;
    currentProblemIndex = problemIndex;
    
    populateScanWindow(unitIndex, problemIndex);
    
    // Store the AI-generated problem statement for later use
    currentProblemStatement = model->getProblemStatement(unitIndex, problemIndex);
    
    // Center the dialog over the main window
    scanWindow->move(
        this->x() + (this->width() - scanWindow->width()) / 2,
        this->y() + (this->height() - scanWindow->height()) / 2
    );
    
    scanWindow->exec(); // Show as modal dialog
}

void View::showScanResultWindow(const QString& ocrResult)
{
    previousWindow = currentWindow;
    currentWindow = WindowType::ScanResultWindow;
    currentOcrResult = ocrResult;
    
    // Set the OCR result in the label
    scanResultUI->scanResultLabel->setText(ocrResult);
    scanResultUI->scanResultTitleLabel->setText("Scan Result");
    
    // Center the dialog
    scanResultWindow->move(
        this->x() + (this->width() - scanResultWindow->width()) / 2,
        this->y() + (this->height() - scanResultWindow->height()) / 2
    );
    
    scanResultWindow->exec();
}

void View::showScanReviewWindow(const QString& gradingResult)
{
    previousWindow = currentWindow;
    currentWindow = WindowType::ScanReviewWindow;
    currentGradingResult = gradingResult;
    
    // Set the grading result in the label
    scanReviewUI->scanReviewLabel->setText(gradingResult);
    scanReviewUI->scanReviewTitleLabel->setText("Solution Review");
    
    // Center the dialog
    scanReviewWindow->move(
        this->x() + (this->width() - scanReviewWindow->width()) / 2,
        this->y() + (this->height() - scanReviewWindow->height()) / 2
    );
    
    scanReviewWindow->exec();
}

void View::showTheoryWindow(int unitIndex, int problemIndex, WindowType prevWindow)
{
    previousWindow = prevWindow;
    currentWindow = WindowType::TheoryWindow;
    currentUnitIndex = unitIndex;
    currentProblemIndex = problemIndex;
    
    populateTheoryWindow(unitIndex, problemIndex);
    
    // Center the dialog over the main window
    theoryWindow->move(
        this->x() + (this->width() - theoryWindow->width()) / 2,
        this->y() + (this->height() - theoryWindow->height()) / 2
    );
    
    theoryWindow->exec(); // Show as modal dialog
}

// Content population methods
void View::populateMultipleChoiceWindow(int unitIndex, int problemIndex)
{
    if (!model || !multipleChoiceUI) return;
    
    // Set unit and problem label
    QString unitProblemText = model->unitProblemToString(unitIndex, problemIndex);
    multipleChoiceUI->unitLabel->setText(unitProblemText);
    
    // Set problem statement
    QString problemStatement = model->getProblemStatement(unitIndex, problemIndex);
    if (problemStatement.isEmpty() || problemStatement == "Problem not found") {
        multipleChoiceUI->problemLabel->setText("Generating problem... Please wait.");
    } else {
        multipleChoiceUI->problemLabel->setText(problemStatement);
    }
    
    // Set multiple choice options
    QVector<MultipleChoiceOption> choices = model->getMultipleChoiceOptions(unitIndex, problemIndex);
    correctChoiceIndex = model->getCorrectChoiceIndex(unitIndex, problemIndex);
    
    if (choices.size() >= 4) {
        multipleChoiceUI->choiceButton1->setText(choices[0].text);
        multipleChoiceUI->choiceButton2->setText(choices[1].text);
        multipleChoiceUI->choiceButton3->setText(choices[2].text);
        multipleChoiceUI->choiceButton4->setText(choices[3].text);
        setChoiceButtonsEnabled(true);
    } else {
        // No choices yet, disable until AI fills them
        multipleChoiceUI->choiceButton1->setText("A)");
        multipleChoiceUI->choiceButton2->setText("B)");
        multipleChoiceUI->choiceButton3->setText("C)");
        multipleChoiceUI->choiceButton4->setText("D)");
        setChoiceButtonsEnabled(false);
    }
    
    // Reset button styles to original (remove any green highlighting)
    resetChoiceButtonStyles();
}

void View::populateTheoryWindow(int unitIndex, int problemIndex)
{
    if (!model || !theoryUI) return;
    
    const Unit* unit = model->getUnit(unitIndex);
    if (unit && problemIndex >= 0 && problemIndex < unit->problems.size()) {
        const Problem& problem = unit->problems[problemIndex];
        theoryUI->theoryTitleLabel->setText(QString("📚 %1").arg(problem.name));
    }
    
    QString theoryContent = model->getTheoryContent(unitIndex, problemIndex);
    theoryUI->theoryLabel->setText(theoryContent);
}

void View::populateScanWindow(int unitIndex, int problemIndex)
{
    if (!model || !scanUI) return;
    
    QString unitProblemText = model->unitProblemToString(unitIndex, problemIndex);
    scanUI->unitLabel->setText(unitProblemText);
    
    // Display AI-generated problem statement
    QString problemStatement = model->getProblemStatement(unitIndex, problemIndex);
    if (problemStatement.isEmpty() || problemStatement == "Problem not found") {
        scanUI->scanProblemLabel->setText("AI-generated problem will appear here...");
    } else {
        scanUI->scanProblemLabel->setText(problemStatement);
    }
}

// Button event handlers
void View::onProblemSelectionChanged()
{
    QComboBox* senderCombo = qobject_cast<QComboBox*>(sender());
    if (!senderCombo) return;
    
    int unitIndex = senderCombo->property("unitIndex").toInt();
    int problemIndex = senderCombo->currentIndex() - 1; // -1 because first item is "-- Choose a problem --"
    
    if (problemIndex >= 0) {
        // Trigger AI generation first for all problems
        emit problemSelected(unitIndex, problemIndex);
        
        if (problemIndex < 3) {
            // Use QTimer to allow AI to generate before showing MC window
            QTimer::singleShot(100, this, [this, unitIndex, problemIndex]() {
                showMultipleChoiceWindow(unitIndex, problemIndex);
            });
        } else {
            // For scan window, also wait for AI generation to complete
            QTimer::singleShot(100, this, [this, unitIndex, problemIndex]() {
                showScanWindow(unitIndex, problemIndex);
            });
        }
    }
}

void View::onBackButtonClicked()
{
    // Close the current dialog and restore previous state
    QDialog* currentDialog = nullptr;
    
    switch (currentWindow) {
        case WindowType::MultipleChoiceWindow:
            currentDialog = multipleChoiceWindow;
            break;
        case WindowType::SettingsWindow:
            currentDialog = settingsWindow;
            break;
        case WindowType::ScanWindow:
            currentDialog = scanWindow;
            break;
        case WindowType::TheoryWindow:
            currentDialog = theoryWindow;
            break;
        default:
            break;
    }
    
    if (currentDialog) {
        // Restore the previous window state
        currentWindow = previousWindow;
        currentDialog->accept(); // Close the dialog
    }
    
    emit backButtonClicked();
}

void View::onSettingsButtonClicked()
{
    showSettingsWindow(currentWindow);
    emit settingsButtonClicked();
}

void View::onScanButtonClicked()
{
    // Open file dialog to select .png file
    QString fileName = QFileDialog::getOpenFileName(
        scanWindow,
        tr("Select Image"),
        "",
        tr("PNG Files (*.png)")
    );
    
    if (fileName.isEmpty()) {
        return; // User cancelled
    }
    
    currentScanImagePath = fileName;
    
    // Call OCR scanner through model
    if (!model) {
        QMessageBox::warning(scanWindow, tr("Error"), tr("Model not initialized!"));
        return;
    }
    
    QString ocrResult = model->scanImage(fileName);
    
    if (ocrResult.isEmpty()) {
        QMessageBox::warning(scanWindow, tr("Error"), tr("Failed to scan image!"));
        return;
    }
    
    // Close scan window and show result window
    scanWindow->accept();
    showScanResultWindow(ocrResult);
    
    emit scanButtonClicked();
}

void View::onTheoryButtonClicked()
{
    showTheoryWindow(currentUnitIndex, currentProblemIndex, currentWindow);
    emit theoryButtonClicked();
}

void View::onChoiceButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton || !multipleChoiceUI) return;
    
    int choiceIndex = -1;
    if (clickedButton == multipleChoiceUI->choiceButton1) choiceIndex = 0;
    else if (clickedButton == multipleChoiceUI->choiceButton2) choiceIndex = 1;
    else if (clickedButton == multipleChoiceUI->choiceButton3) choiceIndex = 2;
    else if (clickedButton == multipleChoiceUI->choiceButton4) choiceIndex = 3;
    
    if (choiceIndex >= 0) {
        // Disable all buttons
        setChoiceButtonsEnabled(false);
        
        // Highlight correct choice
        highlightCorrectChoice(correctChoiceIndex);
        
        qDebug() << "Choice selected:" << choiceIndex << "Correct:" << correctChoiceIndex;
        emit choiceSelected(choiceIndex);
    }
}

void View::onDifficultyChanged(int index)
{
    if (!model || !settingsUI) return;
    
    QString difficulty = settingsUI->difficultyComboBox->currentText();
    model->setUserDifficulty(difficulty);
    
    qDebug() << "Difficulty changed to:" << difficulty;
}

void View::onMainSettingsButtonClicked()
{
    showSettingsWindow(WindowType::MainWindow);
    emit settingsButtonClicked();
}

void View::onScanResultBackButtonClicked()
{
    // Go back to scan window, preserving state
    scanResultWindow->reject();
    showScanWindow(currentUnitIndex, currentProblemIndex);
}

void View::onScanResultNextButtonClicked()
{
    // User confirmed OCR result, now grade the solution
    if (!controller) {
        QMessageBox::warning(scanResultWindow, tr("Error"), tr("Controller not initialized!"));
        return;
    }
    
    // Create a SolutionGrader instance
    SolutionGrader grader;
    QString gradingResult = grader.gradeSolution(currentOcrResult, currentProblemStatement);
    
    // Close result window and show review window
    scanResultWindow->accept();
    showScanReviewWindow(gradingResult);
}

void View::onScanReviewBackButtonClicked()
{
    // Go back to scan result window, preserving state
    scanReviewWindow->reject();
    showScanResultWindow(currentOcrResult);
}

void View::onScanReviewMenuButtonClicked()
{
    // Go back to main menu
    scanReviewWindow->accept();
    showMainWindow();
}

// Helper methods
void View::setChoiceButtonsEnabled(bool enabled)
{
    if (!multipleChoiceUI) return;
    
    multipleChoiceUI->choiceButton1->setEnabled(enabled);
    multipleChoiceUI->choiceButton2->setEnabled(enabled);
    multipleChoiceUI->choiceButton3->setEnabled(enabled);
    multipleChoiceUI->choiceButton4->setEnabled(enabled);
}

void View::highlightCorrectChoice(int choiceIndex)
{
    if (!multipleChoiceUI) return;
    
    QPushButton* correctButton = nullptr;
    switch (choiceIndex) {
        case 0: correctButton = multipleChoiceUI->choiceButton1; break;
        case 1: correctButton = multipleChoiceUI->choiceButton2; break;
        case 2: correctButton = multipleChoiceUI->choiceButton3; break;
        case 3: correctButton = multipleChoiceUI->choiceButton4; break;
    }
    
    if (correctButton) {
        QString greenStyle = correctButton->styleSheet() + 
            "\nQPushButton { border: 3px solid rgb(0, 255, 0); background-color: rgb(0, 150, 0); }";
        correctButton->setStyleSheet(greenStyle);
    }
}

void View::resetChoiceButtonStyles()
{
    if (!multipleChoiceUI) return;
    
    // Define the original button style (from the UI file)
    QString originalStyle = 
        "QPushButton {\n"
        "    border: 2px solid rgb(255, 140, 0);\n"
        "    border-radius: 5px;\n"
        "    padding: 6px;\n"
        "    color: rgb(255, 140, 0);\n"
        "    background-color: rgb(80, 40, 120);\n"
        "}";
    
    // Reset all choice buttons to original style
    multipleChoiceUI->choiceButton1->setStyleSheet(originalStyle);
    multipleChoiceUI->choiceButton2->setStyleSheet(originalStyle);
    multipleChoiceUI->choiceButton3->setStyleSheet(originalStyle);
    multipleChoiceUI->choiceButton4->setStyleSheet(originalStyle);
}

// Main window methods (keeping existing functionality)
void View::refreshMultipleChoice(int unitIndex, int problemIndex)
{
    populateMultipleChoiceWindow(unitIndex, problemIndex);
}
void View::refreshUnits()
{
    if (!model) return;
    
    clearUnitsLayout();
    
    // Create widgets for each unit
    for (int i = 0; i < model->getUnitCount(); ++i) {
        createUnitWidget(i);
    }
    
    // Add stretch to push everything to the top
    unitsLayout->addStretch();
}

void View::createUnitWidget(int unitIndex)
{
    if (!model) return;
    
    const Unit* unit = model->getUnit(unitIndex);
    if (!unit) return;
    
    // Create group box for the unit
    QGroupBox* unitGroupBox = createStyledGroupBox(unit->name);
    
    // Create vertical layout for the unit content
    QVBoxLayout* unitLayout = new QVBoxLayout(unitGroupBox);
    unitLayout->setSpacing(10);
    
    // Add unit description label
    QLabel* descLabel = createStyledLabel(unit->description);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(descLabel->styleSheet() + 
        "font-size: 12px; color: rgb(200, 200, 200); font-weight: normal;");
    unitLayout->addWidget(descLabel);
    
    // Create horizontal layout for problem selection
    QHBoxLayout* problemLayout = new QHBoxLayout();
    
    // Add "Select Problem:" label
    QLabel* problemLabel = createStyledLabel("Select Problem:");
    problemLabel->setStyleSheet(problemLabel->styleSheet() + 
        "font-size: 14px; font-weight: bold; border: none; background: transparent; padding: 0px;");
    problemLayout->addWidget(problemLabel);
    
    // Create and populate combo box
    QComboBox* problemCombo = createStyledComboBox();
    problemCombo->addItem("-- Choose a problem --");
    
    QStringList problems = model->getProblemsForUnit(unitIndex);
    problemCombo->addItems(problems);
    
    // Store the unit index as property for signal handling
    problemCombo->setProperty("unitIndex", unitIndex);
    connect(problemCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &View::onProblemSelectionChanged);
    
    problemLayout->addWidget(problemCombo);
    problemLayout->addStretch(); // Push everything to the left
    
    unitLayout->addLayout(problemLayout);
    
    // Store references
    unitGroupBoxes.append(unitGroupBox);
    problemComboBoxes.append(problemCombo);
    
    // Add to main layout
    unitsLayout->addWidget(unitGroupBox);
}

void View::updateUnitProblems(int unitIndex)
{
    if (unitIndex >= 0 && unitIndex < problemComboBoxes.size()) {
        QComboBox* combo = problemComboBoxes[unitIndex];
        combo->clear();
        combo->addItem("-- Choose a problem --");
        
        if (model) {
            QStringList problems = model->getProblemsForUnit(unitIndex);
            combo->addItems(problems);
        }
    }
}

void View::clearUnitsLayout()
{
    // Clear stored references
    unitGroupBoxes.clear();
    problemComboBoxes.clear();
    
    // Remove all widgets from layout
    while (QLayoutItem* item = unitsLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

QGroupBox* View::createStyledGroupBox(const QString& title)
{
    QGroupBox* groupBox = new QGroupBox(title);
    groupBox->setStyleSheet(
        "QGroupBox {"
        "    font-family: 'Comic Sans MS';"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: rgb(255, 140, 0);"
        "    border: 2px solid rgb(255, 140, 0);"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "    background-color: rgb(80, 40, 120);"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 5px 10px;"
        "    background-color: rgb(80, 40, 120);"
        "    border-radius: 4px;"
        "}"
    );
    return groupBox;
}

QLabel* View::createStyledLabel(const QString& text)
{
    QLabel* label = new QLabel(text);
    label->setStyleSheet(
        "QLabel {"
        "    font-family: 'Comic Sans MS';"
        "    font-size: 14px;"
        "    color: rgb(255, 140, 0);"
        "    background-color: transparent;"
        "    border: 1px solid rgb(255, 140, 0);"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "}"
    );
    return label;
}

QComboBox* View::createStyledComboBox()
{
    QComboBox* comboBox = new QComboBox();
    comboBox->setStyleSheet(
        "QComboBox {"
        "    font-family: 'Comic Sans MS';"
        "    font-size: 12px;"
        "    color: rgb(255, 140, 0);"
        "    background-color: rgb(80, 40, 120);"
        "    border: 2px solid rgb(255, 140, 0);"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    min-width: 200px;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 20px;"
        "    border-left: 1px solid rgb(255, 140, 0);"
        "    border-top-right-radius: 3px;"
        "    border-bottom-right-radius: 3px;"
        "    background-color: rgb(100, 50, 140);"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 5px solid transparent;"
        "    border-right: 5px solid transparent;"
        "    border-top: 5px solid rgb(255, 140, 0);"
        "    width: 0px;"
        "    height: 0px;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: rgb(80, 40, 120);"
        "    color: rgb(255, 140, 0);"
        "    selection-background-color: rgb(100, 50, 140);"
        "    border: 1px solid rgb(255, 140, 0);"
        "}"
        "QComboBox:hover {"
        "    background-color: rgb(90, 45, 130);"
        "}"
    );
    return comboBox;
}