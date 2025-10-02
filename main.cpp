#include <QApplication>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Example.h"   // <-- Include the Example AI test

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
    example.testAI();     // call the AIService tests

    return app.exec();
}
