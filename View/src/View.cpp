#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <QApplication>

View::View(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(nullptr)
    , model(nullptr)
    , unitsLayout(nullptr)
{
    ui->setupUi(this);
    setupUI();
}

View::~View()
{
    delete ui;
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
    // Create main layout for the units container
    unitsLayout = new QVBoxLayout();
    unitsLayout->setSpacing(15);
    unitsLayout->setContentsMargins(10, 10, 10, 10);
    
    // Set the layout to the units container widget
    ui->unitsContainer->setLayout(unitsLayout);
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

void View::onProblemSelectionChanged()
{
    QComboBox* senderCombo = qobject_cast<QComboBox*>(sender());
    if (!senderCombo) return;
    
    int unitIndex = senderCombo->property("unitIndex").toInt();
    int problemIndex = senderCombo->currentIndex() - 1; // -1 because first item is "-- Choose a problem --"
    
    if (problemIndex >= 0) {
        emit problemSelected(unitIndex, problemIndex);
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
