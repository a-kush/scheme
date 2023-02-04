#include <parser.h>
#include <vector>
#include <error.h>

Number::Number(int val) : value_(val) {
}
int Number::GetValue() const {
    return value_;
}

Symbol::Symbol(std::string name) : name_(std::move(name)) {
}
const std::string& Symbol::GetName() const {
    return name_;
}

Cell::Cell() {
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}
std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}
std::shared_ptr<Object>& Cell::GetFirst() {
    return first_;
}
std::shared_ptr<Object>& Cell::GetSecond() {
    return second_;
}

Dot::Dot() {
}

Bool::Bool(std::string b) {
    if (b == "#t") {
        bool_ = true;
    } else {
        bool_ = false;
    }
}
Bool::Bool(bool val) {
    bool_ = val;
}
bool Bool::GetBool() {
    return bool_;
}

std::shared_ptr<Object> ListASTFromVector(std::vector<std::shared_ptr<Object>> list) {
    std::shared_ptr<Object> to_ret = std::make_shared<Cell>();
    if (list.empty()) {
        return to_ret;
    }
    auto cur = to_ret;
    auto prev = cur;
    for (size_t i = 0; i < list.size(); ++i) {
        if (Is<Dot>(list[i])) {
            if (i + 2 != list.size() || i == 0) {
                throw SyntaxError("Wrong Syntax");
            }
            As<Cell>(prev)->GetSecond() = list.back();
            break;
        } else {
            As<Cell>(cur)->GetFirst() = list[i];
            if (i != list.size() - 1) {
                As<Cell>(cur)->GetSecond() = std::make_shared<Cell>();
            }
        }
        prev = cur;
        cur = As<Cell>(cur)->GetSecond();
    }
    return to_ret;
}

std::shared_ptr<Object> ReadImpl(Tokenizer* tokenizer);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    tokenizer->Next();
    std::vector<std::shared_ptr<Object>> list;
    while (true) {
        if (IsCloseBracket(tokenizer->GetToken())) {
            break;
        }
        list.push_back(ReadImpl(tokenizer));
    }
    if (list.empty()) {
        return nullptr;
    }
    return ListASTFromVector(list);
}

std::shared_ptr<Object> ReadImpl(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    std::shared_ptr<Object> to_ret;

    if (IsOpenBracket(token)) {
        to_ret = ReadList(tokenizer);
    } else if (IsConstantToken(token)) {
        to_ret = std::make_shared<Number>(GetConstantTokenValue(token));
    } else if (IsQuoteToken(token)) {
        auto str = "quote";
        auto first = std::make_shared<Symbol>(str);
        to_ret = std::make_shared<Cell>();
        As<Cell>(to_ret)->GetFirst() = first;
        tokenizer->Next();
        auto argument = ReadImpl(tokenizer);
        auto second_cell = std::make_shared<Cell>();
        second_cell->GetFirst() = argument;
        As<Cell>(to_ret)->GetSecond() = second_cell;

        return to_ret;
    } else if (IsSymbolToken(token)) {
        auto str = GetSymbolTokenValue(token);
        if (str == "#t" || str == "#f") {
            to_ret = std::make_shared<Bool>(str);
        } else {
            to_ret = std::make_shared<Symbol>(GetSymbolTokenValue(token));
        }
    } else if (IsDotToken(token)) {
        to_ret = std::make_shared<Dot>();
    } else {
        throw SyntaxError("Wrong Syntax");
    }
    tokenizer->Next();
    return to_ret;
}

#include <iostream>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto to_ret = ReadImpl(tokenizer);
    if (!tokenizer->IsEnd()) {
        throw SyntaxError("Wrong Syntax");
    }
    return to_ret;
}
