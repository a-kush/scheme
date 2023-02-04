#pragma once

#include <memory>
#include <string>
#include <memory>
#include <vector>
#include <functional>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::shared_ptr<Object> Eval() = 0;
    virtual std::string Serialize() = 0;
};

class Number : public Object {
public:
    Number(int val);
    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;

    int GetValue() const;

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(std::string name);

    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;

    const std::string& GetName() const;

private:
    std::string name_;
};

class Function : public Object {
public:
    Function(std::string name);

    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;

    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args);

private:
    std::function<std::shared_ptr<Object>(std::vector<std::shared_ptr<Object>>&)> func_;
};

class Bool : public Object {
public:
    Bool(std::string val);
    Bool(bool val);

    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;

    bool GetBool();

private:
    bool bool_;
};

class Dot : public Object {
public:
    Dot();
    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;
};

class Cell : public Object {
public:
    Cell();

    std::shared_ptr<Object> Eval() override;
    std::string Serialize() override;

    std::shared_ptr<Object> GetFirst() const;
    std::shared_ptr<Object> GetSecond() const;
    std::shared_ptr<Object>& GetFirst();
    std::shared_ptr<Object>& GetSecond();

private:
    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto ptr = std::dynamic_pointer_cast<T>(obj);
    if (ptr) {
        return true;
    }
    return false;
}
