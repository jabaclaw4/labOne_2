#pragma once
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include <cstddef>
#include <utility>
#include <stdexcept>

//DynamicArray<T> динамический массив на основе UnqPtr<T>. Каждый элемент хранится через владение (UnqPtr) разделяемый доступ к элементу выдаётся через ShrdPtr<T>.

template <typename T>
class DynamicArray {
private:
    UnqPtr<T>* items; //сырой массив умных указателей
    size_t count;      //сколько элементов реально добавлено
    size_t capacity;   //сколько места выделено под массив

    //увеличивает вместимость вдвое (или до 4, если массив был пуст)
    //копировать UnqPtr нельзя элементы переносятся через move
    void Grow() {
        size_t newCapacity = (capacity == 0) ? 4 : capacity * 2;
        UnqPtr<T>* newItems = new UnqPtr<T>[newCapacity];

        for (size_t i = 0; i < count; ++i) {
            newItems[i] = std::move(items[i]);
        }

        delete[] items;
        items = newItems;
        capacity = newCapacity;
    }

public:
    DynamicArray() : items(nullptr), count(0), capacity(0) {}

    ~DynamicArray() {
        delete[] items;
    }

    DynamicArray(const DynamicArray&) = delete;//копирование не реализовано
    DynamicArray& operator=(const DynamicArray&) = delete;

    //move для самого контейнера
    DynamicArray(DynamicArray&& other) noexcept
            : items(other.items), count(other.count), capacity(other.capacity) {
        other.items = nullptr;
        other.count = 0;
        other.capacity = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] items;
            items = other.items;
            count = other.count;
            capacity = other.capacity;
            other.items = nullptr;
            other.count = 0;
            other.capacity = 0;
        }
        return *this;
    }

    //добавляет элемент. rawPtr должен быть создан через new владение переходит массиву
    void PushBack(T* rawPtr) {
        if (count == capacity) {
            Grow();
        }
        items[count] = UnqPtr<T>(rawPtr);
        ++count;
    }

    //удаляет последний элемент
    void PopBack() {
        if (count == 0) {
            throw std::out_of_range("DynamicArray: PopBack on empty array");
        }
        --count;
        items[count].reset(); //явно освобождаем объект UnqPtr остаётся пустым
    }

    size_t Size() const noexcept { return count; }
    bool IsEmpty() const noexcept { return count == 0; }

    //прямой доступ к объекту по индексу
    T& Get(size_t index) {
        if (index >= count) {
            throw std::out_of_range("DynamicArray: index out of range");
        }
        return *items[index];
    }

    const T& Get(size_t index) const {
        if (index >= count) {
            throw std::out_of_range("DynamicArray: index out of range");
        }
        return *items[index];
    }

    //разделяемая ссылка на элемент создаёт ShrdPtr ссылающийся на внутренний UnqPtr этого элемента
    ShrdPtr<T> Share(size_t index) {
        if (index >= count) {
            throw std::out_of_range("DynamicArray: index out of range");
        }
        return ShrdPtr<T>(items[index]);
    }

    T& operator[](size_t index) { return Get(index); }
    const T& operator[](size_t index) const { return Get(index); }
};