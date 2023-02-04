#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <string>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    std::istream* in_;
};

bool IsConstantToken(Token token);

int GetConstantTokenValue(Token token);

bool IsSymbolToken(Token token);

std::string GetSymbolTokenValue(Token token);

bool IsOpenBracket(Token token);

bool IsCloseBracket(Token token);

bool IsDotToken(Token token);

bool IsQuoteToken(Token token);