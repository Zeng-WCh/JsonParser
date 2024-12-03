#pragma once
#include <cstddef>
#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include "jsonobj.h"
#include <string>

#define LOGGER(fmt, ...)                                                       \
  fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

enum tok {
  tok_eof = -1,
  tok_null = 256,
  tok_true,
  tok_false,
  tok_number,
  tok_string,
  tok_lbracket, // [
  tok_rbracket, // ]
  tok_lbrace,   // {
  tok_rbrace,   // }
  tok_comma,    // ,
  tok_colon,    // :

  tok_unknown
};

typedef struct Loc {
  size_t line, col;
  // File name, do not have the ownership of the string
  const char *file;

  inline Loc(size_t l, size_t c, const char *f) : line(l), col(c), file(f) {}
  inline Loc() : line(0), col(0), file(nullptr) {}
  inline Loc(const Loc &loc) : line(loc.line), col(loc.col), file(loc.file) {}
  inline Loc &operator=(const Loc &loc) {
    line = loc.line;
    col = loc.col;
    file = loc.file;
    return *this;
  }

  inline bool operator==(const Loc &loc) const {
    return line == loc.line && col == loc.col && file == loc.file;
  }
  inline bool operator!=(const Loc &loc) const { return !(*this == loc); }

  inline size_t getLine() const { return line; }
  inline size_t getCol() const { return col; }
  inline const char *getFileName() const { return file; }
} Loc;

class Tok {
private:
  tok type;
  std::string lexeme;
  Loc location;

public:
  inline Tok(tok t, const std::string &l, const Loc &loc)
      : type(t), lexeme(l), location(loc) {}

  inline Tok() : type(tok_unknown), lexeme(""), location() {}

  inline tok getType() const { return type; }
  inline const char *getLexeme() const { return lexeme.c_str(); }
  inline const Loc &getLocation() const { return location; }
};

class Lexer {
private:
  int LastChar;
  FILE *fp;

protected:
  char getChar();

public:
  Lexer();
  ~Lexer();
};

class Parser {
private:
  Lexer *lexer;

public:
  json_node *parse();
};

#endif