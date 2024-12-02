#ifndef __JSON_OBJ__
#define __JSON_OBJ__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TO_JSON_STRING(STR) strcpy((char *)malloc(strlen((STR)) + 1), (STR))

typedef enum {
  JSON_NULL = 0,
  JSON_INT = 257,
  JSON_DOUBLE,
  JSON_STRING,
  JSON_TRUE,
  JSON_FALSE,
  JSON_ARRAY,
  JSON_OBJECT
} json_type;

typedef union {
  int64_t val_as_int;
  double val_as_double;
  char *val_as_str;
} json_value;

typedef struct json_node {
  // k-v pair
  char *key;
  json_value val;
  json_type type;

  // next node
  struct json_node *next; // TODO: *pre;

  // child node
  struct json_node *child; // TODO: *parent;
} json_node;

json_node *create_json_node();

void free_json_node(json_node *);

void print_json(json_node *node, int indent, FILE *fp);

json_node *json_get(json_node *, const char *);

json_node *add_child(json_node *, json_type);

json_node *add_a_neighbor(json_node *, json_type);

#ifdef __cplusplus
}
#endif

#endif