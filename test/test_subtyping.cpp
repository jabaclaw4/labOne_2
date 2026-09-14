#include "headers/UnqPtr.h"
#include "headers/ShrdPtr.h"
#include <cassert>
#include <iostream>
#include <string>

namespace {
    struct Animal {
        virtual ~Animal() = default;
        virtual std::string Sound() const { return "..."; }
    };

    struct Cat : public Animal {
        std::string Sound() const override { return "Meow"; }
    };
}

void TestUnqPtrSubtyping() {
    UnqPtr<Cat> catPtr(new Cat());
    UnqPtr<Animal> animalPtr = std::move(catPtr);
    assert(animalPtr->Sound() == "Meow");

    std::cout << "  TestUnqPtrSubtyping: OK\n";
}

void TestShrdPtrSubtyping() {
    UnqPtr<Cat> catPtr(new Cat());
    ShrdPtr<Cat> catShrd(catPtr);
    ShrdPtr<Animal> animalShrd = catShrd;

    assert(animalShrd->Sound() == "Meow");
    assert(catShrd.useCount() == 2);

    std::cout << "  TestShrdPtrSubtyping: OK\n";
}

void RunSubtypingTests() {
    std::cout << "Subtyping tests:\n";
    TestUnqPtrSubtyping();
    TestShrdPtrSubtyping();
}