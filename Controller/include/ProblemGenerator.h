#ifndef PROBLEMGENERATOR_H
#define PROBLEMGENERATOR_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "AIService.h"
#include "../Model/include/Model.h"

class ProblemGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ProblemGenerator(QObject *parent = nullptr);
    
    // Set the model reference to get current problem and difficulty
    void setModel(Model* model);
    
    // Generate problem based on current model selection
    void generateProblem();
    
    // Generate problem with specific parameters (for testing/manual use)
    void generateProblem(const QString& problemType, const QString& difficulty);
    
    // Synchronous version that blocks and returns the generated problem
    QString generateProblemSync(const QString& problemType, const QString& difficulty);

signals:
    void problemGenerated(const QString& problem);
    void errorOccurred(const QString& error);

private slots:
    void handleAIResponse(const QString& response);
    void handleAIError(const QString& error);

private:
    AIService* aiService;
    Model* model;
    
    // Helper methods
    QString createPrompt(const QString& problemType, const QString& difficulty);
    QString extractProblemFromResponse(const QString& response);
};

#endif // PROBLEMGENERATOR_H
