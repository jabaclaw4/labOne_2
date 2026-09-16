#pragma once
#include "UnqPtr.h"
#include <type_traits> // ДОБАВЛЕНО

// ShrdPtr<T> — умный указатель с разделяемым владением.
// По условию задания не хранит T* напрямую, а ссылается на UnqPtr<T>,
// через который и получает доступ к объекту.
// Подсчёт ссылок ведётся полем size_t* referenceCount, которое ShrdPtr
// создаёт при первом обращении к UnqPtr и удаляет, когда счётчик доходит до нуля.
// Сам управляемый объект ShrdPtr не удаляет — это ответственность UnqPtr.
template <typename T>
class ShrdPtr {
private:
    UnqPtr<T>* master;
    size_t* referenceCount;

    template <typename U> friend class ShrdPtr; // ДОБАВЛЕНО: доступ между ShrdPtr<U> и ShrdPtr<T>

    void detach() noexcept {
        if (referenceCount == nullptr) return;
        --(*referenceCount);
        if (*referenceCount == 0) {
            delete referenceCount;
            if (master != nullptr) {
                master->referenceCount = nullptr;
            }
        }
    }

public:
    ShrdPtr() noexcept : master(nullptr), referenceCount(nullptr) {}

    explicit ShrdPtr(UnqPtr<T>& owner) {
        master = &owner;
        if (owner.referenceCount == nullptr) {
            owner.referenceCount = new size_t(0);
        }
        referenceCount = owner.referenceCount;
        ++(*referenceCount);
    }

    ShrdPtr(const ShrdPtr& other) noexcept
            : master(other.master), referenceCount(other.referenceCount) {
        if (referenceCount != nullptr) {
            ++(*referenceCount);
        }
    }

    ShrdPtr& operator=(const ShrdPtr& other) noexcept {
        if (this != &other) {
            detach();
            master = other.master;
            referenceCount = other.referenceCount;
            if (referenceCount != nullptr) {
                ++(*referenceCount);
            }
        }
        return *this;
    }

    ShrdPtr(ShrdPtr&& other) noexcept
            : master(other.master), referenceCount(other.referenceCount) {
        other.master = nullptr;
        other.referenceCount = nullptr;
    }

    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) {
            detach();
            master = other.master;
            referenceCount = other.referenceCount;
            other.master = nullptr;
            other.referenceCount = nullptr;
        }
        return *this;
    }

    // ДОБАВЛЕНО: конвертирующий copy-конструктор для подтипизации.
    // ShrdPtr<U> -> ShrdPtr<T>, разрешён только если U* приводится к T*.
    template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    ShrdPtr(const ShrdPtr<U>& other) noexcept
            : master(reinterpret_cast<UnqPtr<T>*>(other.master)),
            referenceCount(other.referenceCount) {
        if (referenceCount != nullptr) {
            ++(*referenceCount);
        }
    }

    ~ShrdPtr() {
        detach();
    }

    T& operator*()  const noexcept { assert(master != nullptr); return *master->get(); }
    T* operator->() const noexcept { assert(master != nullptr); return master->get(); }
    T* get()         const noexcept { return master != nullptr ? master->get() : nullptr; }
    size_t useCount() const noexcept { return referenceCount != nullptr ? *referenceCount : 0; }

    explicit operator bool() const noexcept {
        return master != nullptr && master->get() != nullptr;
    }
};