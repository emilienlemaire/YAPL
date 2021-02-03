#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <pthread/qos.h>

/**
 * Type declaration for the AST and Symbol Table
 * */

namespace yapl {
    struct id_counter {
        static uint32_t count;
    };
    

    template<class DERIVED>
        struct has_id: id_counter {
            static uint32_t GetId() {
                static uint32_t c = id_counter::count++;
                return c;
            }
        };

    class StaticType {
        virtual const uint32_t getId() const = 0;
        bool operator==(const StaticType &type) const {
            return type.getId() == getId();
        }
    };

    class Bool: public StaticType, public has_id<Bool> {
    public:
        virtual const uint32_t getId() const override {
            return Bool::GetId();
        }
    };

    class Integer: public StaticType, public has_id<Integer> {
    public:
        virtual const uint32_t getId() const override {
            return Integer::GetId();
        }
    };

    class Float: public StaticType, public has_id<Float> {
    public:
        virtual const uint32_t getId() const override {
            return Float::GetId();
        }
    };

    class Double: public StaticType, public has_id<Double> {
    public:
        virtual const uint32_t getId() const override {
            return Double::GetId();
        }
    };

    class Char: public StaticType, public has_id<Char> {
    public:
        virtual const uint32_t getId() const override {
            return Char::GetId();
        }
    };

    class Array: public StaticType, public has_id<Array> {
    private:
        size_t m_Size;
        StaticType *m_ElementsType;
    public:
        Array(size_t arraySize, StaticType *elementsType);

        size_t getSize() const;
        StaticType *getElementsType() const;

        virtual const uint32_t getId() const override {
            return Array::GetId();
        }
    };

    class Struct: public StaticType, public has_id<Struct> {
    private:
        std::vector<StaticType*> m_FieldsTypes;
        std::string m_StructName;
    public:
        Struct(const std::string &structName, const std::vector<StaticType*> &fieldsTypes);

        std::vector<StaticType*> getFieldsTypes() const;
        std::string getStructName() const;

        virtual const uint32_t getId() const override {
            return Struct::GetId();
        }
    };

    class FunctionType: public StaticType, public has_id<FunctionType> {
    private:
        StaticType *m_ReturnType;
        std::vector<StaticType*> m_ParametersTypes;
    public:
        FunctionType(StaticType *returnType, const std::vector<StaticType*>& paramsTypes);

        StaticType *getReturnType() const;
        std::vector<StaticType*> getParametersTypes() const;

        virtual const uint32_t getId() const override {
            return FunctionType::GetId();
        }
    };
} // namespace yapl
