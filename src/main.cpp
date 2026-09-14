#include "headers/UnqPtr.h"
#include "headers/ShrdPtr.h"
#include "headers/DynamicArray.h"
#include <iostream>
#include <string>
#include <limits>

// Функциональные тесты определены в test/*.cpp — здесь только объявления,
// чтобы main.cpp мог их вызвать (реализация подключается на этапе линковки)
void RunUnqPtrTests();
void RunShrdPtrTests();
void RunDynamicArrayTests();
void RunSubtypingTests();

// Нагрузочные тесты определены в test/load_test.cpp
void RunLoadTests();

void RunAllFunctionalTests() {
    std::cout << "\n--- Full functional test suite ---\n";
    RunUnqPtrTests();
    RunShrdPtrTests();
    RunDynamicArrayTests();
    RunSubtypingTests();
    std::cout << "All functional tests passed.\n";
}

// Небольшая живая демонстрация — показывает useCount() в реальном времени,
// удобно для защиты: наглядно видно, как меняется счётчик
void RunLiveDemo() {
    std::cout << "\n--- Live demo: UnqPtr + ShrdPtr ---\n";

    UnqPtr<int> owner(new int(42));
    std::cout << "Created UnqPtr<int> with value 42\n";

    ShrdPtr<int> s1(owner);
    std::cout << "Created ShrdPtr s1, useCount() = " << s1.useCount() << "\n";

    {
        ShrdPtr<int> s2 = s1;
        std::cout << "Copied s1 into s2, useCount() = " << s1.useCount() << "\n";
        std::cout << "*s2 = " << *s2 << "\n";
    }
    std::cout << "s2 went out of scope, useCount() = " << s1.useCount() << "\n";

    std::cout << "Demo finished, owner still owns the value: " << *owner << "\n";
}

void PrintMenu() {
    std::cout << "\n=== Smart Pointers Lab — Console Menu ===\n";
    std::cout << "1. Run full functional test suite\n";
    std::cout << "2. Run load tests (time & memory, saves CSV)\n";
    std::cout << "3. Live demo (UnqPtr + ShrdPtr walkthrough)\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

int main() {
    while (true) {
        PrintMenu();

        int choice;
        if (!(std::cin >> choice)) {
            // некорректный ввод (например, буквы вместо числа)
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input, try again.\n";
            continue;
        }

        switch (choice) {
            case 1: RunAllFunctionalTests(); break;
            case 2: RunLoadTests(); break;
            case 3: RunLiveDemo(); break;
            case 0: std::cout << "Bye.\n"; return 0;
            default: std::cout << "Unknown option.\n"; break;
        }
    }
}