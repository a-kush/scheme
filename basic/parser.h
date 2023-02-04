#pragma once

#include <memory>

#include "object.h"
#include <tokenizer.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer);

std::shared_ptr<Object> ListASTFromVector(std::vector<std::shared_ptr<Object>> list);