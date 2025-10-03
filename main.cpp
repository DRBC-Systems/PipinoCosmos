#include <QApplication>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Example.h"   // <-- AI test
// #include "ProblemGenerator.h"  // Commented out - not used in current build
#include "AIService.h" // <-- Add AIService for OCR
#include "SolutionGrader.h"
#include <iostream>
#include <QDebug>
#include "OcrScanner.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create MVC components
    Model model;
    View view;
    Controller controller;
    
    // Set up MVC connections
    controller.setModel(&model);
    controller.setView(&view);
    
    // Initialize the application
    controller.initializeApplication();
    
    // Show the main window
    view.show();

    // // -----------------------------
    // // Run AI tests
    // // -----------------------------
    // Example example;      // QObject-based, parent is nullptr
    // example.askAI("Give me an lineal equation");     // call the AIService tests


    // // Generate and print Linear Equations Easy problem with multiple choice
    // ProblemGenerator problemGenerator;
    
    // std::cout << "\n=== AI Problem Generation Test ===" << std::endl;
    
    // // Test Linear Equations Easy with multiple choice
    // GeneratedProblem easyProblem = problemGenerator.generateCompleteProblemSync("Linear Equations", "Easy");
    // std::cout << "Topic: Linear Equations (Easy)" << std::endl;
    
    // if (easyProblem.problemStatement.startsWith("Error:") || easyProblem.problemStatement.startsWith("Timeout:")) {
    //     std::cout << "Failed: " << easyProblem.problemStatement.toStdString() << std::endl;
    // } else {
    //     std::cout << "Problem: " << easyProblem.problemStatement.toStdString() << std::endl;
        
    //     if (easyProblem.isMultipleChoice && easyProblem.choices.size() == 4) {
    //         std::cout << "\nMultiple Choice Options:" << std::endl;
    //         for (int i = 0; i < easyProblem.choices.size(); ++i) {
    //             std::cout << easyProblem.choices[i].text.toStdString();
    //             if (easyProblem.choices[i].isCorrect) {
    //                 std::cout << " (CORRECT)";
    //             }
    //             std::cout << std::endl;
    //         }
    //     } else {
    //         std::cout << "(No multiple choice options generated)" << std::endl;
    //     }
    // }
    
    // -----------------------------
    // Test Solution Grading
    // -----------------------------
    std::cout << "\n=== AI Solution Grading Test ===" << std::endl;
    
    SolutionGrader solutionGrader;
    
    // Test grading a correct solution (should be short feedback)
    QString testProblem = "Solve for x: 2x + 5 = 13";
    QString testSolution = "2x + 5 = 13\n2x = 13 - 5\n2x = 8\nx = 4";
    
    // Test with an incorrect solution (should be detailed feedback)
    QString testProblem2 = "Find the area of a rectangle with length 8 cm and width 5 cm";
    QString testSolution2 = "Area = length + width\nArea = 8 + 5\nArea = 13 cm";
    
    // Test with a partially correct solution
    QString testProblem3 = "Calculate: 15 + 8 - 3";
    QString testSolution3 = "15 + 8 = 23\n23 - 3 = 20";
    
    std::cout << "Problem: " << testProblem.toStdString() << std::endl;
    std::cout << "Student Solution: " << testSolution.toStdString() << std::endl;
    std::cout << "\n🤖 AI Tutor: Grading solution..." << std::endl;
    
    QString gradingFeedback = solutionGrader.gradeSolution(testSolution, testProblem);
    
    if (gradingFeedback.startsWith("Error:") || gradingFeedback.startsWith("Timeout:")) {
        std::cout << "❌ Grading failed: " << gradingFeedback.toStdString() << std::endl;
    } else {
        std::cout << "🤖 AI Tutor: " << gradingFeedback.toStdString() << std::endl;
        
        // Simulate student asking for more details
        std::cout << "\n👤 Student: Can you explain more?" << std::endl;
        std::cout << "🤖 AI Tutor: Getting detailed feedback..." << std::endl;
        
        QString detailedFeedback = solutionGrader.getDetailedFeedback(testSolution, testProblem);
        std::cout << "🤖 AI Tutor: " << detailedFeedback.toStdString() << std::endl;
    }
    
    // Test with incorrect solution
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Problem: " << testProblem2.toStdString() << std::endl;
    std::cout << "Student Solution: " << testSolution2.toStdString() << std::endl;
    std::cout << "\n🤖 AI Tutor: Grading solution..." << std::endl;
    
    QString gradingFeedback2 = solutionGrader.gradeSolution(testSolution2, testProblem2);
    
    if (gradingFeedback2.startsWith("Error:") || gradingFeedback2.startsWith("Timeout:")) {
        std::cout << "❌ Grading failed: " << gradingFeedback2.toStdString() << std::endl;
    } else {
        std::cout << "🤖 AI Tutor: " << gradingFeedback2.toStdString() << std::endl;
        
        // Simulate student asking for help
        std::cout << "\n👤 Student: I don't understand. Can you help me?" << std::endl;
        std::cout << "🤖 AI Tutor: Getting detailed feedback..." << std::endl;
        
        QString detailedFeedback2 = solutionGrader.getDetailedFeedback(testSolution2, testProblem2);
        std::cout << "🤖 AI Tutor: " << detailedFeedback2.toStdString() << std::endl;
    }
    
    // Test with partially correct solution
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Problem: " << testProblem3.toStdString() << std::endl;
    std::cout << "Student Solution: " << testSolution3.toStdString() << std::endl;
    std::cout << "\n🤖 AI Tutor: Grading solution..." << std::endl;
    
    QString gradingFeedback3 = solutionGrader.gradeSolution(testSolution3, testProblem3);
    
    if (gradingFeedback3.startsWith("Error:") || gradingFeedback3.startsWith("Timeout:")) {
        std::cout << "❌ Grading failed: " << gradingFeedback3.toStdString() << std::endl;
    } else {
        std::cout << "🤖 AI Tutor: " << gradingFeedback3.toStdString() << std::endl;
        
        // Simulate student asking for clarification
        std::cout << "\n👤 Student: What did I do wrong?" << std::endl;
        std::cout << "🤖 AI Tutor: Getting detailed feedback..." << std::endl;
        
        QString detailedFeedback3 = solutionGrader.getDetailedFeedback(testSolution3, testProblem3);
        std::cout << "🤖 AI Tutor: " << detailedFeedback3.toStdString() << std::endl;
    }
  
  // -----------------------------
    // OCR Test
    // -----------------------------
    OcrScanner scanner;
    QString imagePath = "Assets/test.png";  // <-- Your test image
    QString ocrResult = scanner.scanImage(imagePath);

    // Feed `ocrText` directly to the AI service or store it in the model
    AIService ai;
    QString aiResponse = ai.promptSync(ocrText);

    std::cout << "\n=== OCR Test ===" << std::endl;
    std::cout << "Extracted Text: " << ocrText.toStdString() << std::endl;
    std::cout << "AI Response: " << aiResponse.toStdString() << std::endl;
    std::cout << "================================\n" << std::endl;
    std::cout << "================================\n" << std::endl;

    return app.exec();
}
