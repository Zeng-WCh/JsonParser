#include "jsonparser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonobj.h"

int json_line = 1;
int json_column = 1;

int json_token = -1;

FILE *json_file = NULL;

char *json_string = NULL;

int bufsize = DEFAULT_BUF_SIZE;

int stop_read = 0;

void logger_error(int expected) {
  fprintf(stderr, "Error at <%d:%d>: Expected \"%s\", got %s.\n", json_line,
          json_column, tok_to_string(expected), tok_to_string(json_token));
}

const char *tok_to_string(int tok) {
  switch (tok) {
    case TOK_EOF:
      return "EOF";
    case TOK_LBRACE:
      return "{";
    case TOK_RBRACE:
      return "}";
    case TOK_LBRACKET:
      return "[";
    case TOK_RBRACKET:
      return "]";
    case TOK_COLON:
      return ":";
    case TOK_COMMA:
      return ",";
    case TOK_STRING:
      return "STRING";
    case TOK_INT:
      return "INT";
    case TOK_DOUBLE:
      return "DOUBLE";
    case TOK_TRUE:
      return "true";
    case TOK_FALSE:
      return "false";
    case TOK_NULL:
      return "null";
    default:
      return "UNKNOWN";
  }
}

int read_a_char() {
  int ch = fgetc(json_file);
  // comment
  while (ch == '/' || isspace(ch)) {
    if (ch == '/') {
      json_column++;
      if (ch == '/') {
        json_column++;
        while (ch != '\n') {
          ch = fgetc(json_file);
          json_column++;
        }
      } else {
        fprintf(stderr, "Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    }

    while (isspace(ch)) {
      if (ch == '\n') {
        json_line++;
        json_column = 1;
      } else {
        json_column++;
      }
      ch = fgetc(json_file);
    }
  }

  return ch;
}

int next_token() {
  if (json_string == NULL) {
    json_string = malloc(sizeof(char) * bufsize);
  }
  if (json_file == NULL) {
    fprintf(stderr, "Error from token: No file opened.\n");
    exit(EXIT_FAILURE);
  }
  if (stop_read == 1) {
    stop_read = 0;
    return json_token;
  }
  // EOF, and never read
  if (json_token == TOK_EOF) {
    return TOK_EOF;
  }
  int ch = read_a_char();
  if (ch == EOF) {
    json_token = TOK_EOF;
    return TOK_EOF;
  }

  if (ch == ':') {
    json_token = TOK_COLON;
    return TOK_COLON;
  }
  if (ch == ',') {
    json_token = TOK_COMMA;
    return TOK_COMMA;
  }
  if (ch == '{') {
    json_token = TOK_LBRACE;
    return TOK_LBRACE;
  }
  if (ch == '}') {
    json_token = TOK_RBRACE;
    return TOK_RBRACE;
  }
  if (ch == '[') {
    json_token = TOK_LBRACKET;
    return TOK_LBRACKET;
  }
  if (ch == ']') {
    json_token = TOK_RBRACKET;
    return TOK_RBRACKET;
  }
  if (ch == '"') {
    // finish reading a string
    int size = 0;
    ch = read_a_char();
    while (ch != '"') {
      json_string[size++] = ch;
      if (size == bufsize) {
        bufsize += DEFAULT_BUF_SIZE;
        json_string = realloc(json_string, sizeof(char) * bufsize);
      }
      ch = read_a_char();
    }
    json_string[size] = '\0';
    json_token = TOK_STRING;
    return TOK_STRING;
  }
  if (ch == 't') {
    // true
    ch = read_a_char();
    if (ch == 'r') {
      ch = read_a_char();
      if (ch == 'u') {
        ch = read_a_char();
        if (ch == 'e') {
          json_token = TOK_TRUE;
          return TOK_TRUE;
        } else {
          fprintf(stderr, "Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        fprintf(stderr, "Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (ch == 'f') {
    // false
    ch = read_a_char();
    if (ch == 'a') {
      ch = read_a_char();
      if (ch == 'l') {
        ch = read_a_char();
        if (ch == 's') {
          ch = read_a_char();
          if (ch == 'e') {
            json_token = TOK_FALSE;
            return TOK_FALSE;
          } else {
            fprintf(stderr, "Error from token: Invalid Json Structure.\n");
            exit(EXIT_FAILURE);
          }
        } else {
          fprintf(stderr, "Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        fprintf(stderr, "Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (ch == 'n') {
    // null
    ch = read_a_char();
    if (ch == 'u') {
      ch = read_a_char();
      if (ch == 'l') {
        ch = read_a_char();
        if (ch == 'l') {
          json_token = TOK_NULL;
          return TOK_NULL;
        } else {
          fprintf(stderr, "Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        fprintf(stderr, "Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (isdigit(ch) || ch == '-') {
    // number, further check if a double or an int
    int size = 0, is_double = 0;
    json_string[size++] = ch;
    ch = read_a_char();
    while (isdigit(ch) || ch == '.') {
      if (ch == '.') {
        is_double = 1;
      }
      json_string[size++] = ch;
      if (size == bufsize) {
        bufsize += DEFAULT_BUF_SIZE;
        json_string = realloc(json_string, sizeof(char) * bufsize);
      }
      ch = read_a_char();
    }
    json_string[size] = '\0';
    if (!feof(json_file))
      fseek(json_file, -1, SEEK_CUR);
    if (is_double) {
      json_token = TOK_DOUBLE;
      return TOK_DOUBLE;
    } else {
      json_token = TOK_INT;
      return TOK_INT;
    }
  }
  fprintf(stderr, "Error from token: Invalid Json Structure.\n");
  exit(EXIT_FAILURE);
}

void json_free() {
  if (json_file != NULL) {
    fclose(json_file);
  }
  if (json_string != NULL) {
    free(json_string);
  }
}

json_node *parse_json_file(const char *filename) {
  json_file = fopen(filename, "r");
  if (json_file == NULL) {
    fprintf(stderr, "Error: Cannot open file.\n");
    exit(EXIT_FAILURE);
  }
  json_node *root = parse_json();
  json_free();
  return root;
}

// <json> ::= object | array | string | number | true | false | <empty>
json_node *parse_json() {
  int tok = next_token();

  if (tok == TOK_LBRACE) {
    // object
    stop_read = 1;
    return parse_object();
  } else if (tok == TOK_LBRACKET) {
    stop_read = 1;
    return parse_array();
  }

  else {
    // string, number, true, false, or empty
    if (tok == TOK_STRING) {
      json_node *node = create_json_node();
      node->type = JSON_STRING;
      node->val.val_as_str = TO_JSON_STRING(json_string);
      return node;
    } else if (tok == TOK_INT) {
      json_node *node = create_json_node();
      node->type = JSON_INT;
      node->val.val_as_int = atoi(json_string);
      return node;
    } else if (tok == TOK_DOUBLE) {
      json_node *node = create_json_node();
      node->type = JSON_DOUBLE;
      node->val.val_as_double = atof(json_string);
      return node;
    } else if (tok == TOK_TRUE) {
      json_node *node = create_json_node();
      node->type = JSON_TRUE;
      return node;
    } else if (tok == TOK_FALSE) {
      json_node *node = create_json_node();
      node->type = JSON_FALSE;
      return node;
    } else if (tok == TOK_NULL) {
      json_node *node = create_json_node();
      node->type = JSON_NULL;
      return node;
    } else if (tok == TOK_EOF) {
      // empty
      return NULL;
    } else {
      fprintf(stderr, "Error from json: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
}

// object ::= '{' <member> <object_res> '}' | '{' '}'
json_node *parse_object() {
  int cur = next_token();
  if (cur != TOK_LBRACE) {
    logger_error(TOK_LBRACE);
    exit(EXIT_FAILURE);
  }
  cur = next_token();
  if (cur == TOK_RBRACE) {
    json_node *empty = create_json_node();
    empty->type = JSON_OBJECT;
    return empty;
  }
  stop_read = 1;

  json_node *node = create_json_node();
  node->type = JSON_OBJECT;
  node->child = parse_member();
  node->child->next = parse_object_res();

  cur = next_token();
  if (cur != TOK_RBRACE) {
    logger_error(TOK_RBRACE);
    exit(EXIT_FAILURE);
  }

  return node;
}

// <array> ::= '[' <json> <array_res> ']' | '[' ']'
json_node *parse_array() {
  int cur = next_token();
  if (cur != TOK_LBRACKET) {
    logger_error(TOK_LBRACKET);
    exit(EXIT_FAILURE);
  }
  cur = next_token();
  if (cur == TOK_RBRACKET) {
    json_node *empty = create_json_node();
    empty->type = JSON_ARRAY;
    return empty;
  }
  stop_read = 1;
  json_node *node = create_json_node();
  node->type = JSON_ARRAY;
  node->child = parse_json();
  node->child->next = parse_array_res();

  cur = next_token();
  if (cur != TOK_RBRACKET) {
    logger_error(TOK_RBRACKET);
    exit(EXIT_FAILURE);
  }
  return node;
}

// <array_res> ::= ',' <json> <array_res> | <empty>
json_node *parse_array_res() {
  int cur = next_token();
  if (cur != TOK_COMMA) {
    stop_read = 1;
    return NULL;
  }
  json_node *node = parse_json();
  node->next = parse_array_res();
  return node;
}

// <member> ::= <string> ':' <json>
json_node *parse_member() {
  int cur = next_token();
  if (cur != TOK_STRING) {
    logger_error(TOK_STRING);
    exit(EXIT_FAILURE);
  }
  json_node *key = create_json_node();
  key->key = TO_JSON_STRING(json_string);

  cur = next_token();
  if (cur != TOK_COLON) {
    logger_error(TOK_COLON);
    exit(EXIT_FAILURE);
  }

  json_node *val = parse_json();

  if (val->type == JSON_OBJECT || val->type == JSON_ARRAY) {
    val->key = key->key;
    free(key);
    return val;
  } else {
    key->val = val->val;
    key->type = val->type;
    free(val);
  }

  return key;
}

// <object_res> ::= ',' <member> <object_res> | <empty>
json_node *parse_object_res() {
  int cur = next_token();
  if (cur != TOK_COMMA) {
    stop_read = 1;
    return NULL;
  }
  json_node *mem = parse_member();
  mem->next = parse_object_res();
  return mem;
}