template <typename T>
class UniquePtr {
private:
    T* ptr;
public:
    Unique(T* p = nullptr) : ptr(p) {}
    Unique() { delete ptr; }//деструктор

    UniquePtr(const UniquePtr&) = delete;//запрет копирования конструктор удален
    UniquePtr& operator=(const UniquePtr&) = delete;//запрет коп-я (присваивания)

    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr; //передаем другому указателю
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {//проверка на самоприсваивание
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const {return ptr; }
    T* get() const { return ptr; } //метод обычный указатель

    T* release() {//забрать объект из умного без удаления
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) {//замена объекта
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }
};

























