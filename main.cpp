#include "jsonobj.h"

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
json_node *write_example() {
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

int main() {
  json_node *obj = write_example();
  obj->print(0, stdout);
  delete obj;
  return 0;
}