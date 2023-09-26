#include "statement.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace ast {

using runtime::Closure;
using runtime::Context;
using runtime::ObjectHolder;

namespace {
const string ADD_METHOD = "__add__"s;
const string INIT_METHOD = "__init__"s;
}  // namespace

ObjectHolder Assignment::Execute(Closure& closure, Context& context) {
    closure[var_] = move(rv_->Execute(closure, context));
    return closure.at(var_);
}

Assignment::Assignment(std::string var, std::unique_ptr<Statement> rv)
    : var_(move(var)), rv_(move(rv)) {}

VariableValue::VariableValue(const std::string& var_name) {
    dotted_ids_.push_back(var_name);
}

VariableValue::VariableValue(std::vector<std::string> dotted_ids)
    : dotted_ids_(move(dotted_ids)) {}

ObjectHolder VariableValue::Execute(Closure& closure, Context& /*context*/) {
    Closure* ptr_clos = &closure;
    runtime::Closure::iterator it_object;

    for (const auto& variable : dotted_ids_) {
        it_object = ptr_clos->find(variable);
        if (ptr_clos->end() == it_object) throw runtime_error("Not found variable!");

        auto ptr_class = it_object->second.TryAs<runtime::ClassInstance>();
        if (!ptr_class) return it_object->second;

        ptr_clos = &ptr_class->Fields();
    }
    return it_object->second;
}

unique_ptr<Print> Print::Variable(const std::string& name) {
    return make_unique<Print>(make_unique<VariableValue>(name));
}

Print::Print(unique_ptr<Statement> argument) {
    args_.push_back(move(argument));
}

Print::Print(vector<unique_ptr<Statement>> args)
    : args_(move(args)) {}

ObjectHolder Print::Execute(Closure& closure, Context& context) {
    auto& out = context.GetOutputStream();
    ObjectHolder object;

    for (const auto& arg : args_) {
        if (arg != args_.front()) out << " ";

        object = arg->Execute(closure, context);

        if (object) object->Print(out, context);
        else out << "None";
    }
    out << "\n";
    return object;
}

MethodCall::MethodCall(std::unique_ptr<Statement> object, std::string method,
                       std::vector<std::unique_ptr<Statement>> args)
    : object_(move(object)), method_(move(method)), args_(move(args)) {}

ObjectHolder MethodCall::Execute(Closure& closure, Context& context) {
    auto ptr_class = object_->Execute(closure, context).TryAs<runtime::ClassInstance>();
    std::vector<runtime::ObjectHolder> args;

    for (const auto& arg : args_) {
        args.push_back(std::move(arg->Execute(closure, context)));
    }

    return ptr_class->Call(method_, args, context);
}

ObjectHolder Stringify::Execute(Closure& closure, Context& context) {
    auto object = argument_->Execute(closure, context);
    if (!object) return ObjectHolder::Own(runtime::String{ "None" });

    runtime::DummyContext context_dummy;
    object->Print(context_dummy.GetOutputStream(), context_dummy);

    return ObjectHolder::Own(runtime::String{ context_dummy.output.str() });
}

ObjectHolder Add::Execute(Closure& closure, Context& context) {
    ObjectHolder lhs = lhs_->Execute(closure, context);
    ObjectHolder rhs = rhs_->Execute(closure, context);

    auto val_lhs = lhs.TryAs<runtime::Number>();
    auto val_rhs = rhs.TryAs<runtime::Number>();
    if (val_lhs && val_rhs) return ObjectHolder::Own(runtime::Number{ val_lhs->GetValue() + val_rhs->GetValue() });

    auto val_lhs_str = lhs.TryAs<runtime::String>();
    auto val_rhs_str = rhs.TryAs<runtime::String>();
    if (val_lhs_str && val_rhs_str) return ObjectHolder::Own(runtime::String{ val_lhs_str->GetValue() + val_rhs_str->GetValue() });

    auto val = lhs.TryAs< runtime::ClassInstance>();
    if (val && val->HasMethod(ADD_METHOD, 1)) return val->Call(ADD_METHOD, { rhs }, context);

    throw runtime_error("Incorrect data types!");
}

ObjectHolder Sub::Execute(Closure& closure, Context& context) {
    ObjectHolder lhs = lhs_->Execute(closure, context);
    ObjectHolder rhs = rhs_->Execute(closure, context);

    auto val_lhs = lhs.TryAs<runtime::Number>();
    auto val_rhs = rhs.TryAs<runtime::Number>();
    if (val_lhs && val_rhs) return ObjectHolder::Own(runtime::Number{ val_lhs->GetValue() - val_rhs->GetValue() });

    throw runtime_error("Incorrect data types for subtraction!");
}

ObjectHolder Mult::Execute(Closure& closure, Context& context) {
    ObjectHolder lhs = lhs_->Execute(closure, context);
    ObjectHolder rhs = rhs_->Execute(closure, context);

    auto val_lhs = lhs.TryAs<runtime::Number>();
    auto val_rhs = rhs.TryAs<runtime::Number>();
    if (val_lhs && val_rhs) return ObjectHolder::Own(runtime::Number{ val_lhs->GetValue() * val_rhs->GetValue() });

    throw runtime_error("Incorrect data types for multiplication!");
}

ObjectHolder Div::Execute(Closure& closure, Context& context) {
    ObjectHolder lhs = lhs_->Execute(closure, context);
    ObjectHolder rhs = rhs_->Execute(closure, context);

    auto val_lhs = lhs.TryAs<runtime::Number>();
    auto val_rhs = rhs.TryAs<runtime::Number>();
    if (val_lhs && val_rhs) {
        if (val_rhs->GetValue() == 0) throw runtime_error("You can't divide by zero!");
        return ObjectHolder::Own(runtime::Number{ val_lhs->GetValue() / val_rhs->GetValue() });
    }
    throw runtime_error("Incorrect data types for division!");
}

ObjectHolder Compound::Execute(Closure& closure, Context& context) {
    for (const auto& stmt : statements_) {
        stmt->Execute(closure, context);
    }
    return {};
}

ReturnException::ReturnException(const runtime::ObjectHolder& object)
    : object_(object) {}

runtime::ObjectHolder ReturnException::GetValue() {
    return object_;
}

ObjectHolder Return::Execute(Closure& closure, Context& context) {
    auto object = statement_->Execute(closure, context);
    throw ReturnException(object);
}

ClassDefinition::ClassDefinition(ObjectHolder cls)
    : class_(move(cls)) {}

ObjectHolder ClassDefinition::Execute(Closure& closure, Context& /*context*/) {
    closure[class_.TryAs<runtime::Class>()->GetName()] = std::move(class_);
    return {};
}

FieldAssignment::FieldAssignment(VariableValue object, std::string field_name,
                                 std::unique_ptr<Statement> rv)
    : object_(move(object)), field_name_(move(field_name)), rv_(move(rv)) {}

ObjectHolder FieldAssignment::Execute(Closure& closure, Context& context) {
    auto ptr_class = object_.Execute(closure, context).TryAs<runtime::ClassInstance>();
    ptr_class->Fields()[field_name_] = std::move(rv_->Execute(closure, context));

    return ptr_class->Fields()[field_name_];
}

IfElse::IfElse(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> if_body,
               std::unique_ptr<Statement> else_body)
    : condition_(move(condition)), if_body_(move(if_body)), else_body_(move(else_body)) {}

ObjectHolder IfElse::Execute(Closure& closure, Context& context) {
    if (runtime::IsTrue(condition_->Execute(closure, context))) {
        return if_body_->Execute(closure, context);
    }
    else {
        if (else_body_) return else_body_->Execute(closure, context);
        else return {};
    }
}

ObjectHolder Or::Execute(Closure& closure, Context& context) {
    auto lhs = lhs_->Execute(closure, context);
    auto rhs = rhs_->Execute(closure, context);

    if (runtime::IsTrue(lhs)) return ObjectHolder::Own(runtime::Bool{ true });
    return ObjectHolder::Own(runtime::Bool{ runtime::IsTrue(rhs) });
}

ObjectHolder And::Execute(Closure& closure, Context& context) {
    auto lhs = lhs_->Execute(closure, context);
    auto rhs = rhs_->Execute(closure, context);

    if (!runtime::IsTrue(lhs)) return ObjectHolder::Own(runtime::Bool{ false });
    return ObjectHolder::Own(runtime::Bool{ runtime::IsTrue(lhs) && runtime::IsTrue(rhs) });
}

ObjectHolder Not::Execute(Closure& closure, Context& context) {
    auto value = argument_->Execute(closure, context);
    return ObjectHolder::Own(runtime::Bool{ !runtime::IsTrue(value) });
}

Comparison::Comparison(Comparator cmp, unique_ptr<Statement> lhs, unique_ptr<Statement> rhs)
    : BinaryOperation(std::move(lhs), std::move(rhs)), cmp_(move(cmp)) {}

ObjectHolder Comparison::Execute(Closure& closure, Context& context) {
    auto lhs = lhs_->Execute(closure, context);
    auto rhs = rhs_->Execute(closure, context);

    return ObjectHolder::Own(runtime::Bool{ cmp_(lhs, rhs, context) });
}

NewInstance::NewInstance(const runtime::Class& cls, std::vector<std::unique_ptr<Statement>> args)
    : class_(cls), args_(move(args)) {}

NewInstance::NewInstance(const runtime::Class& cls)
    : class_(cls) {}

ObjectHolder NewInstance::Execute(Closure& closure, Context& context) {
    vector<runtime::ObjectHolder> args;
    for (const auto& arg : args_) {
        args.push_back(move(arg->Execute(closure, context)));
    }
    if (class_.HasMethod(INIT_METHOD, args_.size())) class_.Call(INIT_METHOD, move(args), context);
    return runtime::ObjectHolder::Share(class_);
}

MethodBody::MethodBody(std::unique_ptr<Statement>&& body)
    : body_(move(body)) {}

ObjectHolder MethodBody::Execute(Closure& closure, Context& context) {
    try {
        body_->Execute(closure, context);
    }
    catch (ReturnException& object) {
        return object.GetValue();
    }
    return {};
}

}  // namespace ast