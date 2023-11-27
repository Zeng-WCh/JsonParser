#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonobj.h"
#include "jsonparser.h"

const char *msg = "Usage: %s <input_json_file>\n";

int main(int argc, const char **argv) {
  if (argc < 2) {
    fprintf(stderr, msg, argv[0]);
    return EXIT_FAILURE;
  }

  json_node *f = parse_json_file(argv[1]);
  print_json(f, 0, stdout);

  free_json_node(f);

  return 0;
}