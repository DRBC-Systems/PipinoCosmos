#include "ProblemGenerator.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

ProblemGenerator::ProblemGenerator(QObject *parent)
    : QObject(parent), aiService(nullptr), model(nullptr)
{
    // Initialize AI service
    aiService = new AIService(this);
    
    // Connect AI service signals
    connect(aiService, &AIService::finished, this, &ProblemGenerator::handleAIResponse);
    connect(aiService, &AIService::errorOccurred, this, &ProblemGenerator::handleAIError);
}

void ProblemGenerator::setModel(Model* modelPtr)
{
    model = modelPtr;
}

void ProblemGenerator::generateProblem()
{
    if (!model) {
        emit errorOccurred("Model not set. Please set the model before generating problems.");
        return;
    }
    
    // Get current problem and difficulty from model
    std::string currentProblem = model->getCurrentProblem();
    std::string currentDifficulty = model->getCurrentDifficulty();
    
    // Convert to QString for easier handling
    QString problemType = QString::fromStdString(currentProblem);
    QString difficulty = QString::fromStdString(currentDifficulty);
    
    // Check if we have valid data
    if (problemType == "No problem selected" || difficulty == "No difficulty selected") {
        emit errorOccurred("No problem or difficulty selected. Please select a problem first.");
        return;
    }
    
    generateProblem(problemType, difficulty);
}

void ProblemGenerator::generateProblem(const QString& problemType, const QString& difficulty)
{
    if (!aiService) {
        emit errorOccurred("AI Service not initialized.");
        return;
    }
    
    // Create appropriate prompt
    QString prompt = createPrompt(problemType, difficulty);
    
    qDebug() << "Generating problem for:" << problemType << "at" << difficulty << "difficulty";
    qDebug() << "Prompt:" << prompt;
    
    // Send prompt to AI service
    aiService->prompt(prompt);
}

QString ProblemGenerator::createPrompt(const QString& problemType, const QString& difficulty)
{
    QString difficultyLower = difficulty.toLower();
    
    QString prompt;
    
    if (difficultyLower == "easy") {
        prompt = QString(
            "Generate a simple %1 problem suitable for beginners. With no words, just the problem statement."
            "Requirements:\n"
            "- Use basic concepts and simple numbers\n"
            "- The problem should be solvable in 1-2 steps\n"
            "- Provide only the problem statement, no solution\n"
            "- Example format: 'Calculate: 15 + 8 - 3'\n\n"
            "Topic: %1\n"
            "Difficulty: Easy"
        ).arg(problemType);
    }
    else if (difficultyLower == "medium") {
        prompt = QString(
            "Generate a %1 problem of moderate difficulty. "
            "Requirements:\n"
            "- Use intermediate-level concepts and reasonable numbers\n"
            "- Include some context or real-world application if appropriate\n"
            "- The problem should challenge but not overwhelm\n"
            "- Provide only the problem statement, no solution\n"
            "- Make it educational and practical\n"
            "- Example format: 'A rectangle has length 12 cm and width 8 cm. Find its area and perimeter.'\n\n"
            "Topic: %1\n"
            "Difficulty: Medium"
        ).arg(problemType);
    }
    else if (difficultyLower == "hard") {
        prompt = QString(
            "Generate a challenging %1 problem for advanced students. "
            "Requirements:\n"
            "- Create a complex, multi-step problem\n"
            "- Use advanced concepts and realistic scenarios\n"
            "- Include detailed context and real-world applications\n"
            "- The problem should require deep understanding and multiple solution steps\n"
            "- Provide only the problem statement, no solution\n"
            "- Make it intellectually stimulating and comprehensive\n"
            "- Example format: 'A projectile is launched at 45° with initial velocity 25 m/s from a 10m high platform. Calculate the maximum height, time of flight, and horizontal range.'\n\n"
            "Topic: %1\n"
            "Difficulty: Hard"
        ).arg(problemType);
    }
    else {
        // Default to medium if difficulty is not recognized
        prompt = QString(
            "Generate a %1 problem. "
            "Requirements:\n"
            "- Create a clear problem statement\n"
            "- Use appropriate mathematical concepts\n"
            "- Provide only the problem statement, no solution\n"
            "- Make it educational and engaging\n\n"
            "Topic: %1"
        ).arg(problemType);
    }
    
    return prompt;
}

void ProblemGenerator::handleAIResponse(const QString& response)
{
    qDebug() << "AI Response received:" << response;
    
    // Extract the problem from the response
    QString problem = extractProblemFromResponse(response);
    
    if (problem.isEmpty()) {
        emit errorOccurred("Received empty or invalid response from AI service.");
        return;
    }
    
    emit problemGenerated(problem);
}

void ProblemGenerator::handleAIError(const QString& error)
{
    qDebug() << "AI Service Error:" << error;
    emit errorOccurred(QString("AI Service Error: %1").arg(error));
}

QString ProblemGenerator::extractProblemFromResponse(const QString& response)
{
    // Try to parse as JSON first
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8(), &parseError);
    
    QString content;
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        
        // Try different possible keys for the response content
        if (obj.contains("content")) {
            content = obj["content"].toString();
        } else if (obj.contains("response")) {
            content = obj["response"].toString();
        } else if (obj.contains("message")) {
            content = obj["message"].toString();
        } else if (obj.contains("text")) {
            content = obj["text"].toString();
        } else if (obj.contains("problem")) {
            content = obj["problem"].toString();
        } else {
            // If no recognized key, use the whole response
            content = response;
        }
    } else {
        // If not JSON, use the response as-is
        content = response;
    }
    
    // Clean up the content
    content = content.trimmed();
    
    // Remove common prefixes that might be added by the LLM
    QStringList prefixesToRemove = {
        "Problem:",
        "Problem Statement:",
        "Here's a problem:",
        "Here is a problem:",
        "Generated problem:",
        "Math problem:"
    };
    
    for (const QString& prefix : prefixesToRemove) {
        if (content.startsWith(prefix, Qt::CaseInsensitive)) {
            content = content.mid(prefix.length()).trimmed();
            break;
        }
    }
    
    // Remove quotes if the entire content is wrapped in them
    if ((content.startsWith('"') && content.endsWith('"')) ||
        (content.startsWith('\'') && content.endsWith('\''))) {
        content = content.mid(1, content.length() - 2);
    }
    
    return content.trimmed();
}

QString ProblemGenerator::generateProblemSync(const QString& problemType, const QString& difficulty)
{
    if (!aiService) {
        return "Error: AI Service not initialized.";
    }
    
    // Create appropriate prompt
    QString prompt = createPrompt(problemType, difficulty);
    
    qDebug() << "Generating problem synchronously for:" << problemType << "at" << difficulty << "difficulty";
    qDebug() << "Prompt:" << prompt;
    
    // Send prompt to AI service and wait for response
    QString response = aiService->promptSync(prompt);
    
    // Extract and clean the problem from the response
    QString cleanedProblem = extractProblemFromResponse(response);
    
    if (cleanedProblem.isEmpty()) {
        return "Error: Received empty or invalid response from AI service.";
    }
    
    return cleanedProblem;
}
