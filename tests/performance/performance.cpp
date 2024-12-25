#include <iostream>
#include <chrono>
#include <cstdlib>

int main() {
    // Start time measurement
    auto start = std::chrono::high_resolution_clock::now();

    // Simulate running the main program by executing the compiled binary
    // Assuming the compiled program is named "program" and is located in the build folder

    char name[] = "./test";

    int result = std::system(name);

    // End time measurement
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Total Execution Time for " << name << ": " << duration.count() << " seconds" << std::endl;

    return 0;
}
