#pragma once
#include <memory>
#include <map>
#include <vector>
#include "base_object.h"

class BaseObject;
using ObjectPtr = std::shared_ptr<BaseObject>;

class Scope;
using ScopePtr = std::shared_ptr<Scope>;


class Scope {
public:
    Scope();

    explicit Scope(ScopePtr parent_scope);

    Scope(const std::vector<std::string>& args,
          const std::vector<ObjectPtr>& arg_values,
          ScopePtr outer_scope);

    Scope(const Scope& other);

    ObjectPtr Get(const std::string& name) const;

    void Define(const std::string& name, ObjectPtr object);

    void Set(const std::string& name, ObjectPtr object);

    ~Scope();

    void Reset();

private:
    std::map<std::string, ObjectPtr> name_to_obj_;
    ScopePtr parent_scope_;
};

Scope MakeGlobalScope();