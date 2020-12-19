#pragma once

#include <string>

namespace yapl {
    class Error {
        private:
            std::string m_Message;
        public:
            Error(const std::string& msg)
                :m_Message(msg)
            {}
            ~Error() = default;

            const std::string getMessage() const { return m_Message; }
    };
}

