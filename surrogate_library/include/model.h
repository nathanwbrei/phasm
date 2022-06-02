
// Copyright 2022, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.


#ifndef SURROGATE_TOOLKIT_MODEL_H
#define SURROGATE_TOOLKIT_MODEL_H

#include "model_variable.h"
#include "call_site_variable.h"
#include <iosfwd>  // Forward decls for std::ostream
#include <memory>

namespace phasm {

class OpticBuilder;

class Surrogate;

/// There should be exactly one Model in your codebase for each unique function that you wish to surrogate.
/// Contrast this with Surrogate. There should be one Surrogate for each call site of that function,
/// and each of these Surrogates delegate to the same underlying model.
class Model {
    friend class Surrogate;

protected:
    std::vector<std::shared_ptr<CallSiteVariable>> callsite_vars;
    std::map<std::string, std::shared_ptr<CallSiteVariable>> callsite_var_map;
    std::vector<std::shared_ptr<ModelVariable>> model_vars;
    std::map<std::string, std::shared_ptr<ModelVariable>> model_var_map;

    // The following two are just for convenience
    std::vector<std::shared_ptr<ModelVariable>> inputs;
    std::vector<std::shared_ptr<ModelVariable>> outputs;
    size_t captured_rows = 0;

public:
    Model() = default;

    Model(const OpticBuilder &b);

    virtual ~Model() = default; // We want to be able to inherit from this

    // Initialize the underlying neural net once all the inputs and outputs are known
    virtual void initialize() {};

    // Performs tasks such as training or writing to CSV, right before the model gets destroyed
    void finalize();

    size_t get_capture_count() const;

    // Retrieve a model variable by position
    std::shared_ptr<ModelVariable> get_model_var(size_t position);

    // Retrieve a model variable by name
    std::shared_ptr<ModelVariable> get_model_var(std::string param_name);

    // Train takes all of the captures associated with each parameter
    virtual void train_from_captures() {};

    // Infer takes the sample associated with each parameter
    virtual void infer(Surrogate &) {};

    void dump_captures_to_csv(std::ostream &);

    void dump_ranges(std::ostream &);

    virtual void save();

    template<typename T>
    void add_var(std::string param_name, Direction dir);

    template<typename T>
    void add_var(std::string call_site_var_name, Optic <T> *accessor, std::string model_var_name, Direction dir);


};

template<typename T>
void Model::add_var(std::string call_site_var_name, Direction dir) {
    add_var(call_site_var_name, new Primitive<T>, call_site_var_name, dir);
}


template<typename T>
void Model::add_var(std::string call_site_var_name, Optic<T> *accessor, std::string model_var_name, Direction dir) {
    auto mv = std::make_shared<ModelVariable>();
    mv->name = model_var_name;
    mv->is_input = (dir == Direction::Input) || (dir == Direction::InputOutput);
    mv->is_output = (dir == Direction::Output) || (dir == Direction::InputOutput);
    mv->accessor = accessor;
    if (model_var_map.find(model_var_name) != model_var_map.end()) {
        throw std::runtime_error("Model variable already exists!");
    }
    model_vars.push_back(mv);
    model_var_map[model_var_name] = mv;
    if (mv->is_input) inputs.push_back(mv);
    if (mv->is_output) outputs.push_back(mv);

    std::shared_ptr<CallSiteVariable> csv = nullptr;
    auto pair = callsite_var_map.find(call_site_var_name);
    if (pair == callsite_var_map.end()) {
        csv = std::make_shared<CallSiteVariable>();
        csv->name = call_site_var_name;
        csv->binding = phasm::any_ptr((T *) nullptr);
        callsite_var_map[call_site_var_name] = csv;
        callsite_vars.push_back(csv);
    } else {
        csv = pair->second;
    }
    csv->model_vars.push_back(mv);
}


} // namespace phasm
#endif //SURROGATE_TOOLKIT_MODEL_H
