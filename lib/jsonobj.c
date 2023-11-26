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

void print_json(json_node *node, int indent, FILE *fp) {
  if (node == NULL) {
    return;
  }

  int first = 1;

  // A Json Structure is a tree, so we can use recursive to print it.
  if (node->key == NULL) {
    if (node->type == JSON_OBJECT) {
      for (int i = 0; i < indent; i++) {
        fprintf(fp, " ");
      }
      fprintf(fp, "{\n");
      print_json(node->child, indent + 2, fp);
      if (node->child) {
        fprintf(fp, "\n");
      }
      for (int i = 0; i < indent; i++) {
        fprintf(fp, " ");
      }
      fprintf(fp, "}");
    } else if (node->type == JSON_ARRAY) {
      for (int i = 0; i < indent; i++) {
        fprintf(fp, " ");
      }
      // visit the array
      fprintf(fp, "[\n");
      json_node *start = node->child;
      while (start) {
        print_json(start, indent + 2, fp);

        if (start->next)
          fprintf(fp, ",");
        fprintf(fp, "\n");
        start = start->next;
      }
      for (int i = 0; i < indent; i++) {
        fprintf(fp, " ");
      }
      fprintf(fp, "]");
    } else {
      fprintf(stderr, "Error: Invalid Json Structure.\n");
    }
  } else {
    while (node != NULL) {
      if (first != 0) {
        first = 0;
      } else {
        // if (node->next || node->child)
        fprintf(fp, ",\n");
      }
      for (int i = 0; i < indent; i++) {
        fprintf(fp, " ");
      }
      if (node->type == JSON_ARRAY) {
        fprintf(fp, "\"%s\": [\n", node->key);
        json_node *data = node->child;
        while (data) {
          print_json(data, indent + 2, fp);
          if (data->next) {
            fprintf(fp, ",\n");
          }
          data = data->next;
        }
        fprintf(fp, "\n");
        for (int i = 0; i < indent; i++) {
          fprintf(fp, " ");
        }
        fprintf(fp, "]");
      } else if (node->type == JSON_OBJECT) {
        fprintf(fp, "\"%s\": {\n", node->key);
        print_json(node->child, indent + 2, fp);
        fprintf(fp, "\n");
        for (int i = 0; i < indent; i++) {
          fprintf(fp, " ");
        }
        fprintf(fp, "}");
      } else if (node->type == JSON_STRING) {
        fprintf(fp, "\"%s\": \"%s\"", node->key, node->val.val_as_str);
      } else if (node->type == JSON_INT) {
        fprintf(fp, "\"%s\": %ld", node->key, node->val.val_as_int);
      } else if (node->type == JSON_DOUBLE) {
        fprintf(fp, "\"%s\": %f", node->key, node->val.val_as_double);
      } else if (node->type == JSON_TRUE) {
        fprintf(fp, "\"%s\": true", node->key);
      } else if (node->type == JSON_FALSE) {
        fprintf(fp, "\"%s\": false", node->key);
      } else if (node->type == JSON_NULL) {
        fprintf(fp, "\"%s\": null", node->key);
      } else {
        fprintf(stderr, "Error: Invalid Json Structure.\n");
      }
      node = node->next;
    }
  }
}

json_node *add_child(json_node *parent, json_type t) {
  json_node *child = create_json_node();
  child->type = t;
  if (parent->child == NULL) {
    parent->child = child;
  } else {
    json_node *start = parent->child;
    while (start->next != NULL) {
      start = start->next;
    }
    start->next = child;
  }
  return child;
}

json_node *add_a_neighbor(json_node *node, json_type t) {
  json_node *neighbor = create_json_node();
  neighbor->type = t;
  if (node->next == NULL) {
    node->next = neighbor;
  } else {
    json_node *start = node->next;
    while (start->next != NULL) {
      start = start->next;
    }
    start->next = neighbor;
  }
  return neighbor;
}

json_node *json_get(json_node *node, const char *key) {
  if (node == NULL) {
    return NULL;
  }
  if (node->key == NULL) {
    return json_get(node->child, key);
  }
  if (strcmp(node->key, key) == 0) {
    return node;
  }
  return json_get(node->next, key);
}