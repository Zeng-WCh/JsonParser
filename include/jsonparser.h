#ifndef __JSON_PARSER__
#define __JSON_PARSER__

#include <stdio.h>
#include "jsonobj.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_BUF_SIZE 32

typedef enum {
  TOK_EOF = 0,
  TOK_LBRACE = 257,  // {
  TOK_RBRACE,        // }
  TOK_LBRACKET,      // [
  TOK_RBRACKET,      // ]
  TOK_COLON,         // :
  TOK_COMMA,         // ,
  TOK_STRING,        // "..."
  TOK_INT,           // 1234
  TOK_DOUBLE,        // 1234.5678
  TOK_TRUE,          // true
  TOK_FALSE,         // false
  TOK_NULL           // null
} json_tokens;

extern int json_line;
extern int json_column;

extern int json_token;

extern FILE *json_file;

extern char *json_string;

int next_token();

const char *tok_to_string(int tok);

void json_free();

json_node *parse_json_file(const char *filename);

json_node *parse_json();

json_node *parse_object();

json_node *parse_array();

json_node *parse_array_res();

json_node *parse_member();

json_node *parse_object_res();

#ifdef __cplusplus
}
#endif

#endif