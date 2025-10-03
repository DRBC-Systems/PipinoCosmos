#include <QApplication>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Example.h"   // <-- Include the Example AI test
#include "ProblemGenerator.h"
#include <iostream>


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

    // -----------------------------
    // Run AI tests
    // -----------------------------
    Example example;      // QObject-based, parent is nullptr
    example.askAI("Give me an lineal equation");     // call the AIService tests


    // Generate and print Linear Equations Easy problem with multiple choice
    ProblemGenerator problemGenerator;
    
    std::cout << "\n=== AI Problem Generation Test ===" << std::endl;
    
    // Test Linear Equations Easy with multiple choice
    GeneratedProblem easyProblem = problemGenerator.generateCompleteProblemSync("Linear Equations", "Easy");
    std::cout << "Topic: Linear Equations (Easy)" << std::endl;
    
    if (easyProblem.problemStatement.startsWith("Error:") || easyProblem.problemStatement.startsWith("Timeout:")) {
        std::cout << "Failed: " << easyProblem.problemStatement.toStdString() << std::endl;
    } else {
        std::cout << "Problem: " << easyProblem.problemStatement.toStdString() << std::endl;
        
        if (easyProblem.isMultipleChoice && easyProblem.choices.size() == 4) {
            std::cout << "\nMultiple Choice Options:" << std::endl;
            for (int i = 0; i < easyProblem.choices.size(); ++i) {
                std::cout << easyProblem.choices[i].text.toStdString();
                if (easyProblem.choices[i].isCorrect) {
                    std::cout << " (CORRECT)";
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << "(No multiple choice options generated)" << std::endl;
        }
    }
    
    // std::cout << "\n--- Additional Tests ---" << std::endl;
    
    // // Test Medium difficulty
    // QString mediumProblem = problemGenerator.generateProblemSync("Linear Equations", "Medium");
    // std::cout << "Linear Equations (Medium): " << mediumProblem.toStdString() << std::endl;
    
    // // Test Hard difficulty  
    // QString hardProblem = problemGenerator.generateProblemSync("Linear Equations", "Hard");
    // std::cout << "Linear Equations (Hard): " << hardProblem.toStdString() << std::endl;
    
    // std::cout << "================================\n" << std::endl;

    return app.exec();
}
