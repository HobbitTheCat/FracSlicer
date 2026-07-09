#include "application/application.h"

#include <exception>

int main() {
    try {
        Application app;
        app.Init();
        app.Run();
        app.Cleanup();
    } catch (const std::exception& e) {
        std::cerr << "Critical error during execution: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}