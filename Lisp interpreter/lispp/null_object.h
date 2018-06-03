#include "base_object.h"

class NullObject : public BaseObject {
public:
    NullObject();

    explicit NullObject(bool printable);

    ObjectPtr Evaluate(ScopePtr scope) override;

    ObjectPtr Duplicate() const override;

    std::string Print() const override;

    bool IsList() const override;

    bool IsNull() const override;

    bool Printable() const override;

    std::string Type() const override {
        return "null";
    }
private:
    bool printable_ = true;
};