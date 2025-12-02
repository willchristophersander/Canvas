/**
 * Canvas - A C++ Drawing Application
 * 
 * Main entry point for the application.
 */

#include "core/Application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused

    Canvas::Application app;

    if (!app.initialize(1280, 720, "Canvas - Drawing Application")) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }

    app.run();
    app.shutdown();

    return 0;
}

