#include <cstdio>
#include "jsonobj.h"
#include "jsonparser.h"
#include "logger.h"

// This function is used to show how to create a json object like
// {
//   "description" : "This is a json test file for testing the json parser",
//   "object" : {"list" : [
//     114,
//     514
//   ]},
//   "int" : 1,
//   "numerical" : 1.0,
//   "boolean" : {
//     "true" : true,
//     "false" : false
//   }
//   "null" : null
// }
json_node *usage_example() {
  TRACE();
  json_node *obj = new json_object();
  obj->set(
      "description",
      new json_string("This is a json test file for testing the json parser"));
  auto inner = obj->set("object", new json_object());
  auto json_list = inner->set("list", new json_array());
  json_list->push_back(new json_int(114));
  json_list->push_back(new json_int(514));
  obj->set("int", new json_int(1));
  obj->set("numerical", new json_double(1.0));
  auto bool_obj = obj->set("boolean", new json_object());
  bool_obj->set("true", new json_true());
  bool_obj->set("false", new json_false());
  obj->set("null", new json_null());
  return obj;
}

// TBD
json_node *read_example(const char *filename) {
  TRACE();
  return from_file(filename);
}

int main() {
  json_node *obj = usage_example();
  // note that the to_string method will add a '\n' at the end of the
  // json string
  auto jsonStr = to_string(obj);
  delete obj;
  std::cout << "Usage Example: \n" << jsonStr;
  std::cout.flush();

  obj = from_string(jsonStr);
  fflush(stdout);
  fprintf(stdout, "Parsed json str, get: \n");
  fflush(stdout);
  obj->print(0, stdout);
  fprintf(stdout, "\n");
  fflush(stdout);
  delete obj;

  auto obj2 = read_example("test.json");
  std::cout.flush();
  obj2->print(0, std::cout);
  // as here we need to flush the stream
  std::cout << std::endl;
  delete obj2;

  return 0;
}