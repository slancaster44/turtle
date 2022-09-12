#include <iostream>
#include <string>

void failIfNotSuccess(int exitStatus, std::string msg) {
    if (exitStatus != 0) {
        std::cout << exitStatus << " FAIL: " << msg << std::endl;
        exit(exitStatus);
    }

    std::cout << "\tPASS: " << msg << std::endl;
}