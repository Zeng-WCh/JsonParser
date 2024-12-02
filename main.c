#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonobj.h"
#include "jsonparser.h"

#define TEST_FILE "test.json"

json_node *read_example(const char *filename) {
  return parse_json_file(filename);
}

// This is function is used to create a json object below:
// {
//   "array": [
//     {}
//   ],
//   "int": 1,
//   "double": 1.0,
//   "string": "This is an example string.",
//   "boolean": {
//     "true": true,
//     "false": false
//   },
//   "null": null
// }
json_node *write_exaple() {
  json_node *obj = create_json_node();
  obj->type = JSON_OBJECT;
  json_node *arr = add_child(obj, JSON_ARRAY);
  arr->key = TO_JSON_STRING("array");
  add_child(arr, JSON_OBJECT);

  // int, this is a first way to add a
  // k-v pair to an object
  json_node *json_int = add_child(obj, JSON_INT);
  json_int->val.val_as_int = 1;
  json_int->key = TO_JSON_STRING("int");

  // double, to show another a way to add a
  // k-v pair to an object, if you know exactly
  // where you would like to insert it
  // For better performance, it is best to use the latter way
  json_node *json_double = add_a_neighbor(json_int, JSON_DOUBLE);
  json_double->key = TO_JSON_STRING("double");
  json_double->val.val_as_double = 1.;
  // string
  json_node *json_string = add_a_neighbor(json_double, JSON_STRING);
  json_string->key = TO_JSON_STRING("string");
  json_string->val.val_as_str = TO_JSON_STRING("This is an example string.");
  // boolean
  json_node *json_bool_object = add_a_neighbor(json_string, JSON_OBJECT);
  json_bool_object->key = TO_JSON_STRING("boolean");
  json_node *json_true = add_child(json_bool_object, JSON_TRUE);
  json_true->key = TO_JSON_STRING("true");
  json_node *json_false = add_a_neighbor(json_true, JSON_FALSE);
  json_false->key = TO_JSON_STRING("false");
  // null
  json_node *json_null = add_a_neighbor(json_bool_object, JSON_NULL);
  json_null->key = TO_JSON_STRING("null");

  return obj;
}

int main() {
  // printf("This is an example program for show how to use this library.\n\n");
  // printf("Trying to parse from file: %s\n", TEST_FILE);

  json_node *file = read_example(TEST_FILE);

  if (file == NULL) {
    fprintf(stderr, "Failed to read from %s\n", TEST_FILE);
    exit(-1);
  } else {
    printf("Success!!\n");
    printf("File content: \n\"");
    fflush(stdout);
    print_json(file, 0, stdout);
    printf("\"\n");
    fflush(stdout);
  }

  printf("Trying to create a json file:\n");
  json_node *obj = write_exaple();
  print_json(obj, 0, stdout);
  free_json_node(file);
  free_json_node(obj);
}