/**
 * libs/Symbol/FunctionType.cpp
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
#include "Symbol/Type.hpp"
#include "Symbol/FunctionType.hpp"
#include "parallel_hashmap/phmap_utils.h"
#include <vector>

namespace yapl {
    FunctionType::FunctionType(Type *returnType, std::vector<Type*> paramsType)
        : Type(), p_ReturnType(returnType), v_ParamsType(std::move(paramsType))
    {}

    auto FunctionType::begin() -> decltype(v_ParamsType.begin()) {
        return v_ParamsType.begin();
    }

    auto FunctionType::end() -> decltype(v_ParamsType.end()) {
        return v_ParamsType.end();
    }

    auto FunctionType::cbegin() -> decltype(v_ParamsType.cbegin()) {
        return v_ParamsType.cbegin();
    }

    auto FunctionType::cend() -> decltype(v_ParamsType.cend()) {
        return v_ParamsType.cend();
    }

    bool FunctionType::isEqual(const Type &o) const {
        if (auto t = dynamic_cast<const FunctionType*>(&o)) {
            if (v_ParamsType.size() == t->getNumParams()) {
                if (*p_ReturnType != *(t->getReturnType())) {
                    return false;
                }

                for (size_t i = 0; i < v_ParamsType.size(); i++) {
                    if (*(v_ParamsType[i]) != *(t->getParamType(i))) {
                        return false;
                    }
                }

                return true;
            }
        }

        return false;
    }

    size_t FunctionType::hash() const {
        auto hash = phmap::HashState().combine(0, *p_ReturnType);
        for (const auto &paramType : v_ParamsType) {
            hash = phmap::HashState().combine(0, hash, *paramType);
        }
        return hash;
    }

    const std::string FunctionType::dump() const {
        std::string str = "Function type: return " + p_ReturnType->dump() + ", ";

        if (v_ParamsType.size() > 0) {
            str += "arg types: ";
            for (auto argType : v_ParamsType) {
                str += (argType->dump() + ", ");
            }
        } else {
            str += "nor args.";
        }

        return str;
    }
} // namespace yapl
