#pragma once
#include <cstddef>
#include <type_traits> // для std::is_convertible_v


//UnqPtr<T> умный указатель с единоличным владением копирование запрещено разрешено только перемещение
//счётчиком ссылок теперь полностью управляет ShrdPtr UnqPtr о нём не знает

template <typename T>
class ShrdPtr; //классы ссылаются друг на друга

template <typename T>
class UnqPtr {
private:
    T* ptr;

    template <typename U> friend class UnqPtr;

public:
    UnqPtr(T* p = nullptr) noexcept : ptr(p) {}

    ~UnqPtr() {
        delete ptr;
    }

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept
            : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    //конвертирующий move-конструктор: UnqPtr<U> -> UnqPtr<T> разрешён только если U наследник T
    template <typename U>
    requires std::is_convertible_v<U*, T*>
    UnqPtr(UnqPtr<U>&& other) noexcept
            : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    T& operator*()  const { return *ptr; } //без noexcept: разыменование пустого указателя не гарантируем как безопасное
    T* operator->() const noexcept { return ptr; }
    T* get()        const noexcept { return ptr; }

    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }

    explicit operator bool() const noexcept { return ptr != nullptr; }
};