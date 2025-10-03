#include "SolutionGrader.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

SolutionGrader::SolutionGrader(QObject *parent)
    : QObject(parent), aiService(nullptr)
{
    // Initialize AI service
    aiService = new AIService(this);
    
    // Connect AI service signals
    connect(aiService, &AIService::finished, this, &SolutionGrader::handleAIResponse);
    connect(aiService, &AIService::errorOccurred, this, &SolutionGrader::handleAIError);
}

SolutionGrader::~SolutionGrader()
{
    // AI service will be automatically deleted as it's a child of this object
}

QString SolutionGrader::gradeSolution(const QString& userSolution, const QString& problemStatement)
{
    if (!aiService) {
        return "Error: AI Service not initialized.";
    }
    
    // Create grading prompt
    QString prompt = createGradingPrompt(userSolution, problemStatement);
    
    qDebug() << "Grading solution for problem:" << problemStatement;
    qDebug() << "User solution:" << userSolution;
    qDebug() << "Grading prompt:" << prompt;
    
    // Send prompt to AI service and wait for response
    QString response = aiService->promptSync(prompt);
    
    if (response.startsWith("Error:") || response.startsWith("Timeout:")) {
        return response;
    }
    
    // Extract and format the grading feedback
    return extractGradingFeedback(response);
}

void SolutionGrader::gradeSolutionAsync(const QString& userSolution, const QString& problemStatement)
{
    if (!aiService) {
        emit gradingError("AI Service not initialized.");
        return;
    }
    
    // Create grading prompt
    QString prompt = createGradingPrompt(userSolution, problemStatement);
    
    qDebug() << "Grading solution asynchronously for problem:" << problemStatement;
    qDebug() << "User solution:" << userSolution;
    qDebug() << "Grading prompt:" << prompt;
    
    // Send prompt to AI service
    aiService->prompt(prompt);
}

QString SolutionGrader::getDetailedFeedback(const QString& userSolution, const QString& problemStatement)
{
    if (!aiService) {
        return "Error: AI Service not initialized.";
    }
    
    // Create detailed feedback prompt
    QString prompt = createDetailedFeedbackPrompt(userSolution, problemStatement);
    
    qDebug() << "Getting detailed feedback for problem:" << problemStatement;
    qDebug() << "User solution:" << userSolution;
    qDebug() << "Detailed feedback prompt:" << prompt;
    
    // Send prompt to AI service and wait for response
    QString response = aiService->promptSync(prompt);
    
    if (response.startsWith("Error:") || response.startsWith("Timeout:")) {
        return response;
    }
    
    // Extract and format the detailed feedback
    return extractGradingFeedback(response);
}

void SolutionGrader::getDetailedFeedbackAsync(const QString& userSolution, const QString& problemStatement)
{
    if (!aiService) {
        emit gradingError("AI Service not initialized.");
        return;
    }
    
    // Create detailed feedback prompt
    QString prompt = createDetailedFeedbackPrompt(userSolution, problemStatement);
    
    qDebug() << "Getting detailed feedback asynchronously for problem:" << problemStatement;
    qDebug() << "User solution:" << userSolution;
    qDebug() << "Detailed feedback prompt:" << prompt;
    
    // Send prompt to AI service
    aiService->prompt(prompt);
}

QString SolutionGrader::createGradingPrompt(const QString& userSolution, const QString& problemStatement)
{
    QString prompt = QString(
        "You are an expert math tutor. Grade the following student's solution and provide ONLY a simple grade and brief feedback.\n\n"
        "PROBLEM:\n%1\n\n"
        "STUDENT'S SOLUTION:\n%2\n\n"
        "Provide ONLY:\n"
        "🎯 **Grade: [A/B/C/D/F]**\n"
        "[One sentence feedback - just confirm if correct or mention the main issue]\n\n"
        "Examples:\n"
        "- \"🎯 Grade: A - Correct!\"\n"
        "- \"🎯 Grade: C - You used addition instead of multiplication for area.\"\n"
        "- \"🎯 Grade: B - Right answer, but check your arithmetic in step 2.\"\n\n"
        "Keep it very brief. The student can ask for detailed feedback if needed."
    ).arg(problemStatement, userSolution);
    
    return prompt;
}

QString SolutionGrader::createDetailedFeedbackPrompt(const QString& userSolution, const QString& problemStatement)
{
    QString prompt = QString(
        "You are an expert math tutor. The student has asked for detailed feedback on their solution.\n\n"
        "PROBLEM:\n%1\n\n"
        "STUDENT'S SOLUTION:\n%2\n\n"
        "Provide comprehensive feedback in a conversational chat format:\n\n"
        "🎯 **Grade: [A/B/C/D/F]**\n\n"
        "Hi! Let me give you detailed feedback on your solution:\n\n"
        "**What I noticed:**\n"
        "- [Analysis of their approach and what they did well]\n"
        "- [Any errors or misconceptions, explained gently]\n\n"
        "**You did really well with:**\n"
        "- [Specific strengths and correct steps]\n\n"
        "**To make it even better:**\n"
        "- [Constructive suggestions for improvement]\n\n"
        "**Next time, try:**\n"
        "- [Specific tips for similar problems]\n\n"
        "**Keep it up!** [Encouraging, personalized message]\n\n"
        "Be encouraging, educational, and helpful. Focus on learning and growth."
    ).arg(problemStatement, userSolution);
    
    return prompt;
}

QString SolutionGrader::extractGradingFeedback(const QString& response)
{
    // Try to parse as JSON first (in case the API returns JSON)
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
        } else if (obj.contains("feedback")) {
            content = obj["feedback"].toString();
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
        "Feedback:",
        "Grading:",
        "Here's the feedback:",
        "Here is the feedback:",
        "AI Tutor Feedback:",
        "Tutor Response:",
        "System Message:",
        "Human Message:",
        "**System Message:**",
        "**Human Message:**"
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

void SolutionGrader::handleAIResponse(const QString& response)
{
    qDebug() << "AI Grading Response received:" << response;
    
    // Extract the feedback from the response
    QString feedback = extractGradingFeedback(response);
    
    if (feedback.isEmpty()) {
        emit gradingError("Received empty or invalid response from AI service.");
        return;
    }
    
    emit gradingComplete(feedback);
}

void SolutionGrader::handleAIError(const QString& error)
{
    qDebug() << "AI Service Error during grading:" << error;
    emit gradingError(QString("AI Service Error: %1").arg(error));
}
