#include "jsonobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

json_node *create_json_node() {
  json_node *node = malloc(sizeof(json_node));
  memset(node, 0, sizeof(json_node));
  return node;
}

void free_json_node(json_node *node) {
  // do the recursive free
  if (node->child != NULL) {
    free_json_node(node->child);
  }
  if (node->next != NULL) {
    free_json_node(node->next);
  }
  // free the node itself
  if (node->key != NULL) {
    free(node->key);
  }
  if (node->type == JSON_STRING && node->val.val_as_str != NULL) {
    free(node->val.val_as_str);
  }
  free(node);
}

void print_json(json_node *node, int indent) {
  if (node == NULL) {
    return;
  }

  int first = 1;

  // A Json Structure is a tree, so we can use recursive to print it.
  if (node->key == NULL) {
    if (node->type == JSON_OBJECT) {
      for (int i = 0; i < indent; i++) {
        printf(" ");
      }
      printf("{\n");
      print_json(node->child, indent + 2);
      if (node->child) {
        printf("\n");
      }
      for (int i = 0; i < indent; i++) {
        printf(" ");
      }
      printf("}");
    } else if (node->type == JSON_ARRAY) {
      for (int i = 0; i < indent; i++) {
        printf(" ");
      }
      // visit the array
      printf("[\n");
      json_node *start = node->child;
      while (start) {
        print_json(start, indent + 2);

        if (start->next)
          printf(",");
        printf("\n");
        start = start->next;
      }
      for (int i = 0; i < indent; i++) {
        printf(" ");
      }
      printf("]");
    } else {
      fprintf(stderr, "Error: Invalid Json Structure.\n");
    }
  } else {
    while (node != NULL) {
      if (first != 0) {
        first = 0;
      } else {
        // if (node->next || node->child)
        printf(",\n");
      }
      for (int i = 0; i < indent; i++) {
        printf(" ");
      }
      if (node->type == JSON_ARRAY) {
        printf("\"%s\": [\n", node->key);
        json_node *data = node->child;
        while (data) {
          print_json(data, indent + 2);
          if (data->next) {
            printf(",\n");
          }
          data = data->next;
        }
        printf("\n");
        for (int i = 0; i < indent; i++) {
          printf(" ");
        }
        printf("]");
      } else if (node->type == JSON_OBJECT) {
        printf("\"%s\": {\n", node->key);
        print_json(node->child, indent + 2);
        printf("\n");
        for (int i = 0; i < indent; i++) {
          printf(" ");
        }
        printf("}");
      } else if (node->type == JSON_STRING) {
        printf("\"%s\": \"%s\"", node->key, node->val.val_as_str);
      } else if (node->type == JSON_INT) {
        printf("\"%s\": %ld", node->key, node->val.val_as_int);
      } else if (node->type == JSON_DOUBLE) {
        printf("\"%s\": %f", node->key, node->val.val_as_double);
      } else if (node->type == JSON_TRUE) {
        printf("\"%s\": true", node->key);
      } else if (node->type == JSON_FALSE) {
        printf("\"%s\": false", node->key);
      } else if (node->type == JSON_NULL) {
        printf("\"%s\": null", node->key);
      } else {
        fprintf(stderr, "Error: Invalid Json Structure.\n");
      }
      node = node->next;
    }
  }
}
