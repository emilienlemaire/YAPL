/**
 * include/Symbol/FunctionType.hpp
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

#include "Type.hpp"
#include <vector>

namespace yapl {
    class FunctionType : public Type {
    private:
        std::vector<Type*> v_ParamsType;
        Type *p_ReturnType;

        FunctionType(Type* returnType, std::vector<Type*> paramsType);

        virtual bool isEqual(const Type &o) override;

        friend class Type;
    public:
        [[nodiscard]] virtual size_t hash() const override;

        auto begin() -> decltype(v_ParamsType.begin());
        auto end() -> decltype(v_ParamsType.end());
        auto cbegin() -> decltype(v_ParamsType.cbegin());
        auto cend() -> decltype(v_ParamsType.cend());

        [[nodiscard]] const std::vector<Type*> &getParamsType() const { return v_ParamsType; }
        [[nodiscard]] const Type* getParamType(size_t idx) const { return v_ParamsType[idx]; }
        [[nodiscard]] auto getNumParams() const -> decltype(v_ParamsType.size()) { return v_ParamsType.size(); }
        [[nodiscard]] const Type *getReturnType() const { return p_ReturnType; }
    };
} // namespace yapl
