#include "headers/UnqPtr.h"
#include <cassert>
#include <iostream>

void TestUnqPtrBasics() {
    UnqPtr<int> p(new int(42));
    assert(*p == 42);
    assert(p.get() != nullptr);

    UnqPtr<int> p2 = std::move(p);
    assert(p.get() == nullptr);
    assert(*p2 == 42);

    std::cout << "  TestUnqPtrBasics: OK\n";
}

void TestUnqPtrReset() {
    UnqPtr<int> p(new int(1));
    p.reset(new int(2));
    assert(*p == 2);

    p.reset(); // без аргумента — освобождает без замены
    assert(p.get() == nullptr);

    std::cout << "  TestUnqPtrReset: OK\n";
}

void TestUnqPtrRelease() {
    UnqPtr<int> p(new int(7));
    int* raw = p.release();
    assert(p.get() == nullptr);
    assert(*raw == 7);
    delete raw; // владение вернулось нам — сами освобождаем

    std::cout << "  TestUnqPtrRelease: OK\n";
}

void RunUnqPtrTests() {
    std::cout << "UnqPtr tests:\n";
    TestUnqPtrBasics();
    TestUnqPtrReset();
    TestUnqPtrRelease();
}