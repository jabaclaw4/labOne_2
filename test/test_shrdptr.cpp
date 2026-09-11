#include "../UnqPtr.h"
#include "../ShrdPtr.h"
#include <cassert>
#include <iostream>

void TestShrdPtrRefCounting() {
    UnqPtr<int> owner(new int(100));
    {
        ShrdPtr<int> s1(owner);
        assert(s1.useCount() == 1);
        {
            ShrdPtr<int> s2 = s1;
            assert(s1.useCount() == 2);
            assert(*s2 == 100);
        }
        assert(s1.useCount() == 1);
    }
    assert(*owner == 100); // объект всё ещё жив — им владеет owner

    std::cout << "  TestShrdPtrRefCounting: OK\n";
}

void TestShrdPtrMove() {
    UnqPtr<int> owner(new int(5));
    ShrdPtr<int> s1(owner);
    assert(s1.useCount() == 1);

    ShrdPtr<int> s2 = std::move(s1);
    assert(s2.useCount() == 1); // count не изменился, просто переехал
    assert(*s2 == 5);

    std::cout << "  TestShrdPtrMove: OK\n";
}

void TestShrdPtrEmptyByDefault() {
    ShrdPtr<int> s; // пустой
    assert(s.useCount() == 0);
    assert(s.get() == nullptr);
    assert(static_cast<bool>(s) == false);

    std::cout << "  TestShrdPtrEmptyByDefault: OK\n";
}

void RunShrdPtrTests() {
    std::cout << "ShrdPtr tests:\n";
    TestShrdPtrRefCounting();
    TestShrdPtrMove();
    TestShrdPtrEmptyByDefault();
}