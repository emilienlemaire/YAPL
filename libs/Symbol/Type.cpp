/**
 * libs/Symbol/Type.cpp
 * Copyright (c) 2021 Emilien Lemaire <emilien.lem@icloud.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <exception>
#include <memory>
#include <string>
#include <utility>

#include "Symbol/Type.hpp"
#include "Symbol/PrimitiveType.hpp"
#include "Symbol/ArrayType.hpp"
#include "Symbol/FunctionType.hpp"
#include "Symbol/StructType.hpp"
#include "parallel_hashmap/phmap.h"

namespace yapl {
    phmap::parallel_node_hash_map<Type*, std::shared_ptr<Type>> Type::s_Types =
        phmap::parallel_node_hash_map<Type*, std::shared_ptr<Type>>();

    std::shared_ptr<PrimitiveType> Type::CreatePrimitiveType(bool isNumeric) {
        auto t = PrimitiveType(isNumeric);
        return std::make_shared<PrimitiveType>(t);
    }

    std::shared_ptr<ArrayType> Type::CreateArrayType(Type *elementsType, uint64_t numElements) {
        auto t = ArrayType(elementsType, numElements);
        return std::make_shared<ArrayType>(t);
    }

    std::shared_ptr<FunctionType> Type::CreateFunctionType(Type *returnType, std::vector<Type *> paramsType) {
        auto t = FunctionType(returnType, std::move(paramsType));
        return std::make_shared<FunctionType>(t);
    }

    std::shared_ptr<StructType> Type::CreateStructType(std::string identifier, std::vector<std::string> fieldNames, std::vector<Type *> elementsType) {
        auto t = StructType(std::move(identifier), std::move(fieldNames), std::move(elementsType));
        return std::make_shared<StructType>(t);
    }

} // namespace yapl
