#pragma once
#include <memory>
#include "base_object.h"


class FunctionObject;

class Scope;

class PairObject : public BaseObject {
public:
    PairObject(const PairObject& other);

    PairObject(const ObjectPtr& car, const ObjectPtr& cdr);

    ObjectPtr Duplicate() const override;

    ObjectPtr Evaluate(ScopePtr scope) override;

    const ObjectPtr& Car() const;

    const ObjectPtr& Cdr() const;

    ObjectPtr& Car();

    ObjectPtr& Cdr();

    void SetCar(const ObjectPtr& car);

    void SetCdr(const ObjectPtr& cdr);

    bool IsPair() const override;

    bool IsList() const override;

    int64_t ListSize() const;

    ObjectPtr ListGet(size_t index) const;

    std::vector<ObjectPtr> ExtractEvaluatedFromList(ScopePtr scope);

    std::vector<ObjectPtr> ExtractFromList() const;

    std::string Print() const override;

    std::string Type() const override {
        return "pair";
    }

private:
    ObjectPtr car_;
    ObjectPtr cdr_;
};