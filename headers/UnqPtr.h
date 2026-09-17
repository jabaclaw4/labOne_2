#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits> // для std::is_convertible_v


//UnqPtr<T> умный указатель с единоличным владением копирование запрещено разрешено только перемещение
//size_t* referenceCount чтобы при разрушении проверить отсутствие живых ShrdPtr на этот объект

template <typename T>
class ShrdPtr; //классы ссылаются друг на друга

template <typename T>
class UnqPtr {
private:
    T* ptr;
    size_t* referenceCount;

    friend class ShrdPtr<T>; //разрешаем ShrdPtr<T> трогать наши private-поля
    template <typename U> friend class UnqPtr;

public:
    UnqPtr(T* p = nullptr) noexcept : ptr(p), referenceCount(nullptr) {}

    ~UnqPtr() {
        // В учебных целях считаем это ошибкой программиста и ловим через assert.
        assert((referenceCount == nullptr || *referenceCount == 0) &&
               "UnqPtr is destroyed as long as there are live ShrdPtrs on it!");
        delete ptr;
    }

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept
            : ptr(other.ptr), referenceCount(other.referenceCount) {
        other.ptr = nullptr;
        other.referenceCount = nullptr;
    }

    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            referenceCount = other.referenceCount;
            other.ptr = nullptr;
            other.referenceCount = nullptr;
        }
        return *this;
    }

    //конвертирующий move-конструктор: UnqPtr<U> -> UnqPtr<T> разрешён только если U наследник T
    template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    UnqPtr(UnqPtr<U>&& other) noexcept
            : ptr(other.ptr), referenceCount(other.referenceCount) {
        other.ptr = nullptr;
        other.referenceCount = nullptr;
    }

    T& operator*()  const noexcept { assert(ptr); return *ptr; }
    T* operator->() const noexcept { assert(ptr); return ptr; }
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