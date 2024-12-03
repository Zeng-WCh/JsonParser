#include "jsonparser.h"
#include "jsonobj.h"
#include "logger.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int JsonLine = 1;
int JsonColumn = 1;
int JsonToken = -1;
FILE *JsonFile = NULL;
char *JsonString = NULL;
int BufSize = DEFAULT_BUF_SIZE;
bool StopRead = false;

int next_token();
const char *tok_to_string(int tok);
void json_free();
json_node *parse_json_file(const char *filename);
json_node *parse_json();
json_object *parse_object();
json_array *parse_array();
void parse_array_res(json_array *arr);
void parse_member(json_object *obj);
void parse_object_res(json_object *obj);

json_node *from_file(const std::string &filename) {
  return parse_json_file(filename.c_str());
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

int read_a_char(bool escape_space) {
  int ch = fgetc(JsonFile);

  if (escape_space) {
    while (isspace(ch)) {
      if (ch == '\n') {
        JsonLine++;
        JsonColumn = 1;
      } else {
        JsonColumn++;
      }
      ch = fgetc(JsonFile);
    }
  } else {
  }

  return ch;
}

int next_token() {
  if (JsonString == NULL) {
    JsonString = (char *)malloc(sizeof(char) * BufSize);
  }
  if (JsonFile == NULL) {
    ERROR("Error from token: No file opened.\n");
    exit(EXIT_FAILURE);
  }
  if (StopRead == 1) {
    StopRead = 0;
    return JsonToken;
  }
  // EOF, and never read
  if (JsonToken == TOK_EOF) {
    return TOK_EOF;
  }
  int ch = read_a_char(true);
  if (ch == EOF) {
    JsonToken = TOK_EOF;
    return TOK_EOF;
  }

  if (ch == ':') {
    JsonToken = TOK_COLON;
    return TOK_COLON;
  }
  if (ch == ',') {
    JsonToken = TOK_COMMA;
    return TOK_COMMA;
  }
  if (ch == '{') {
    JsonToken = TOK_LBRACE;
    return TOK_LBRACE;
  }
  if (ch == '}') {
    JsonToken = TOK_RBRACE;
    return TOK_RBRACE;
  }
  if (ch == '[') {
    JsonToken = TOK_LBRACKET;
    return TOK_LBRACKET;
  }
  if (ch == ']') {
    JsonToken = TOK_RBRACKET;
    return TOK_RBRACKET;
  }
  if (ch == '"') {
    // finish reading a string
    int size = 0;
    ch = read_a_char(false);
    while (ch != '"') {
      JsonString[size++] = ch;
      if (size == BufSize) {
        BufSize += DEFAULT_BUF_SIZE;
        JsonString = (char *)realloc(JsonString, sizeof(char) * BufSize);
      }
      ch = read_a_char(false);
    }
    JsonString[size] = '\0';
    JsonToken = TOK_STRING;
    return TOK_STRING;
  }
  if (ch == 't') {
    // true
    ch = read_a_char(0);
    if (ch == 'r') {
      ch = read_a_char(0);
      if (ch == 'u') {
        ch = read_a_char(0);
        if (ch == 'e') {
          JsonToken = TOK_TRUE;
          return TOK_TRUE;
        } else {
          ERROR("Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        ERROR("Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      ERROR("Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (ch == 'f') {
    // false
    ch = read_a_char(0);
    if (ch == 'a') {
      ch = read_a_char(0);
      if (ch == 'l') {
        ch = read_a_char(0);
        if (ch == 's') {
          ch = read_a_char(0);
          if (ch == 'e') {
            JsonToken = TOK_FALSE;
            return TOK_FALSE;
          } else {
            ERROR("Error from token: Invalid Json Structure.\n");
            exit(EXIT_FAILURE);
          }
        } else {
          ERROR("Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        ERROR("Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      ERROR("Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (ch == 'n') {
    // null
    ch = read_a_char(0);
    if (ch == 'u') {
      ch = read_a_char(0);
      if (ch == 'l') {
        ch = read_a_char(0);
        if (ch == 'l') {
          JsonToken = TOK_NULL;
          return TOK_NULL;
        } else {
          ERROR("Error from token: Invalid Json Structure.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        ERROR("Error from token: Invalid Json Structure.\n");
        exit(EXIT_FAILURE);
      }
    } else {
      ERROR("Error from token: Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (isdigit(ch) || ch == '-') {
    // number, further check if a double or an int
    int size = 0, is_double = 0;
    JsonString[size++] = ch;
    ch = read_a_char(0);
    while (isdigit(ch) || ch == '.') {
      if (ch == '.') {
        is_double = 1;
      }
      JsonString[size++] = ch;
      if (size == BufSize) {
        BufSize += DEFAULT_BUF_SIZE;
        JsonString = (char *)realloc(JsonString, sizeof(char) * BufSize);
      }
      ch = read_a_char(0);
    }
    JsonString[size] = '\0';
    if (!feof(JsonFile))
      fseek(JsonFile, -1, SEEK_CUR);
    if (is_double) {
      JsonToken = TOK_DOUBLE;
      return TOK_DOUBLE;
    } else {
      JsonToken = TOK_INT;
      return TOK_INT;
    }
  }
  ERROR("Error from token: Invalid Json Structure.\n");
  exit(EXIT_FAILURE);
}

void json_free() {
  if (JsonFile != NULL) {
    fclose(JsonFile);
  }
  if (JsonString != NULL) {
    free(JsonString);
  }
}

json_node *parse_json_file(const char *filename) {
  JsonFile = fopen(filename, "r");
  if (JsonFile == NULL) {
    ERROR("Error: Cannot open file.\n");
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
    StopRead = true;
    return parse_object();
  } else if (tok == TOK_LBRACKET) {
    StopRead = true;
    return parse_array();
  }

  else {
    // string, number, true, false, or empty
    if (tok == TOK_STRING) {
      json_node *node = new json_string(JsonString);
      return node;
    } else if (tok == TOK_INT) {
      json_node *node = new json_int(atoi(JsonString));
      return node;
    } else if (tok == TOK_DOUBLE) {
      json_node *node = new json_double(atof(JsonString));
      return node;
    } else if (tok == TOK_TRUE) {
      return new json_true();
    } else if (tok == TOK_FALSE) {
      return new json_false();
    } else if (tok == TOK_NULL) {
      return new json_null();
    } else if (tok == TOK_EOF) {
      // empty
      return nullptr;
    } else {
      ERROR("Invalid Json Structure.\n");
      exit(EXIT_FAILURE);
    }
  }
}

// object ::= '{' <member> <object_res> '}' | '{' '}'
json_object *parse_object() {
  int tok = next_token();
  if (tok != TOK_LBRACE) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting '{'.\n");
    exit(EXIT_FAILURE);
  }
  tok = next_token();
  if (tok == TOK_RBRACE) {
    return new json_object();
  }
  StopRead = true;
  json_object *node = new json_object();
  parse_member(node);
  parse_object_res(dynamic_cast<json_object *>(node));
  tok = next_token();
  if (tok != TOK_RBRACE) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting '}'.\n");
    exit(EXIT_FAILURE);
  }
  return node;
}

// <array> ::= '[' <json> <array_res> ']' | '[' ']'
json_array *parse_array() {
  int tok = next_token();
  if (tok != TOK_LBRACKET) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting '['.\n");
    exit(EXIT_FAILURE);
  }
  tok = next_token();
  if (tok == TOK_RBRACKET) {
    return new json_array();
  }
  StopRead = true;
  json_array *node = new json_array();
  json_node *value = parse_json();
  if (value) {
    node->push_back(value);
    parse_array_res(node);
  }
  tok = next_token();
  if (tok != TOK_RBRACKET) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting ']'.\n");
    exit(EXIT_FAILURE);
  }
  return node;
}

// <array_res> ::= ',' <json> <array_res> | <empty>
void parse_array_res(json_array *arr) {
  int tok = next_token();
  if (tok != TOK_COMMA) {
    StopRead = true;
    return;
  }
  json_node *value = parse_json();
  arr->push_back(value);
  parse_array_res(arr);
  return;
}

// <member> ::= <string> ':' <json>
void parse_member(json_object *obj) {
  int tok = next_token();
  if (tok != TOK_STRING) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting a string.\n");
    exit(EXIT_FAILURE);
  }
  std::string key = JsonString;
  tok = next_token();
  if (tok != TOK_COLON) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting ':'.\n");
    exit(EXIT_FAILURE);
  }
  json_node *value = parse_json();
  obj->set(key, value);
  return;
}

// <object_res> ::= ',' <member> <object_res> | <empty>
void parse_object_res(json_object *obj) {
  int tok = next_token();
  if (tok != TOK_COMMA) {
    StopRead = true;
    return;
  }
  parse_member(obj);
  parse_object_res(obj);
  return;
}