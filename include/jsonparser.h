#pragma once
#include <cstddef>
#ifndef __JSON_PARSER_HPP__
#define __JSON_PARSER_HPP__

#include "jsonobj.h"
#include <string>

json_node *from_string(const std::string &json);
json_node *from_file(const std::string &filename);
std::string to_string(json_node *node);

namespace {
#define DEFAULT_BUF_SIZE 32

typedef enum {
  TOK_EOF = 0,
  TOK_LBRACE = 257, // {
  TOK_RBRACE,       // }
  TOK_LBRACKET,     // [
  TOK_RBRACKET,     // ]
  TOK_COLON,        // :
  TOK_COMMA,        // ,
  TOK_STRING,       // "..."
  TOK_INT,          // 1234
  TOK_DOUBLE,       // 1234.5678
  TOK_TRUE,         // true
  TOK_FALSE,        // false
  TOK_NULL          // null
} json_tokens;
} // namespace

#endif