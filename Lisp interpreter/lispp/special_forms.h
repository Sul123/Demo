#include "base_object.h"

namespace Special {
    ObjectPtr If(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr Quote(ObjectPtr arg);
    ObjectPtr Lambda(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr And(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr Or(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr Define(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr Set(ObjectPtr arg, std::shared_ptr<Scope> scope);
    ObjectPtr Eval(ObjectPtr arg, std::shared_ptr<Scope> scope);
}