#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits> // для std::is_convertible_v


// UnqPtr<T> — умный указатель с единоличным владением.
// Копирование запрещено, разрешено только перемещение (move-семантика).
// Хранит size_t* referenceCount, который заводит и обслуживает ShrdPtr<T>,
// чтобы при разрушении проверить отсутствие живых ShrdPtr на этот объект.

template <typename T>
class ShrdPtr; // предварительное объявление, т.к. классы ссылаются друг на друга

template <typename T>
class UnqPtr {
private:
    T* ptr;
    size_t* referenceCount; // адрес книги учёта гостей (пока её нет — nullptr)

    friend class ShrdPtr<T>; // разрешаем ShrdPtr<T> трогать наши private-поля
    template <typename U> friend class UnqPtr; // для конвертирующего конструктора ниже

public:
    UnqPtr(T* p = nullptr) noexcept : ptr(p), referenceCount(nullptr) {}

    ~UnqPtr() {
        // Если сюда попали, а счётчик ещё жив — где-то остался "гость" со старым пропуском.
        // В учебных целях считаем это ошибкой программиста и ловим через assert.
        assert((referenceCount == nullptr || *referenceCount == 0) &&
               "UnqPtr уничтожается, пока есть живые ShrdPtr на него!");
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

    // Конвертирующий move-конструктор: UnqPtr<U> -> UnqPtr<T>,
    // разрешён только если U* приводится к T* (U — наследник T)
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