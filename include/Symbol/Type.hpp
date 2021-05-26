/**
 * include/Symbol/Type.hpp
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
#pragma once

#include "parallel_hashmap/phmap.h"
#include "parallel_hashmap/phmap_fwd_decl.h"
#include <memory>
#include <string>
#include <vector>

namespace yapl {
    class PrimitiveType;
    class ArrayType;
    class FunctionType;
    class StructType;
    // TODO(feature): Add pointer types
    // TODO(feature): Add vector types

    // TODO(maybe): Add type traits to allow operator overloads, etc...
    //NOLINTNEXTLINE
    class Type {
    private:

        static phmap::parallel_node_hash_map<Type *, std::shared_ptr<Type>> s_Types;
        Type() = default;

        [[nodiscard]] virtual bool isEqual(const Type &o) const = 0;

        friend class PrimitiveType;
        friend class ArrayType;
        friend class FunctionType;
        friend class StructType;

    public:
        template<class T>
        bool operator==(const T &o) const {
            return this->isEqual(o);
        }

        template<class T>
        bool operator!=(const T &o) const {
            return !this->isEqual(o);
        }

        [[nodiscard]] virtual size_t hash() const = 0;

        friend size_t hash_value(const Type &t) {
            return t.hash();
        }

        static std::shared_ptr<PrimitiveType> CreatePrimitiveType(bool isNumeric=false);

        static std::shared_ptr<ArrayType> CreateArrayType(
                Type* elementsType,
                uint64_t numElements
            );

        static std::shared_ptr<FunctionType> CreateFunctionType(
                Type *returnType,
                std::vector<Type*> paramsType
            );

        static std::shared_ptr<StructType> CreateStructType(
                std::string identifier,
                std::vector<std::string> fieldNames,
                std::vector<Type*> elementsType
            );

        static std::shared_ptr<Type> GetOrInsertType(std::shared_ptr<Type> type) {
            if (s_Types.find(type) != s_Types.end()) {
                auto typePtr = s_Types.find(type.get());
                return typePtr->second;
            }

            s_Types[type.get()] = type;

            return type;
        }
    };
} // namespace yapl
