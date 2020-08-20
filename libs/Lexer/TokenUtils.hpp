#include <string>

std::string tokenToString(int token){
    switch (token) {
        case -1:
            return "EOF";
        case -2:
            return "EOL";
        case -3:
            return "type";
        case -4:
            return "def";
        case -5:
            return "label";
        default:
            return std::string(1, (char)token);
    }
}

enum token{
  eof = -1,
  eol = -2,

  type = -3,
  def = -4,
  label = -5,

  paropen = -6,
  parclose = -7,
  bopen = -8,
  bclose = -9,
  iopen = -10,
  iclose = -11,

  eq = -12,
  plus = -13,
  minus = -14,
  divide = -15,
  times = -16,
  mod = -17,
  lth = -18,
  mth = -19,
  leq = -20,
  meq = -21,
  
  fto = -22,
  forlabel = -23,
  whilelabel = -24,
  iflabel = -25,
  elselabel = -26
};
