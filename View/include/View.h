#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QFrame>
#include <QVector>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include "ui_MainWindow.h"
#include "ui_MultipleChoiceWindow.h"
#include "ui_SettingsWindow.h"
#include "ui_ScanWindow.h"
#include "ui_TheoryWindow.h"
#include "ui_ScanResultWindow.h"
#include "ui_ScanReviewWindow.h"

class Model;
class Controller;

enum class WindowType {
    MainWindow,
    MultipleChoiceWindow,
    SettingsWindow,
    ScanWindow,
    ScanResultWindow,
    ScanReviewWindow,
    TheoryWindow
};

class View : public QMainWindow
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    
    void setController(Controller* controller);
    void setModel(Model* model);
    // Public refresh to update MC content after async AI generation
    void refreshMultipleChoice(int unitIndex, int problemIndex);
    
    // Window navigation methods
    void showMainWindow();
    void showMultipleChoiceWindow(int unitIndex, int problemIndex);
    void showSettingsWindow(WindowType previousWindow);
    void showScanWindow(int unitIndex, int problemIndex);
    void showScanResultWindow(const QString& ocrResult);
    void showScanReviewWindow(const QString& gradingResult);
    void showTheoryWindow(int unitIndex, int problemIndex, WindowType previousWindow);
    
    // UI update methods
    void refreshUnits();
    void updateUnitProblems(int unitIndex);

signals:
    void problemSelected(int unitIndex, int problemIndex);
    void backButtonClicked();
    void settingsButtonClicked();
    void scanButtonClicked();
    void theoryButtonClicked();
    void choiceSelected(int choiceIndex);

private slots:
    void onProblemSelectionChanged();
    void onBackButtonClicked();
    void onSettingsButtonClicked();
    void onMainSettingsButtonClicked();
    void onScanButtonClicked();
    void onTheoryButtonClicked();
    void onChoiceButtonClicked();
    void onDifficultyChanged(int index);
    
    // Scan workflow slots
    void onScanResultBackButtonClicked();
    void onScanResultNextButtonClicked();
    void onScanReviewBackButtonClicked();
    void onScanReviewMenuButtonClicked();

private:
    // UI objects for all windows
    Ui::MainWindow *mainUI;
    Ui::MultipleChoiceWindow *multipleChoiceUI;
    Ui::SettingsWindow *settingsUI;
    Ui::ScanWindow *scanUI;
    Ui::TheoryWindow *theoryUI;
    Ui::scanConfirmWindow *scanResultUI;
    Ui::scanReviewWindow *scanReviewUI;
    
    // Dialog widgets
    QDialog *multipleChoiceWindow;
    QDialog *settingsWindow;
    QDialog *scanWindow;
    QDialog *theoryWindow;
    QDialog *scanResultWindow;
    QDialog *scanReviewWindow;
    
    Controller* controller;
    Model* model;
    
    // Navigation state
    WindowType currentWindow;
    WindowType previousWindow;
    int currentUnitIndex;
    int currentProblemIndex;
    int correctChoiceIndex;
    
    // Scan workflow state
    QString currentScanImagePath;
    QString currentOcrResult;
    QString currentProblemStatement;
    QString currentGradingResult;
    
    // Main window components
    QVBoxLayout* unitsLayout;
    QVector<QGroupBox*> unitGroupBoxes;
    QVector<QComboBox*> problemComboBoxes;
    
    // Setup methods
    void setupUI();
    void setupMainWindow();
    void setupMultipleChoiceWindow();
    void setupSettingsWindow();
    void setupScanWindow();
    void setupScanResultWindow();
    void setupScanReviewWindow();
    void setupTheoryWindow();
    void connectSignals();
    
    // Main window helper methods
    void createUnitWidget(int unitIndex);
    void clearUnitsLayout();
    QGroupBox* createStyledGroupBox(const QString& title);
    QLabel* createStyledLabel(const QString& text);
    QComboBox* createStyledComboBox();
    
    // Multiple choice helper methods
    void populateMultipleChoiceWindow(int unitIndex, int problemIndex);
    void setChoiceButtonsEnabled(bool enabled);
    void highlightCorrectChoice(int choiceIndex);
    void resetChoiceButtonStyles();
    
    // Content population methods
    void populateTheoryWindow(int unitIndex, int problemIndex);
    void populateScanWindow(int unitIndex, int problemIndex);
};

#endif // VIEW_H
