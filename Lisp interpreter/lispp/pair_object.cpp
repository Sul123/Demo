#include "pair_object.h"
#include "symbol_object.h"
#include "function_object.h"
#include "errors.h"
#include "special_forms.h"

PairObject::PairObject(const PairObject& other) {

    car_ = other.car_->Duplicate();
    cdr_ = other.cdr_->Duplicate();

}

PairObject::PairObject(const ObjectPtr& car, const ObjectPtr& cdr)
        : car_(car), cdr_(cdr) {}

ObjectPtr PairObject::Duplicate() const {
    return std::make_shared<PairObject>(*this);
}

ObjectPtr PairObject::Evaluate(ScopePtr scope) {
    if (car_->IsSymbol()) {
        std::string symbol = (std::dynamic_pointer_cast<SymbolObject>(car_))->GetSymbol();

        //////////**********Try Special Form*********////////////////
        if (symbol == "if") {
            return Special::If(cdr_, scope);
        }
        if (symbol == "lambda") {
            return Special::Lambda(cdr_, scope);
        }
        if (symbol == "and") {
            return Special::And(cdr_, scope);
        }
        if (symbol == "or") {
            return Special::Or(cdr_, scope);
        }
        if (symbol == "define") {
            return Special::Define(cdr_, scope);
        }
        if (symbol == "set!") {
            return Special::Set(cdr_, scope);
        }
        if (symbol == "eval") {
            return Special::Eval(cdr_, scope);
        }

    }
    /////////***********Try Function*************////////////////
    ObjectPtr func_candidate = car_->Evaluate(scope);

    if (!func_candidate->IsFunction()) {
        throw RuntimeError("RuntimeError, first element is not a function");
    }
    auto function = std::dynamic_pointer_cast<FunctionObject>(func_candidate);
    if (!cdr_->IsList()) {
        throw RuntimeError("RuntimeError, argument's isn't a list");
    }
    std::vector<ObjectPtr> arg_values;
    if (!cdr_->IsNull()) {
        arg_values = std::dynamic_pointer_cast<PairObject>(cdr_)->ExtractEvaluatedFromList(scope);
    }
    return function->Apply(arg_values);
}

const ObjectPtr& PairObject::Car() const {
    return car_;
}

const ObjectPtr& PairObject::Cdr() const {
    return cdr_;
}

ObjectPtr& PairObject::Car() {
    return car_;
}

ObjectPtr& PairObject::Cdr() {
    return cdr_;
}

void PairObject::SetCar(const ObjectPtr& car) {
    car_ = car;
}

void PairObject::SetCdr(const ObjectPtr& cdr) {
    cdr_ = cdr;
}

bool PairObject::IsPair() const {
    return true;
}

bool PairObject::IsList() const {
    if (cdr_->IsNull()) {
        return true;
    }
    return cdr_->IsList();
}

int64_t PairObject::ListSize() const {
    if (!IsList()) {
        throw RuntimeError("Runtime Error, argument is not a list");
    }
    int64_t counter = 1;
    std::shared_ptr<PairObject> current_pair_ptr =
            std::dynamic_pointer_cast<PairObject>(Duplicate());
    while (!current_pair_ptr->cdr_->IsNull()) {
        current_pair_ptr = std::dynamic_pointer_cast<PairObject>(current_pair_ptr->cdr_);
        ++counter;
    }
    return counter;
}

ObjectPtr PairObject::ListGet(size_t index) const {
    if (!IsList()) {
        throw RuntimeError("Runtime Error, argument is not a list");
    }
    if (ListSize() <= index) {
        throw RuntimeError("Runtime Error, list index out of range");
    }
    std::shared_ptr<PairObject> current_pair_ptr = std::dynamic_pointer_cast<PairObject>(Duplicate());
    size_t counter = 0;
    while (counter < index && !current_pair_ptr->cdr_->IsNull()) {
        current_pair_ptr = std::dynamic_pointer_cast<PairObject>(current_pair_ptr->cdr_);
        ++counter;
    }
    return current_pair_ptr->car_;
}

std::vector<ObjectPtr> PairObject::ExtractFromList() const {
    if (!IsList()) {
        throw RuntimeError("Runtime error, not a list");
    }
    std::vector<ObjectPtr> elements;
    ObjectPtr current_pair_ptr = Duplicate();
    ObjectPtr current_value;
    while (!current_pair_ptr->IsNull()) {
        current_value =
                std::dynamic_pointer_cast<PairObject>(current_pair_ptr)->car_;
        if (!current_value) {
            throw RuntimeError("Runtime error, argument is nothing");
        }
        elements.push_back(current_value);
        current_pair_ptr = std::dynamic_pointer_cast<PairObject>(current_pair_ptr)->cdr_;
    }

    return elements;
}

std::vector<ObjectPtr> PairObject::ExtractEvaluatedFromList(ScopePtr scope) {
    if (!IsList()) {
        throw RuntimeError("RuntimeError, not a list");
    }
    std::vector<ObjectPtr> elements;
    ObjectPtr current_pair_ptr = Duplicate();
    ObjectPtr current_value;
    while (!current_pair_ptr->IsNull()) {
        current_value = std::dynamic_pointer_cast<PairObject>(current_pair_ptr)->car_->Evaluate(scope);
        if (!current_value) {
            throw RuntimeError("Runtime error, argument is nothing");
        }
        elements.push_back(current_value);
        current_pair_ptr = std::dynamic_pointer_cast<PairObject>(current_pair_ptr)->cdr_;
    }

    return elements;
}

std::string PairObject::Print() const {
    std::shared_ptr<PairObject> current_pair_ptr = std::dynamic_pointer_cast<PairObject>(Duplicate());
    std::string result;

    result += '(' + car_->Print();
    while (!current_pair_ptr->cdr_->IsNull() && current_pair_ptr->cdr_->IsPair()) {
    current_pair_ptr = std::dynamic_pointer_cast<PairObject>(current_pair_ptr->cdr_);
    result += " ";
    result += current_pair_ptr->car_->Print();
    }
    if (!current_pair_ptr->cdr_->IsNull()) {
    result += " . ";
    result += current_pair_ptr->cdr_->Print();
    }
    result += ")";
    return result;
}
