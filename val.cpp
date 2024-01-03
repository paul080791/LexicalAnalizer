#include "val.h"

Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        if (op.GetInt() == 0) {
            return Value();

        }
        return Value(GetInt() / op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        if (op.GetReal() == 0.0) {
            return Value();

        }
        return Value(GetReal() / op.GetReal());
    }
    else if ((IsInt() || IsReal()) && (op.IsString() || op.IsBool())) {
        return Value();

    }
    else if ((op.IsInt() || op.IsReal()) && (IsString() || IsBool())) {
        return Value();

    }
    else  if (IsReal() && op.IsInt()) {
        if (op.GetInt() == 0) {
            return Value();
        }
        return Value(GetReal() / op.GetInt());
    }
    else  if (IsReal() && op.IsReal()) {
        if (op.GetReal() == 0) {
            return Value();
        }
        return Value();
    }
    else  if (op.IsInt() && IsReal()) {
        if (GetReal() == 0) {
            return Value();
        }
        return Value(GetReal() / op.GetInt());
    }
    else  if (op.IsReal() && IsInt()) {
        if (GetInt() == 0) {
            return Value();
        }
        return Value(GetInt() / op.GetReal());
    }
    else {
        return Value();

    }
}
Value Value::operator%(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        if (op.GetInt() == 0) {
            return Value();
        }
        return Value(GetInt() % op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {

        return Value();
    }
    else {
        return Value();
    }
}

Value Value::operator==(const Value& op) const {
    if (T != op.T) {
        if ((T == VINT || T == VREAL) || (op.T == VINT || op.T == VREAL))
        {
            if (T == VINT && op.T == VREAL)
                return Value(Itemp == op.Rtemp);
            else if (T == VREAL && op.T == VINT)
                return Value(Rtemp == op.Itemp);
        }
        return Value();
    }
    else {
        switch (T) {
        case VINT:
            return Value(Itemp == op.Itemp);
        case VREAL:
            return Value(Rtemp == op.Rtemp);
        case VSTRING:
            return Value(Stemp == op.Stemp);
        case VBOOL:
            return Value(Btemp == op.Btemp);
        case VERR:
            throw std::runtime_error("Error");
        default:
            throw std::runtime_error("Error");
        }
    }
}
Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() + op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() + op.GetReal());
    }
    else if (IsString() && op.IsString()) {
        return Value(GetString() + op.GetString());
    }
    else  if (op.IsInt() && IsReal()) {
       return Value(GetReal() + op.GetInt());
    }
    else  if (op.IsReal() && IsInt()) {
       return Value(GetInt() + op.GetReal());
    }
    //else if ((IsInt() && op.IsReal()) || (IsReal() && op.IsInt())) {
      //  return Value(GetReal() + op.GetReal());
   // }
    else {
        return Value();
    }
}
Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() - op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() - op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator*(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() * op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() * op.GetReal());
    }
    else  if (op.IsInt() && IsReal()) {
        return Value(GetReal() * op.GetInt());
    }
    else  if (op.IsReal() && IsInt()) {
        return Value(GetInt() * op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::div(const Value& oper) const {
    if (IsInt() && oper.IsInt()) {
        if (oper.GetInt() != 0) {
            return Value(GetInt() / oper.GetInt());
        }
    }
    else if (IsReal() && oper.IsReal()) {
        if (oper.GetReal() != 0.0) {
            return Value(floor(GetReal() / oper.GetReal()));
        }
    }
    // Handle other cases or division by zero
    return Value();
}
Value Value::operator>(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() > op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() > op.GetReal());
    }
    else if (IsString() && op.IsString()) {
        return Value(GetString() > op.GetString());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() > op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() > op.GetInt());
    }
    else {
        // Handle other cases, return an error or default value
        return Value();
    }
}
Value Value::operator<(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() < op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() < op.GetReal());
    }
    else if (IsString() && op.IsString()) {
        return Value(GetString() < op.GetString());
    }
    else if (IsInt() && op.IsReal()) {
        return Value(GetInt() < op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() < op.GetInt());
    }
    else {
        // Handle other cases, return an error or default value
        return Value();
    }
}
Value Value::idiv(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        if (op.GetInt() != 0) {
            return Value(GetInt() / op.GetInt());
        }
        else {
            throw std::runtime_error("Run-Time Error-Illegal integer division (div) by Zero");
        }
    }
    else {
        // Handle other cases or incompatible types
        return Value();
    }
}
Value Value::operator&&(const Value& oper) const {
    if (IsBool() && oper.IsBool()) {
        return Value(GetBool() && oper.GetBool());
    }
    else {
        // Handle other cases or incompatible types
        return Value();
    }
}
Value Value::operator||(const Value& oper) const {
    if (IsBool() && oper.IsBool()) {
        return Value(GetBool() || oper.GetBool());
    }
    else {
        // Handle other cases or incompatible types
        return Value();
    }
}
Value Value::operator!() const {
    if (IsBool()) {
        return Value(!GetBool());
    }
    else {
        // Handle other cases or incompatible types
        return Value();
    }
}


