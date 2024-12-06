#include "jsonparser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include "jsonobj.h"
#include "logger.h"

int JsonLine = 1;
int JsonColumn = 1;
int JsonToken = -1;
char *JsonString = nullptr;
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
  TRACE();
  return parse_json_file(filename.c_str());
}

class JsonIO {
 public:
  virtual ~JsonIO() = 0;
  virtual int next() = 0;

  virtual bool is_eof() = 0;
  virtual void seek(long offset, int whence) = 0;
};

JsonIO::~JsonIO() {
}

class FileIO : public JsonIO {
 private:
  FILE *file;

 public:
  FileIO(const char *filename) {
    TRACE();
    file = fopen(filename, "r");
    if (file == NULL) {
      ERROR("Error: Cannot open file.\n");
      exit(EXIT_FAILURE);
    }
  }
  ~FileIO() {
    TRACE();
    if (file != NULL) {
      fclose(file);
    }
  }

  int next() override {
    TRACE();
    return fgetc(file);
  }
  bool is_eof() override {
    TRACE();
    return feof(file);
  }
  void seek(long offset, int whence) override {
    TRACE();
    fseek(file, offset, whence);
  }
};

class StringIO : public JsonIO {
 private:
  std::string buffer;
  size_t index;

 public:
  StringIO(const std::string &str) : buffer(str), index(0) {
    TRACE();
  }
  ~StringIO() {
    TRACE();
  }

  int next() override {
    TRACE();
    if (index >= buffer.size()) {
      return EOF;
    }
    return buffer[index++];
  }

  bool is_eof() override {
    TRACE();
    return index >= buffer.size();
  }
  void seek(long offset, int whence) override {
    TRACE();
    if (whence == SEEK_SET) {
      index = offset;
    } else if (whence == SEEK_CUR) {
      index += offset;
    } else if (whence == SEEK_END) {
      index = buffer.size() + offset;
    }
  }
};

JsonIO *io = nullptr;

const char *tok_to_string(int tok) {
  switch (tok) {
    case TOK_EOF:
      return "EOF";
    case TOK_LBRACE:
      return "TOK_LBRACE";
    case TOK_RBRACE:
      return "TOK_RBRACE";
    case TOK_LBRACKET:
      return "TOK_LBRACKET";
    case TOK_RBRACKET:
      return "TOK_RBRACKET";
    case TOK_COLON:
      return "TOK_COLON";
    case TOK_COMMA:
      return "TOK_COMMA";
    case TOK_STRING:
      return "TOK_STRING";
    case TOK_INT:
      return "TOK_INT";
    case TOK_DOUBLE:
      return "TOK_DOUBLE";
    case TOK_TRUE:
      return "TOK_TRUE";
    case TOK_FALSE:
      return "TOK_FALSE";
    case TOK_NULL:
      return "TOK_NULL";
    default:
      return "UNKNOWN_TOKEN";
  }
}

int read_a_char(bool escape_space) {
  TRACE();
  int ch = io->next();

  if (escape_space) {
    while (isspace(ch)) {
      if (ch == '\n') {
        JsonLine++;
        JsonColumn = 1;
      } else {
        JsonColumn++;
      }
      ch = io->next();
    }
  } else {
  }

  return ch;
}

int next_token() {
  TRACE();
  if (JsonString == nullptr) {
    JsonString = (char *)malloc(sizeof(char) * BufSize);
  }
  if (io == nullptr) {
    ERROR("Error: No input source.\n");
    exit(EXIT_FAILURE);
  }
  if (StopRead) {
    StopRead = false;
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
    ch = read_a_char(false);
    if (ch == 'r') {
      ch = read_a_char(false);
      if (ch == 'u') {
        ch = read_a_char(false);
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
    ch = read_a_char(false);
    if (ch == 'a') {
      ch = read_a_char(false);
      if (ch == 'l') {
        ch = read_a_char(false);
        if (ch == 's') {
          ch = read_a_char(false);
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
    ch = read_a_char(false);
    if (ch == 'u') {
      ch = read_a_char(false);
      if (ch == 'l') {
        ch = read_a_char(false);
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
    ch = read_a_char(false);
    while (isdigit(ch) || ch == '.') {
      if (ch == '.') {
        is_double = 1;
      }
      JsonString[size++] = ch;
      if (size == BufSize) {
        BufSize += DEFAULT_BUF_SIZE;
        JsonString = (char *)realloc(JsonString, sizeof(char) * BufSize);
      }
      ch = read_a_char(true);
    }
    JsonString[size] = '\0';
    if (!io->is_eof())
      io->seek(-1, SEEK_CUR);
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
  TRACE();
  if (JsonString != NULL) {
    free(JsonString);
    JsonString = nullptr;
  }
  if (io != nullptr) {
    delete io;
    io = nullptr;
  }
}

json_node *parse_json_file(const char *filename) {
  TRACE();
  io = new FileIO(filename);

  json_node *root = parse_json();
  json_free();
  return root;
}

// <json> ::= object | array | string | number | true | false | <empty>
json_node *parse_json() {
  TRACE();
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
  TRACE();
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
  TRACE();
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
  TRACE();
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
  TRACE();
  DEBUG("Parsing member.");
  int tok = next_token();
  DEBUG("Start with type: %s", tok_to_string(JsonToken));

  if (tok != TOK_STRING) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting a string.\n");
    exit(EXIT_FAILURE);
  }
  std::string key = JsonString;
  DEBUG("Key: \"%s\"", key.c_str());
  tok = next_token();
  if (tok != TOK_COLON) {
    ERROR("Unexcepted token at line %d, column %d.\n", JsonLine, JsonColumn);
    ERROR("Expecting ':'.\n");
    exit(EXIT_FAILURE);
  }
  json_node *value = parse_json();
  DEBUG("Value type: %s", tok_to_string(value->getType()));
  obj->set(key, value);
  return;
}

// <object_res> ::= ',' <member> <object_res> | <empty>
void parse_object_res(json_object *obj) {
  TRACE();
  int tok = next_token();
  if (tok != TOK_COMMA) {
    StopRead = true;
    return;
  }
  parse_member(obj);
  parse_object_res(obj);
  return;
}

json_node *from_string(const std::string &json) {
  TRACE();
  DEBUG("Parsing json string:\n\"%s\"", json.c_str());
  io = new StringIO(json);
  json_node *root = parse_json();
  json_free();
  return root;
}