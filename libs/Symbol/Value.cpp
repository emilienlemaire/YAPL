#include "Symbol/Value.hpp"
#include <memory>

namespace yapl {
    std::shared_ptr<Value> Value::CreateTypeValue(
            const std::string &name,
            std::shared_ptr<Type> type
            ) {
        Value v;
        v.m_Kind = ValueKind::Type;
        v.m_Name = std::move(name);
        v.m_Type = type;

        return std::make_shared<Value>(v);
    }
}
