#include "scheme.h"
#include <sstream>
#include <tokenizer.h>
#include <parser.h>
#include <algorithm>
#include "error.h"

void CheckNullptr(std::shared_ptr<Object> ptr) {
    if (ptr == nullptr) {
        throw RuntimeError("Nullptr issue");
    }
}

template <typename T>
void CheckIfValidTypes(std::vector<std::shared_ptr<Object>>& vec) {
    for (auto elem : vec) {
        if (!Is<T>(elem)) {
            throw RuntimeError("Wrong Types");
        }
    }
}

void CheckIfBadArgsCount(std::vector<std::shared_ptr<Object>>& vec, std::vector<size_t> bad_counts,
                         std::vector<size_t> good_counts) {
    if (!bad_counts.empty()) {
        for (auto elem : bad_counts) {
            if (vec.size() == elem) {
                throw RuntimeError("Wrong Types");
            }
        }
    } else {
        bool fl = false;
        for (auto elem : good_counts) {
            if (vec.size() == elem) {
                fl = true;
            }
        }
        if (!fl) {
            throw RuntimeError("Wrong Types");
        }
    }
}

std::vector<std::shared_ptr<Object>> ConvertToVector(std::shared_ptr<Object> cell) {
    std::vector<std::shared_ptr<Object>> to_ret;
    if (cell == nullptr) {
        return to_ret;
    }
    while (true) {
        if (!Is<Cell>(cell)) {
            throw RuntimeError("RE");
        }
        auto ptr_first = As<Cell>(cell)->GetFirst();
        auto ptr_second = As<Cell>(cell)->GetSecond();
        to_ret.push_back(ptr_first);
        if (ptr_second == nullptr) {
            break;
        }
        if (Is<Number>(ptr_second)) {
            to_ret.push_back(ptr_second);
            break;
        }
        cell = ptr_second;
    }
    return to_ret;
}

std::string Interpreter::Run(const std::string& expr) {
    std::stringstream ss(expr);
    Tokenizer tokenizer(&ss);

    auto input_ast = Read(&tokenizer);

    CheckNullptr(input_ast);
    auto output_ast = input_ast->Eval();

    return output_ast->Serialize();
}

std::shared_ptr<Object> Number::Eval() {
    return std::make_shared<Number>(value_);
}
std::string Number::Serialize() {
    return std::to_string(GetValue());
}

std::shared_ptr<Object> Symbol::Eval() {
    return std::make_shared<Function>(name_);
}
std::string Symbol::Serialize() {
    return GetName();
}

std::shared_ptr<Object> Bool::Eval() {
    return std::make_shared<Bool>(GetBool());
}
std::string Bool::Serialize() {
    if (GetBool()) {
        return "#t";
    } else {
        return "#f";
    }
}

std::vector<std::shared_ptr<Object>> EvalVector(std::vector<std::shared_ptr<Object>>& args) {
    std::vector<std::shared_ptr<Object>> to_ret;
    for (auto elem : args) {
        CheckNullptr(elem);
        to_ret.push_back(elem->Eval());
    }
    return to_ret;
}

Function::Function(std::string name) {
    if (name == "quote") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) { return nullptr; };
    } /*NUMBER FUNCTIONS*/ else if (name == "number?") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Bool>(Is<Number>(args.front()));
            return to_ret;
        };
    } else if (name == "=") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Number>(args[i])->GetValue() == As<Number>(args[i + 1])->GetValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == ">") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Number>(args[i])->GetValue() > As<Number>(args[i + 1])->GetValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "<") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Number>(args[i])->GetValue() < As<Number>(args[i + 1])->GetValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "<=") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Number>(args[i])->GetValue() <= As<Number>(args[i + 1])->GetValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == ">=") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Number>(args[i])->GetValue() >= As<Number>(args[i + 1])->GetValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "+") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            int64_t res = 0;
            for (size_t i = 0; i < args.size(); ++i) {
                res += As<Number>(args[i])->GetValue();
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "*") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            std::shared_ptr<Object> to_ret;
            int64_t res = 1;
            for (size_t i = 0; i < args.size(); ++i) {
                res *= As<Number>(args[i])->GetValue();
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "-") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Number>(args[0])->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                res -= As<Number>(args[i])->GetValue();
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "/") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Number>(args[0])->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                res /= As<Number>(args[i])->GetValue();
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "max") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Number>(args[0])->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                int64_t cur_val = As<Number>(args[i])->GetValue();
                res = std::max(res, cur_val);
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "min") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Number>(args[0])->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                int64_t cur_val = As<Number>(args[i])->GetValue();
                res = std::min(res, cur_val);
            }
            to_ret = std::make_shared<Number>(res);
            return to_ret;
        };
    } else if (name == "abs") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Number>(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Number>(std::abs(As<Number>(args.front())->GetValue()));
            return to_ret;
        };
    } /*BOOLEAN FUNCTIONS*/ else if (name == "boolean?") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Bool>(Is<Bool>(args.front()));
            return to_ret;
        };
    } else if (name == "not") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            bool value = false;
            if (Is<Bool>(args.front())) {
                value = !As<Bool>(args.front())->GetBool();
            }
            to_ret = std::make_shared<Bool>(value);
            return to_ret;
        };
    } else if (name == "and") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret = std::make_shared<Bool>(true);
            for (auto elem : args) {
                auto evaluated = elem->Eval();
                if (Is<Bool>(evaluated) && !As<Bool>(evaluated)->GetBool()) {
                    to_ret = evaluated;
                    break;
                }
                to_ret = evaluated;
            }
            return to_ret;
        };
    } else if (name == "or") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret = std::make_shared<Bool>(false);
            for (auto elem : args) {
                auto evaluated = elem->Eval();
                if (Is<Bool>(evaluated) && !As<Bool>(evaluated)->GetBool()) {
                } else {
                    to_ret = evaluated;
                    break;
                }
            }
            return to_ret;
        };
    } /*LIST OPERATIONS*/ else if (name == "pair?") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp = ConvertToVector(As<Cell>(args.front())->GetFirst());
            bool res = false;
            if (temp.size() == 2) {
                res = true;
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "null?") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp = As<Cell>(args.front());
            bool res = false;
            if (temp->GetFirst() == nullptr && temp->GetSecond() == nullptr) {
                res = true;
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "list?") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            bool res = false;
            auto cell = As<Cell>(args.front())->GetFirst();
            if (cell == nullptr) {
                res = true;
            } else {
                while (true) {
                    auto ptr_first = As<Cell>(cell)->GetFirst();
                    auto ptr_second = As<Cell>(cell)->GetSecond();
                    if (ptr_second == nullptr) {
                        res = true;
                        break;
                    }
                    if (Is<Number>(ptr_second)) {
                        break;
                    }
                    cell = ptr_second;
                }
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "cons") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            CheckIfBadArgsCount(args, {}, {2});
            std::shared_ptr<Object> to_ret;
            auto dot = std::make_shared<Dot>();
            args.insert(++args.begin(), dot);
            return ListASTFromVector(args);
        };
    } else if (name == "car") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            args = ConvertToVector(As<Cell>(args.front())->GetFirst());
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret = args.front();
            return to_ret;
        };
    } else if (name == "cdr") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp_vec = ConvertToVector(As<Cell>(args.front())->GetFirst());
            CheckIfBadArgsCount(temp_vec, {0}, {});
            if (temp_vec.size() == 1) {
                to_ret = std::make_shared<Cell>();
                return to_ret;
            }
            if (temp_vec.size() >= 2) {
                auto temp = As<Cell>(As<Cell>(args.front())->GetFirst())->GetSecond();
                if (Is<Number>(temp)) {
                    return temp;
                } else {
                    to_ret = std::make_shared<Cell>();
                    As<Cell>(to_ret)->GetFirst() = temp;
                    return to_ret;
                }
            }
        };
    } else if (name == "list") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret;
            return ListASTFromVector(args);
        };
    } else if (name == "list-ref") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {2});
            if (!Is<Cell>(args.front()) || !Is<Number>(args.back())) {
                throw RuntimeError("Wrong types");
            }
            int ind = As<Number>(args.back())->GetValue();
            auto list_vec = ConvertToVector(As<Cell>(args.front())->GetFirst());
            if (ind >= list_vec.size() || ind < 0) {
                throw RuntimeError("Index error");
            }
            std::shared_ptr<Object> to_ret;
            auto cell = As<Cell>(args.front())->GetFirst();
            auto ans = args.front();
            for (int i = 0; i < ind + 1; ++i) {
                auto ptr_first = As<Cell>(cell)->GetFirst();
                auto ptr_second = As<Cell>(cell)->GetSecond();
                ans = ptr_first;
                cell = ptr_second;
            }
            return ans;
        };
    } else if (name == "list-tail") {
        func_ = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret;
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {2});
            if (!Is<Cell>(args.front()) || !Is<Number>(args.back())) {
                throw RuntimeError("Wrong types");
            }
            int ind = As<Number>(args.back())->GetValue();
            auto list_vec = ConvertToVector(As<Cell>(args.front())->GetFirst());
            if (ind > list_vec.size() || ind < 0) {
                throw RuntimeError("Index error");
            }
            if (ind == list_vec.size()) {
                to_ret = std::make_shared<Cell>();
                return to_ret;
            }
            auto cell = As<Cell>(args.front())->GetFirst();
            auto ans = As<Cell>(args.front())->GetFirst();
            for (int i = 0; i < ind; ++i) {
                auto ptr_first = As<Cell>(cell)->GetFirst();
                auto ptr_second = As<Cell>(cell)->GetSecond();
                cell = ptr_second;
            }
            return cell;
        };
    } else {
        throw RuntimeError("Unknown Function");
    }
}
std::string Function::Serialize() {
    return "";
}
std::shared_ptr<Object> Function::Apply(std::vector<std::shared_ptr<Object>> args) {
    return func_(args);
}
std::shared_ptr<Object> Function::Eval() {
    return nullptr;
}

std::shared_ptr<Object> Cell::Eval() {
    if (!Is<Symbol>(GetFirst())) {
        throw RuntimeError("Wrong Function");
    }
    auto function = GetFirst()->Eval();

    if (As<Symbol>(GetFirst())->GetName() != "quote") {
        auto args = ConvertToVector(GetSecond());
        return As<Function>(function)->Apply(args);
    }
    return GetSecond();
}
std::string Cell::Serialize() {
    std::string first;
    std::string second;
    if (GetFirst()) {
        first = GetFirst()->Serialize();
        if (Is<Cell>(GetFirst()) && first.size() != 2) {
            first = first.substr(1, first.size() - 2);
        }
    }
    if (GetSecond()) {
        second = GetSecond()->Serialize();
        if (Is<Cell>(GetSecond())) {
            second = second.substr(1, second.size() - 2);
        } else {
            second = ". " + second;
        }
        second = " " + second;
    }
    return "(" + first + second + ")";
}

std::shared_ptr<Object> Dot::Eval() {
    throw RuntimeError("Weird, bro!");
    return nullptr;
}
std::string Dot::Serialize() {
    return ".";
}
