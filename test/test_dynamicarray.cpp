#include "headers/DynamicArray.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

void TestDynamicArrayPushAndAccess() {
    DynamicArray<int> arr;
    assert(arr.IsEmpty());

    for (int i = 0; i < 10; ++i) {
        arr.PushBack(new int(i * i));
    }
    assert(arr.Size() == 10);
    assert(arr.Get(3) == 9);
    assert(arr[5] == 25);

    std::cout << "  TestDynamicArrayPushAndAccess: OK\n";
}

void TestDynamicArrayShare() {
    DynamicArray<int> arr;
    arr.PushBack(new int(49));

    ShrdPtr<int> shared = arr.Share(0);
    assert(*shared == 49);
    assert(shared.useCount() == 1);

    std::cout << "  TestDynamicArrayShare: OK\n";
}

void TestDynamicArrayOutOfRange() {
    DynamicArray<int> arr;
    arr.PushBack(new int(1));

    bool caught = false;
    try {
        arr.Get(5); // индекс за пределами
    } catch (const std::out_of_range&) {
        caught = true;
    }
    assert(caught);

    std::cout << "  TestDynamicArrayOutOfRange: OK\n";
}

void TestDynamicArrayPopBack() {
    DynamicArray<int> arr;
    arr.PushBack(new int(1));
    arr.PushBack(new int(2));

    arr.PopBack();
    assert(arr.Size() == 1);
    assert(arr[0] == 1);

    std::cout << "  TestDynamicArrayPopBack: OK\n";
}

void RunDynamicArrayTests() {
    std::cout << "DynamicArray tests:\n";
    TestDynamicArrayPushAndAccess();
    TestDynamicArrayShare();
    TestDynamicArrayOutOfRange();
    TestDynamicArrayPopBack();
}