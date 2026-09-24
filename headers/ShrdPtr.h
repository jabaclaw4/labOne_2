#pragma once
#include "UnqPtr.h"
#include <type_traits> // ДОБАВЛЕНО

// ShrdPtr<T> умный указатель с разделяемым владением по условию задания не хранит T* напрямую а ссылается на UnqPtr<T>,
//подсчёт ссылок ведётся полем size_t* referenceCount которое ShrdPtr создаёт при первом обращении к UnqPtr и удаляет, когда счётчик доходит до нуля.
//сам управляемый объект ShrdPtr не удаляет это ответственность UnqPtr так как он управляет объектом
//ownsMaster  true если UnqPtr был создан самим ShrdPtr (конструктор от T*) тогда его тоже нужно удалить при обнулении счётчика
template <typename T>
class ShrdPtr {
private:
    UnqPtr<T>* master;
    size_t* referenceCount;
    bool ownsMaster;

    template <typename U> friend class ShrdPtr;

    void detach() noexcept {
        if (referenceCount == nullptr) return;
        --(*referenceCount);
        if (*referenceCount == 0) {
            delete referenceCount;
            if (ownsMaster) {
                delete master; //сами создали UnqPtr в конструкторе от T* сами и освобождаем
            }
        }
    }

public:
    ShrdPtr() noexcept : master(nullptr), referenceCount(nullptr), ownsMaster(false) {}

    explicit ShrdPtr(UnqPtr<T>& owner)
            : master(&owner), referenceCount(new size_t(1)), ownsMaster(false) {}

    //создаёт ShrdPtr напрямую от сырого указателя без ручного объявления UnqPtr заранее
    explicit ShrdPtr(T* rawPtr)
            : master(new UnqPtr<T>(rawPtr)), referenceCount(new size_t(1)), ownsMaster(true) {}

    ShrdPtr(const ShrdPtr& other) noexcept
            : master(other.master), referenceCount(other.referenceCount), ownsMaster(other.ownsMaster) {
        if (referenceCount != nullptr) {
            ++(*referenceCount);
        }
    }

    ShrdPtr& operator=(const ShrdPtr& other) noexcept {
        if (this != &other) {
            detach();
            master = other.master;
            referenceCount = other.referenceCount;
            ownsMaster = other.ownsMaster;
            if (referenceCount != nullptr) {
                ++(*referenceCount);
            }
        }
        return *this;
    }

    ShrdPtr(ShrdPtr&& other) noexcept
            : master(other.master), referenceCount(other.referenceCount), ownsMaster(other.ownsMaster) {
        other.master = nullptr;
        other.referenceCount = nullptr;
        other.ownsMaster = false;
    }

    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) {
            detach();
            master = other.master;
            referenceCount = other.referenceCount;
            ownsMaster = other.ownsMaster;
            other.master = nullptr;
            other.referenceCount = nullptr;
            other.ownsMaster = false;
        }
        return *this;
    }

    //конвертирующий copy-конструктор для подтипизации.
    //ShrdPtr<U> -> ShrdPtr<T> разрешён только если U* приводится к T*
    template <typename U>
    requires std::is_convertible_v<U*, T*>
    ShrdPtr(const ShrdPtr<U>& other) noexcept
            : master(reinterpret_cast<UnqPtr<T>*>(other.master)),
              referenceCount(other.referenceCount), ownsMaster(other.ownsMaster) {
        if (referenceCount != nullptr) {
            ++(*referenceCount);
        }
    }

    ~ShrdPtr() {
        detach();
    }

    T& operator*()  const { return *master->get(); } //без noexcept: доступ к пустому объекту не гарантируем
    T* operator->() const noexcept { return master->get(); }
    T* get()         const noexcept { return master != nullptr ? master->get() : nullptr; }
    size_t useCount() const noexcept { return referenceCount != nullptr ? *referenceCount : 0; }

    explicit operator bool() const noexcept {
        return master != nullptr && master->get() != nullptr;
    }
};