#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef _WIN32
  #include <io.h>
  #define isatty _isatty
  #define fileno _fileno
#endif

using std::string;

static inline bool isSpace(char c){ return c==' '||c=='\t'||c=='\n'||c=='\r'; }
static inline bool isDigitC(char c){ return c>='0' && c<='9'; }
static inline bool isAlphaC(char c){ return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'; }
static inline string ltrim(string s){ size_t i=0; while(i<s.size() && isSpace(s[i])) ++i; return s.substr(i); }
static inline string rtrim(string s){ int i=(int)s.size()-1; while(i>=0 && isSpace(s[i])) --i; return s.substr(0,i+1); }
static inline string trim (string s){ return rtrim(ltrim(s)); }

#endif // COMMON_HPP
