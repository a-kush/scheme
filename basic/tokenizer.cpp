#include <tokenizer.h>
#include <error.h>

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}
bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
}

void KillSpaces(std::istream* in) {
    while (in->peek() == ' ' || in->peek() == '\n') {  // get rid of spaces
        in->get();
    }
}

bool Tokenizer::IsEnd() {
    KillSpaces(in_);
    return in_->peek() == EOF;
}

bool IsConstantToken(Token token) {
    return std::holds_alternative<ConstantToken>(token);
}

bool IsSymbolToken(Token token) {
    return std::holds_alternative<SymbolToken>(token);
}

int GetConstantTokenValue(Token token) {
    return std::get<ConstantToken>(token).value;
}

std::string GetSymbolTokenValue(Token token) {
    return std::get<SymbolToken>(token).name;
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

void Tokenizer::Next() {
    if (IsEnd()) {
        return;
    }
    Token cur = GetToken();
    size_t cnt_move = 1;
    if (IsConstantToken(cur)) {
        std::string str = std::to_string(GetConstantTokenValue(cur));
        cnt_move = str.size();
    } else if (IsSymbolToken(cur)) {
        std::string str = GetSymbolTokenValue(cur);
        cnt_move = str.size();
    }
    while (cnt_move) {
        in_->get();
        --cnt_move;
    }
    if (IsConstantToken(cur) && IsDigit(in_->peek())) {
        in_->get();
    }
    if (!IsEnd()) {
        GetToken();
    }
}

void PutBack(std::string& did_read, std::istream* in) {
    while (!did_read.empty()) {
        in->putback(did_read.back());
        did_read.pop_back();
    }
}

ConstantToken ReadConstantToken(std::string& did_read, std::istream* in) {
    while (IsDigit(in->peek())) {
        did_read.push_back(in->get());
    }
    ConstantToken ret{std::stoi(did_read)};
    PutBack(did_read, in);
    return ret;
}

bool IsSymbolStart(char c) {
    static std::string symbols = "<=>*/#";
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (symbols.find(c) != std::string::npos);
}

bool IsSymbolMid(char c) {
    return IsSymbolStart(c) || IsDigit(c) || c == '!' || c == '-' || c == '?';
}

SymbolToken ReadSymbolToken(std::string& did_read, std::istream* in) {
    while (IsSymbolMid(in->peek())) {
        did_read.push_back(in->get());
    }
    SymbolToken ret{did_read};
    PutBack(did_read, in);
    return ret;
}

Token Tokenizer::GetToken() {
    KillSpaces(in_);

    std::string did_read;
    char first_char = in_->peek();
    if (first_char == '(') {
        return BracketToken::OPEN;
    } else if (first_char == ')') {
        return BracketToken::CLOSE;
    } else if (first_char == '\'') {
        return QuoteToken();
    } else if (first_char == '.') {
        return DotToken();
    } else if (IsDigit(first_char)) {
        return ReadConstantToken(did_read, in_);
    } else if (first_char == '+' || first_char == '-') {
        did_read.push_back(in_->get());
        if (IsDigit(in_->peek())) {
            return ReadConstantToken(did_read, in_);
        } else {
            PutBack(did_read, in_);
            return SymbolToken{std::string(1, first_char)};
        }
    } else if (IsSymbolStart(first_char)) {
        return ReadSymbolToken(did_read, in_);
    } else {
        throw SyntaxError("Wrong Syntax");
    }
}

bool IsOpenBracket(Token token) {
    if (std::holds_alternative<BracketToken>(token)) {
        return std::get<BracketToken>(token) == BracketToken::OPEN;
    }
    return false;
}

bool IsCloseBracket(Token token) {
    if (std::holds_alternative<BracketToken>(token)) {
        return std::get<BracketToken>(token) == BracketToken::CLOSE;
    }
    return false;
}

bool IsDotToken(Token token) {
    return std::holds_alternative<DotToken>(token);
}

bool IsQuoteToken(Token token) {
    return std::holds_alternative<QuoteToken>(token);
}