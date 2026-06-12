#pragma once
#include <string>
#include <iostream>

extern int testsRun;
extern int testsPassed;

inline void assertTest(const std::string& testName, bool condition) {
    testsRun++;
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
        testsPassed++;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
    }
}
