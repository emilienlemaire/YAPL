#pragma once

#include <string>

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

    INT_LIT,
    FLOAT_LIT,

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

inline std::string tokenToString(int token){
    switch (token) {
        case -1:
            return "EOF";
        case -2:
            return "EOL";
        case -3:
            return "type";
        case -4:
            return "func";
        case -5:
            return "label";
        case -6:
            return "popen";
        case -7:
            return "pclose";
        case -8:
            return "bopen";
        case -9:
            return "bclose";
        case -10:
            return "iopen";
        case -11:
            return "iclose";
        case -12:
            return "eq";
        case -13:
            return "plus";
        case -14:
            return "minus";
        case -15:
            return "divide";
        case -16:
            return "times";
        case -17:
            return "mod";
        case -18:
            return "lth";
        case -19:
            return "mth";
        case -20:
            return "forlabel";
        case -21:
            return "whilelabel";
        case -22:
            return "iflabel";
        case -23:
            return "elselabel";
        case -24:
            return "inlabel";
        case -25:
            return "structlabel";
        case -26:
            return "truelalbel";
        case -27:
            return "falselabel";
        case -28:
            return "importlabel";
        case -29:
            return "exportlabel";
        case -30:
            return "int_value";
        case -31:
            return "float_value";
        case -32:
            return "semicolon";
        case -33:
            return "comma";
        case -34:
            return "notsym";
        case -35:
            return "orsym";
        case -36:
            return "andsym";
        case -37:
            return "point";
        case -38:
            return "colon";
        case -39:
            return "squote";
        case -40:
            return "dquote";
        case -41:
            return "access_sym";
        case -42:
            return "fromto";
        case -43:
            return "fromtol";
        case -44:
            return "fromtominus";
        case -45:
            return "fromoreto";
        case -46:
            return "eqcomp";
        case -47:
            return "leq";
        case -48:
            return "meq";
        case -49:
            return "neq";
        case -50:
            return "arrow_op";
        case -51:
            return "returnlabel";
        default:
            return std::string(1, (char)token);
    }
}

enum token{
  eof          = -1,
  eol          = -2,

  type         = -3,
  func         = -4,
  identifier   = -5,

  paropen      = -6,
  parclose     = -7,
  bopen        = -8,
  bclose       = -9,
  iopen        = -10,
  iclose       = -11,

  eq           = -12,
  plus         = -13,
  minus        = -14,
  divide       = -15,
  times        = -16,
  mod          = -17,
  lth          = -18,
  mth          = -19,

  forlabel     = -20,
  whilelabel   = -21,
  iflabel      = -22,
  elselabel    = -23,
  inlabel      = -24,
  structlabel  = -25,
  truelabel    = -26,
  falselabel   = -27,
  importlabel  = -28,
  exportlalbel = -29,

  int_value    = -30,
  float_value  = -31,

  semicolon    = -32,
  comma        = -33,

  notsym       = -34,
  orsym        = -35,
  andsym       = -36,

  point        = -37,
  colon        = -38,
  squote       = -39,
  dquote       = -40,

  access_sym   = -41,
  fromto       = -42,
  fromtol      = -43,
  fromtominus  = -44,
  fromoreto    = -45,

  eqcomp       = -46,
  leq          = -47,
  meq          = -48,
  neq          = -49,
  arrow_op     = -50,

  returnlabel = -51,

  unknown      =-100
};

