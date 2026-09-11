#include "headers/UnqPtr.h"
#include "headers/ShrdPtr.h"
#include "headers/DynamicArray.h"
#include <iostream>
#include <cassert>
#include <string>
#include <limits>

//нагрузочные тесты + графики

// Простая иерархия классов для проверки подтипизации
struct Animal {
    virtual ~Animal() = default;
    virtual std::string Sound() const { return "..."; }
};

struct Cat : public Animal {
    std::string Sound() const override { return "Meow"; }
};

void TestUnqPtrBasics() {
    UnqPtr<int> p(new int(42));
    assert(*p == 42);
    assert(p.get() != nullptr);

    UnqPtr<int> p2 = std::move(p);
    assert(p.get() == nullptr);
    assert(*p2 == 42);

    std::cout << "TestUnqPtrBasics: OK\n";
}

void TestShrdPtrRefCounting() {
    UnqPtr<int> owner(new int(100));
    {
        ShrdPtr<int> s1(owner);
        assert(s1.useCount() == 1);
        {
            ShrdPtr<int> s2 = s1;
            assert(s1.useCount() == 2);
            assert(s2.useCount() == 2);
            assert(*s2 == 100);
        }
        assert(s1.useCount() == 1);
    }
    assert(*owner == 100);

    std::cout << "TestShrdPtrRefCounting: OK\n";
}

void TestSubtyping() {
    UnqPtr<Cat> catPtr(new Cat());
    UnqPtr<Animal> animalPtr = std::move(catPtr);
    assert(animalPtr->Sound() == "Meow");

    UnqPtr<Cat> catPtr2(new Cat());
    ShrdPtr<Cat> catShrd(catPtr2);
    ShrdPtr<Animal> animalShrd = catShrd;
    assert(animalShrd->Sound() == "Meow");
    assert(catShrd.useCount() == 2);

    std::cout << "TestSubtyping: OK\n";
}

void TestDynamicArray() {
    DynamicArray<int> arr;
    assert(arr.IsEmpty());

    for (int i = 0; i < 10; ++i) {
        arr.PushBack(new int(i * i));
    }
    assert(arr.Size() == 10);
    assert(arr.Get(3) == 9);
    assert(arr[5] == 25);

    ShrdPtr<int> shared = arr.Share(7);
    assert(*shared == 49);
    assert(shared.useCount() == 1);

    arr.PopBack();
    assert(arr.Size() == 9);

    std::cout << "TestDynamicArray: OK\n";
}

// Дополнительные функциональные тесты из отдельных файлов tests/*.cpp
void RunUnqPtrTests();
void RunShrdPtrTests();
void RunDynamicArrayTests();
void RunSubtypingTests();

// Нагрузочные тесты из tests/load_test.cpp
void RunLoadTests();

void RunSmokeTests() {
    std::cout << "\n--- Smoke tests ---\n";
    TestUnqPtrBasics();
    TestShrdPtrRefCounting();
    TestSubtyping();
    TestDynamicArray();
    std::cout << "All smoke tests passed.\n";
}

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
    std::cout << "1. Run smoke tests (quick check)\n";
    std::cout << "2. Run full functional test suite\n";
    std::cout << "3. Run load tests (time & memory, saves CSV)\n";
    std::cout << "4. Live demo (UnqPtr + ShrdPtr walkthrough)\n";
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
            case 1: RunSmokeTests(); break;
            case 2: RunAllFunctionalTests(); break;
            case 3: RunLoadTests(); break;
            case 4: RunLiveDemo(); break;
            case 0: std::cout << "Bye.\n"; return 0;
            default: std::cout << "Unknown option.\n"; break;
        }
    }
}