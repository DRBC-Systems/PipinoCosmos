#include <QApplication>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Example.h"   // <-- AI test
#include "ProblemGenerator.h"
#include "AIService.h" // <-- Add AIService for OCR
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

    // -----------------------------
    // Run AI text tests
    // -----------------------------
    Example example;      // QObject-based, parent is nullptr
    example.askAI("Give me a linear equation");     // call the AIService tests


    // -----------------------------
    // Generate and print Linear Equations Easy problem
    // -----------------------------
    ProblemGenerator problemGenerator;
    
    std::cout << "\n=== AI Problem Generation Test ===" << std::endl;
    
    QString easyProblem = problemGenerator.generateProblemSync("Linear Equations", "Easy");
    std::cout << "Topic: Linear Equations (Easy)" << std::endl;
    if (easyProblem.startsWith("Error:") || easyProblem.startsWith("Timeout:")) {
        std::cout << "Failed: " << easyProblem.toStdString() << std::endl;
    } else {
        std::cout << "Problem: " << easyProblem.toStdString() << std::endl;
    }

   // -----------------------------
    // OCR Test
    // -----------------------------
    OcrScanner scanner;
    QString imagePath = "/home/user/Desktop/PipinoCosmos/Assets/test.png";  // <-- Your test image
    QString ocrResult = scanner.scanImage(imagePath);

    std::cout << "\n=== OCR Test ===" << std::endl;
    if (ocrResult.isEmpty()) {
        std::cout << "OCR Failed or returned empty string." << std::endl;
    } else {
        std::cout << "OCR Output: " << ocrResult.toStdString() << std::endl;

        // Send OCR text to AI
        AIService ai;
        QString response = ai.promptSync(ocrResult);
        std::cout << "AI Response: " << response.toStdString() << std::endl;
    }
    std::cout << "================================\n" << std::endl;

    return app.exec();
}
