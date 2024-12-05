#include "jsonobj.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ostream>

namespace {
// helper function to print ident
void print_ident(size_t ident, FILE *fp) {
  for (size_t i = 0; i < ident; i++) {
    fprintf(fp, "  ");
  }
}

void print_ident(size_t ident, std::ostream &os) {
  for (size_t i = 0; i < ident; i++) {
    os << "  ";
  }
}
} // namespace

// Make compiler happy
json_node::~json_node() {}

json_type json_null::getType() const { return type; }

void json_null::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "null");
}

void json_null::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << "null";
}

json_type json_int::getType() const { return type; }
int64_t json_int::getAsInt() const { return val; }
double json_int::getAsDouble() const {
  return static_cast<double>(val);
} // implicit conversion

bool json_int::getAsBool() const { return val != 0; } // implicit conversion

void json_int::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "%ld", val);
}

void json_int::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << val;
}

json_type json_double::getType() const { return type; }
double json_double::getAsDouble() const { return val; }
int64_t json_double::getAsInt() const { return static_cast<int64_t>(val); }
bool json_double::getAsBool() const { return val != 0; }

void json_double::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "%lf", val);
}

void json_double::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << val;
}

json_string::json_string(const std::string &val) : len(val.size()) {
  // first find if the string is already in the set
  auto it = str_set.find(val);
  if (it == str_set.end()) {
    // if not found, insert it
    it = str_set.insert(val).first;
    // use the reference
    this->val = it->data();
  } else {
    // if found, use the reference
    this->val = it->data();
  }
}

json_type json_string::getType() const { return type; }

const char *json_string::getAsStr() const { return val; }

void json_string::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "\"%s\"", val);
}

void json_string::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << "\"" << val << "\"";
}

json_bool::~json_bool() {}

json_type json_true::getType() const { return type; }
bool json_true::getAsBool() const { return true; }

void json_true::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "true");
}

void json_true::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << "true";
}

json_type json_false::getType() const { return type; }
bool json_false::getAsBool() const { return false; }

void json_false::print(size_t ident, bool f, FILE *fp) const {
  // print_ident(ident, fp);
  fprintf(fp, "false");
}

void json_false::print(size_t ident, bool f, std::ostream &os) const {
  // print_ident(ident, os);
  os << "false";
}

json_array::~json_array() {
  for (auto &node : val) {
    delete node;
  }
}

json_type json_array::getType() const { return type; }

json_node *json_array::operator[](size_t index) { return get(index); }

json_node *json_array::get(size_t index) {
  if (index >= val.size()) {
    return nullptr;
  }
  return val[index];
}

void json_array::push_back(json_node *node) { val.push_back(node); }

void json_array::print(FILE *fp) const { print(0, true, fp); }

void json_array::print(std::ostream &os) const { print(0, true, os); }

void json_array::print(size_t ident, bool f, FILE *fp) const {
  if (f)
    print_ident(ident, fp);
  fprintf(fp, "[\n");
  for (size_t i = 0; i < val.size(); i++) {
    print_ident(ident + 4, fp);
    val[i]->print(ident + 4, false, fp);
    if (i != val.size() - 1) {
      fprintf(fp, ",");
    }
    fprintf(fp, "\n");
  }
  print_ident(ident, fp);
  fprintf(fp, "]");
}

void json_array::print(size_t ident, bool f, std::ostream &os) const {
  if (f)
    print_ident(ident, os);
  os << "[\n";
  for (size_t i = 0; i < val.size(); i++) {
    print_ident(ident + 4, os);
    val[i]->print(ident + 4, false, os);
    if (i != val.size() - 1) {
      os << ",";
    }
    os << "\n";
  }
  print_ident(ident, os);
  os << "]";
}

void json_object::print(size_t ident, bool f, FILE *fp) const {
  if (f)
    print_ident(ident, fp);
  fprintf(fp, "{\n");
  for (size_t i = 0; i < keys.size(); i++) {
    print_ident(ident + 4, fp);
    fprintf(fp, "\"%s\": ", keys[i].c_str());
    val.at(keys[i])->print(ident + 4, false, fp);
    if (i != keys.size() - 1) {
      fprintf(fp, ",");
    }
    fprintf(fp, "\n");
  }
  print_ident(ident, fp);
  fprintf(fp, "}");
}

void json_object::print(size_t ident, bool f, std::ostream &os) const {
  if (f)
    print_ident(ident, os);
  os << "{\n";
  for (size_t i = 0; i < keys.size(); i++) {
    print_ident(ident + 4, os);
    os << "\"" << keys[i] << "\": ";
    // if (val.at(keys[i])->getType() == JSON_OBJECT ||
    //     val.at(keys[i])->getType() == JSON_ARRAY) {
    //   os << "\"";
    // }
    val.at(keys[i])->print(ident + 4, false, os);
    if (i != keys.size() - 1) {
      os << ",";
    }
    os << "\n";
  }
  print_ident(ident, os);
  os << "}";
}

void json_object::print(FILE *fp) const { print(0, true, fp); }
void json_object::print(std::ostream &os) const { print(0, true, os); }

json_object::~json_object() {
  for (auto &pair : val) {
    delete pair.second;
  }
}

json_type json_object::getType() const { return type; }

json_node *json_object::get(const std::string &key) {
  auto it = val.find(key);
  if (it == val.end()) {
    return nullptr;
  }
  return it->second;
}
json_node *json_object::operator[](const std::string &key) { return get(key); }

json_node *json_object::set(const std::string &key, json_node *node) {
  auto it = val.find(key);
  if (it != val.end()) {
    delete it->second;
  }
  val[key] = node;
  if (it == val.end())
    keys.push_back(key);
  return node;
}

std::size_t json_object::size() const { return keys.size(); }

std::size_t json_array::size() const { return val.size(); }

std::size_t json_null::size() const { return 0l; }

std::size_t json_string::size() const { return len; }

std::unordered_set<std::string> json_string::str_set;

// void json_object::merge(json_object *obj) {
//   if (!obj) {
//     return;
//   }
//   for (auto &pair : obj->val) {
//     set(pair.first, pair.second);
//   }
// }

// void json_array::merge(json_array *arr) {
//   if (!arr) {
//     return;
//   }
//   for (auto &node : arr->val) {
//     push_back(node);
//   }
// }