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

GeneratedProblem ProblemGenerator::generateCompleteProblemSync(const QString& problemType, const QString& difficulty, bool forceMultipleChoice)
{
    if (!aiService) {
        return GeneratedProblem("Error: AI Service not initialized.");
    }
    
    // Determine if we should generate multiple choice based on difficulty or force flag
    bool shouldGenerateMultipleChoice = forceMultipleChoice || 
                                       (difficulty.toLower() == "easy" || difficulty.toLower() == "medium");
    
    QString prompt;
    if (shouldGenerateMultipleChoice) {
        prompt = createMultipleChoicePrompt(problemType, difficulty);
    } else {
        prompt = createPrompt(problemType, difficulty);
    }
    
    qDebug() << "Generating complete problem for:" << problemType << "at" << difficulty << "difficulty";
    qDebug() << "Multiple choice:" << shouldGenerateMultipleChoice;
    qDebug() << "Prompt:" << prompt;
    
    // Send prompt to AI service and wait for response
    QString response = aiService->promptSync(prompt);
    
    if (response.startsWith("Error:") || response.startsWith("Timeout:")) {
        return GeneratedProblem(response);
    }
    
    if (shouldGenerateMultipleChoice) {
        return parseMultipleChoiceResponse(response);
    } else {
        QString cleanedProblem = extractProblemFromResponse(response);
        return GeneratedProblem(cleanedProblem);
    }
}

QString ProblemGenerator::createMultipleChoicePrompt(const QString& problemType, const QString& difficulty)
{
    QString difficultyLower = difficulty.toLower();
    
    QString basePrompt;
    if (difficultyLower == "easy") {
        basePrompt = QString(
            "Generate a simple %1 multiple choice problem suitable for beginners. "
            "Requirements:\n"
            "- Create a clear, easy-to-understand problem statement\n"
            "- Use basic concepts and simple numbers\n"
            "- The problem should be solvable in 1-2 steps\n"
            "- Provide exactly 4 multiple choice options (A, B, C, D)\n"
            "- Only ONE option should be correct\n"
            "- Make the incorrect options plausible but clearly wrong\n"
        ).arg(problemType);
    } else if (difficultyLower == "medium") {
        basePrompt = QString(
            "Generate a %1 multiple choice problem of moderate difficulty. "
            "Requirements:\n"
            "- Create a problem that requires multiple steps to solve\n"
            "- Use intermediate-level concepts and reasonable numbers\n"
            "- Include some context or real-world application if appropriate\n"
            "- Provide exactly 4 multiple choice options (A, B, C, D)\n"
            "- Only ONE option should be correct\n"
            "- Make the incorrect options result from common mistakes\n"
        ).arg(problemType);
    } else {
        // Hard or other - still generate multiple choice if forced
        basePrompt = QString(
            "Generate a challenging %1 multiple choice problem. "
            "Requirements:\n"
            "- Create a complex, multi-step problem\n"
            "- Use advanced concepts and realistic scenarios\n"
            "- Provide exactly 4 multiple choice options (A, B, C, D)\n"
            "- Only ONE option should be correct\n"
            "- Make the incorrect options sophisticated and challenging\n"
        ).arg(problemType);
    }
    
    QString formatInstructions = 
        "\n\nFormat your response EXACTLY like this:\n"
        "PROBLEM: [Your problem statement here]\n"
        "A) [First option]\n"
        "B) [Second option]\n"
        "C) [Third option]\n"
        "D) [Fourth option]\n"
        "CORRECT: [A, B, C, or D]\n\n"
        "Topic: " + problemType + "\n"
        "Difficulty: " + difficulty;
    
    return basePrompt + formatInstructions;
}

GeneratedProblem ProblemGenerator::parseMultipleChoiceResponse(const QString& response)
{
    qDebug() << "Parsing multiple choice response:" << response;
    
    // Clean up the response
    QString cleanResponse = response.trimmed();
    
    // Try to parse JSON first (in case the API returns JSON)
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(cleanResponse.toUtf8(), &parseError);
    
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("content")) {
            cleanResponse = obj["content"].toString();
        } else if (obj.contains("response")) {
            cleanResponse = obj["response"].toString();
        }
    }
    
    // Parse the formatted response
    QStringList lines = cleanResponse.split('\n', Qt::SkipEmptyParts);
    
    QString problemStatement;
    QVector<MultipleChoiceOption> choices;
    QString correctAnswer;
    
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        
        if (trimmedLine.startsWith("PROBLEM:", Qt::CaseInsensitive)) {
            problemStatement = trimmedLine.mid(8).trimmed(); // Remove "PROBLEM:"
        }
        else if (trimmedLine.startsWith("A)", Qt::CaseInsensitive)) {
            choices.append(MultipleChoiceOption(trimmedLine, false));
        }
        else if (trimmedLine.startsWith("B)", Qt::CaseInsensitive)) {
            choices.append(MultipleChoiceOption(trimmedLine, false));
        }
        else if (trimmedLine.startsWith("C)", Qt::CaseInsensitive)) {
            choices.append(MultipleChoiceOption(trimmedLine, false));
        }
        else if (trimmedLine.startsWith("D)", Qt::CaseInsensitive)) {
            choices.append(MultipleChoiceOption(trimmedLine, false));
        }
        else if (trimmedLine.startsWith("CORRECT:", Qt::CaseInsensitive)) {
            correctAnswer = trimmedLine.mid(8).trimmed().toUpper(); // Remove "CORRECT:"
        }
    }
    
    // Mark the correct answer
    if (!correctAnswer.isEmpty() && choices.size() == 4) {
        int correctIndex = -1;
        if (correctAnswer == "A") correctIndex = 0;
        else if (correctAnswer == "B") correctIndex = 1;
        else if (correctAnswer == "C") correctIndex = 2;
        else if (correctAnswer == "D") correctIndex = 3;
        
        if (correctIndex >= 0 && correctIndex < choices.size()) {
            choices[correctIndex].isCorrect = true;
        }
    }
    
    // Validate we have all required parts
    if (problemStatement.isEmpty()) {
        return GeneratedProblem("Error: Could not parse problem statement from AI response.");
    }
    
    if (choices.size() != 4) {
        // If we don't have 4 choices, return as a regular problem
        qDebug() << "Warning: Expected 4 choices, got" << choices.size() << ". Returning as regular problem.";
        return GeneratedProblem(problemStatement);
    }
    
    // Check if we have exactly one correct answer
    int correctCount = 0;
    for (const auto& choice : choices) {
        if (choice.isCorrect) correctCount++;
    }
    
    if (correctCount != 1) {
        qDebug() << "Warning: Expected 1 correct answer, got" << correctCount << ". Setting first option as correct.";
        // Reset all to false and set first as correct
        for (auto& choice : choices) {
            choice.isCorrect = false;
        }
        choices[0].isCorrect = true;
    }
    
    return GeneratedProblem(problemStatement, choices);
}
