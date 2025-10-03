#ifndef SOLUTIONGRADER_H
#define SOLUTIONGRADER_H

#include <QObject>
#include <QString>
#include "AIService.h"

/**
 * @brief The SolutionGrader class provides AI-powered grading of user solutions
 * 
 * This class takes a user's solution and the original problem, then uses AI
 * to grade the solution and provide detailed feedback in a chat format.
 */
class SolutionGrader : public QObject
{
    Q_OBJECT

public:
    explicit SolutionGrader(QObject *parent = nullptr);
    ~SolutionGrader();
    
    /**
     * @brief Grade a user's solution against a problem
     * @param userSolution The user's submitted solution
     * @param problemStatement The original problem statement
     * @return QString containing the graded feedback in chat format
     */
    QString gradeSolution(const QString& userSolution, const QString& problemStatement);
    
    /**
     * @brief Grade a user's solution asynchronously
     * @param userSolution The user's submitted solution
     * @param problemStatement The original problem statement
     */
    void gradeSolutionAsync(const QString& userSolution, const QString& problemStatement);
    
    /**
     * @brief Get detailed feedback for a previously graded solution
     * @param userSolution The user's submitted solution
     * @param problemStatement The original problem statement
     * @return QString containing detailed feedback in chat format
     */
    QString getDetailedFeedback(const QString& userSolution, const QString& problemStatement);
    
    /**
     * @brief Get detailed feedback asynchronously
     * @param userSolution The user's submitted solution
     * @param problemStatement The original problem statement
     */
    void getDetailedFeedbackAsync(const QString& userSolution, const QString& problemStatement);

signals:
    /**
     * @brief Emitted when grading is complete
     * @param feedback The graded feedback in chat format
     */
    void gradingComplete(const QString& feedback);
    
    /**
     * @brief Emitted when an error occurs during grading
     * @param error The error message
     */
    void gradingError(const QString& error);

private slots:
    void handleAIResponse(const QString& response);
    void handleAIError(const QString& error);

private:
    AIService* aiService;
    
    /**
     * @brief Create a grading prompt for the AI
     * @param userSolution The user's solution
     * @param problemStatement The original problem
     * @return The formatted prompt for the AI
     */
    QString createGradingPrompt(const QString& userSolution, const QString& problemStatement);
    
    /**
     * @brief Create a detailed feedback prompt for the AI
     * @param userSolution The user's solution
     * @param problemStatement The original problem
     * @return The formatted detailed feedback prompt for the AI
     */
    QString createDetailedFeedbackPrompt(const QString& userSolution, const QString& problemStatement);
    
    /**
     * @brief Extract and format the grading response from AI
     * @param response The raw AI response
     * @return The formatted feedback
     */
    QString extractGradingFeedback(const QString& response);
};

#endif // SOLUTIONGRADER_H
