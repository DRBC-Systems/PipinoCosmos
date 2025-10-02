#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QFrame>
#include <QVector>
#include <QGroupBox>
#include "ui_MainWindow.h"

class Model;
class Controller;

class View : public QMainWindow
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    
    void setController(Controller* controller);
    void setModel(Model* model);
    
    // UI update methods
    void refreshUnits();
    void updateUnitProblems(int unitIndex);

signals:
    void problemSelected(int unitIndex, int problemIndex);

private slots:
    void onProblemSelectionChanged();

private:
    Ui::MainWindow *ui;
    Controller* controller;
    Model* model;
    
    QVBoxLayout* unitsLayout;
    QVector<QGroupBox*> unitGroupBoxes;
    QVector<QComboBox*> problemComboBoxes;
    
    void setupUI();
    void createUnitWidget(int unitIndex);
    void clearUnitsLayout();
    QGroupBox* createStyledGroupBox(const QString& title);
    QLabel* createStyledLabel(const QString& text);
    QComboBox* createStyledComboBox();
};

#endif // VIEW_H
