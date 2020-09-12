#pragma once

#include <string>

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
            return "pclone";
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
            return "leq";
        case -21:
            return "med";
        case -22:
            return "fto";
        case -23:
            return "flabel";
        case -24:
            return "whilelable";
        case -25:
            return "iflabel";
        case -26:
            return "elselabel";
        case -27:
            return "inlabel";
        case -28:
            return "int_value";
        case -29:
            return "float_value";
        case -30:
            return "ftol";
        case -31:
            return "ftom";
        case -32:
            return "ffroml";
        case -33:
            return "notsym";
        case -34:
            return "andsym";
        case -35:
            return "orsym";
        case -36:
            return "eqsym";
        case -37:
            return "neqsym";
        case -38:
            return "truelabel";
        case -39:
            return "falselabel";
        case -40:
            return "dcoloperator";
        case -41:
            return "importlabel";
        case -42:
            return "exportlalbel";
        case -43:
            return "semicolon";
        case -44:
            return "comma";
        case -45:
            return "arrow";
        default:
            return std::string(1, (char)token);
    }
}

enum token{
  eof          = -1,
  eol          = -2,

  type         = -3,
  func         = -4,
  label        = -5,

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
  leq          = -20,
  meq          = -21,

  fto          = -22,
  forlabel     = -23,
  whilelabel   = -24,
  iflabel      = -25,
  elselabel    = -26,
  inlabel      = -27,

  int_value    = -28,
  float_value  = -29,

  ftol         = -30,
  ftom         = -31,
  ffroml       = -32,

  notsym       = -33,
  andsym       = -34,
  orsym        = -35,
  eqsym        = -36,
  neqsym       = -37,

  truelabel    = -38,
  falselabel   = -39,

  dcoloperator = -40,

  importlabel  = -41,
  exportlalbel = -42,

  semicolon    = -43,
  comma        = -44,
  arrow        = -45
};

