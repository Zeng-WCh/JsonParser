#pragma once

#ifndef __JSON_OBJ_HPP__
#define __JSON_OBJ_HPP__

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum json_type {
  JSON_NULL = 0,
  JSON_INT = 257,
  JSON_DOUBLE,
  JSON_STRING,
  JSON_TRUE,
  JSON_FALSE,
  JSON_ARRAY,
  JSON_OBJECT
};

/**
 * @brief JSON object base class
 * All JSON objects should inherit from this class
 * This is a pure virtual class
 */
class json_node {
public:
  virtual ~json_node() = 0;

public:
  // helper func
  virtual void print(size_t ident, bool need = false,
                     FILE *fp = stdout) const = 0;
  inline virtual void print(size_t ident, FILE *fp = stdout) const {
    this->print(ident, false, fp);
  }
  virtual void print(size_t ident, bool need = false,
                     std::ostream &os = std::cout) const = 0;
  inline virtual void print(size_t ident, std::ostream &os = std::cout) const {
    this->print(ident, false, os);
  }

public:
  inline virtual void print(FILE *fp = stdout) const { print(0, false, fp); }
  inline virtual void print(std::ostream &os = std::cout) const {
    print(0, false, os);
  }
  // datatype func
  virtual json_type getType() const = 0;

  // some data get/set functions
  inline virtual int64_t getAsInt() const { abort(); }
  inline virtual double getAsDouble() const { abort(); }
  inline virtual const char *getAsStr() const { abort(); }
  inline virtual bool getAsBool() const { abort(); }

  inline virtual json_node *operator[](const std::string &key) { abort(); }
  inline virtual json_node *operator[](size_t index) { abort(); }

  // get func
  inline virtual json_node *get(const std::string &key) { abort(); }
  inline virtual json_node *get(size_t index) { abort(); }

  // for k-v pair in object
  inline virtual json_node *set(const std::string &key, json_node *val) {
    abort();
  }

  // for array
  inline virtual void push_back(json_node *val) { abort(); }

  inline virtual size_t size() const { return 1l; }
};

// Json basic type
// null, int, double, string, boolean, array, object

// Json NULL type
class json_null : public json_node {
private:
  const static json_type type = JSON_NULL;

public:
  json_null() = default;
  ~json_null() = default;

  json_type getType() const override;

  size_t size() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

// Json integer type
class json_int : public json_node {
private:
  const static json_type type = JSON_INT;
  int64_t val;

public:
  explicit inline json_int(int64_t val = 0) : val(val) {}
  inline ~json_int() = default;

  json_type getType() const override;
  int64_t getAsInt() const override;
  double getAsDouble() const override;

  bool getAsBool() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

class json_double : public json_node {
private:
  const static json_type type = JSON_DOUBLE;
  double val;

public:
  explicit json_double(double val = 0.0) : val(val) {}

  json_type getType() const override;
  double getAsDouble() const override;
  int64_t getAsInt() const override;
  bool getAsBool() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

class json_string : public json_node {
private:
  const static json_type type = JSON_STRING;
  // A hash set to store all string values, so that when we have the same string
  // value, we can reuse it
  static std::unordered_set<std::string> str_set;
  // because we already have a hash set to store all string values, we can use a
  // pointer points to the string value in the hash set
  const char *val;
  const std::size_t len;

public:
  json_string(const std::string &val);

  json_type getType() const override;

  const char *getAsStr() const override;

  size_t size() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

class json_bool : public json_node {
public:
  virtual bool getAsBool() const override = 0;
  virtual json_type getType() const override = 0;
  virtual ~json_bool() = 0;

private:
  virtual void print(size_t ident, bool f, FILE *fp) const override = 0;
  virtual void print(size_t ident, bool f, std::ostream &os) const override = 0;

public:
  inline virtual int64_t getAsInt() const override { return getAsBool(); }
  inline virtual double getAsDouble() const override { return getAsBool(); }
};

class json_true : public json_bool {
private:
  const static json_type type = JSON_TRUE;

public:
  inline json_true() = default;
  inline ~json_true() = default;

  json_type getType() const override;
  bool getAsBool() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

class json_false : public json_bool {
private:
  const static json_type type = JSON_FALSE;

public:
  inline json_false() = default;
  inline ~json_false() = default;

  json_type getType() const override;
  bool getAsBool() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

// Json data structure type
// Array, Object
class json_array : public json_node {
private:
  const static json_type type = JSON_ARRAY;
  // Array should take control of the life cycle of its elements
  // so when the array is destructed, all its elements should be destructed by
  // the array
  std::vector<json_node *> val;

public:
  inline json_array() : val(){};

  ~json_array();

  json_type getType() const override;

  json_node *operator[](size_t index) override;

  json_node *get(size_t index) override;

  void push_back(json_node *node) override;
  void print(FILE *fp) const override;
  void print(std::ostream &os) const override;

  size_t size() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

// for json object, we use a hash map to store the key-value pairs
class json_object : public json_node {
private:
  const static json_type type = JSON_OBJECT;
  // Same as array, object should take control of the life cycle of its elements
  std::unordered_map<std::string, json_node *> val;
  std::vector<std::string> keys; // used to keep the order of the keys

public:
  inline json_object() : val() {}

  ~json_object();

  json_type getType() const override;

  json_node *get(const std::string &key) override;
  json_node *operator[](const std::string &key) override;

  json_node *set(const std::string &key, json_node *node) override;
  void print(FILE *fp) const override;
  void print(std::ostream &os) const override;

  size_t size() const override;

private:
  void print(size_t ident, bool f, FILE *fp) const override;
  void print(size_t ident, bool f, std::ostream &os) const override;
};

#endif
