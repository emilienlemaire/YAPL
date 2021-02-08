#pragma once

#include <string>

#define STRINGIFY(tok) #tok

enum class token{
    NONE,
    IDENT,

    FOR,
    WHILE,
    IF,
    ELSE,
    IN,
    STRUCT,
    TRUE,
    FALSE,
    IMPORT,
    EXPORT,
    RETURN,
    FUNC,

    NUMBER,

    PAR_O,
    PAR_C,
    BRA_O,
    BRA_C,
    ACC_O,
    ACC_C,

    DOT,
    COLON,
    S_QUOTE,
    D_QUOTE,
    D_COLON,
    ARROW,
    SEMI,
    COMMA,

    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    BY,
    MOD,

    LTH,
    MTH,
    EQ,
    LEQ,
    MEQ,
    NEQ,

    NOT,
    OR,
    AND,

    FROM_TO,

    EOF_, // avoid conflicts with existing EOF
    EOL
};

inline std::string tokToString(token tok) {
    switch (tok) {
        case token::NONE:
            return "NONE";
        case token::IDENT:
            return "IDENT";
        case token::FOR:
            return "FOR";
        case token::WHILE:
            return "WHILE";
        case token::IF:
            return "IF";
        case token::ELSE:
            return "ELSE";
        case token::IN:
            return "IN";
        case token::STRUCT:
            return "STRUCT";
        case token::TRUE:
            return "TRUE";
        case token::FALSE:
            return "FALSE";
        case token::IMPORT:
            return "IMPORT";
        case token::EXPORT:
            return "EXPORT";
        case token::RETURN:
            return "RETURN";
        case token::FUNC:
            return "FUNC";
        case token::INT_LIT:
            return "INT_LIT";
        case token::FLOAT_LIT:
            return "FLOAT_LIT";
        case token::PAR_O:
            return "PAR_O";
        case token::PAR_C:
            return "PAR_C";
        case token::BRA_O:
            return "BRA_O";
        case token::BRA_C:
            return "BRA_C";
        case token::ACC_O:
            return "ACC_O";
        case token::ACC_C:
            return "ACC_C";
        case token::DOT:
            return "DOT";
        case token::COLON:
            return "COLON";
        case token::S_QUOTE:
            return "S_QUOTE";
        case token::D_QUOTE:
            return "D_QUOTE";
        case token::D_COLON:
            return "D_COLON";
        case token::ARROW:
            return "ARROW";
        case token::SEMI:
            return "SEMI";
        case token::COMMA:
            return "COMMA";
        case token::ASSIGN:
            return "ASSIGN";
        case token::PLUS:
            return "PLUS";
        case token::MINUS:
            return "MINUS";
        case token::TIMES:
            return "TIMES";
        case token::BY:
            return "BY";
        case token::MOD:
            return "MOD";
        case token::LTH:
            return "LTH";
        case token::MTH:
            return "MTH";
        case token::EQ:
            return "EQ";
        case token::LEQ:
            return "LEQ";
        case token::MEQ:
            return "MEQ";
        case token::NEQ:
            return "NEQ";
        case token::NOT:
            return "NOT";
        case token::OR:
            return "OR";
        case token::AND:
            return "AND";
        case token::FROM_TO:
            return "FROM_TO";
        case token::EOF_: // avoid conflicts with existing EOF
            return "EOF_";
        case token::EOL:
            return "EOL";
    }
}
