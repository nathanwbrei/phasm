
// Copyright 2021, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.


#ifndef SURROGATE_TOOLKIT_EXPERIMENTS_H
#define SURROGATE_TOOLKIT_EXPERIMENTS_H

/// These are small scale experiments to build up familiarity with C++20 template metaprogramming.
/// The problem with template metaprogramming is that the pattern language gets lost in the very nonobvious
/// implementation details. So we build up the patterns in isolation first.

#include <any>
#include <tuple>

namespace phasm::test::experiments {

template <typename ReturnT, typename ...ArgsT>
class WrappedFunction {
    std::function<ReturnT(ArgsT...)> m_original;
public:
    WrappedFunction(std::function<ReturnT(ArgsT...)> f) : m_original(f) {};

    ReturnT operator()(ArgsT&&... args) {
        std::cout << "Wrapping original function" << std::endl;
        return m_original(std::forward<ArgsT>(args)...);
    }
};



template <typename ReturnT, typename ...ArgsT>
class MemoizedFunction {
    std::function<ReturnT(ArgsT...)> m_original;
    std::map<std::tuple<ArgsT...>, ReturnT> m_lookup_table;

public:
    bool was_last_call_memoized = false;

    MemoizedFunction(std::function<ReturnT(ArgsT...)> f) : m_original(f) {};

    ReturnT operator()(ArgsT... args) {
        std::tuple<ArgsT...> inputs(args...);
        auto pair = m_lookup_table.find(inputs);
        if (pair == m_lookup_table.end()) {
            was_last_call_memoized = false;
            std::cout << "Memoizing original function" << std::endl;
            ReturnT output = m_original(args...);
            m_lookup_table.insert({inputs, output});
            return output;
        }
        std::cout << "Original function already memoized" << std::endl;
        was_last_call_memoized = true;
        return pair->second;
    }
};


template <typename RetT, typename ...ArgsT>
class CurriedFunction {
    std::function<RetT(ArgsT...)> m_original;
    std::tuple<ArgsT...> m_inputs;

public:
    CurriedFunction(std::function<RetT(ArgsT...)> f, ArgsT... args) : m_original(f) {
        m_inputs = std::tuple<ArgsT...>(args...);
    };

    RetT operator()() {
        return std::apply(m_original, m_inputs);
    }
};




template <typename RetT, typename ...ArgsT>
struct CapturingFunction {

    struct Parameter {
        enum class Datatype {INT, DOUBLE, BOOL, STRING};
        Datatype datatype;
        std::any data;
    };

    std::vector<Parameter> m_parameters;
    std::function<RetT(ArgsT...)> m_original;

    void registerParam(int x) {
        Parameter p;
        p.data = x;
        p.datatype = Parameter::Datatype::INT;
        m_parameters.push_back(p);
    }

    void registerParam(double x) {
        Parameter p;
        p.data = x;
        p.datatype = Parameter::Datatype::DOUBLE;
        m_parameters.push_back(p);
    }

    void registerParam(std::string s) {
	Parameter p;
	p.data = s;
	p.datatype = Parameter::Datatype::STRING;
	m_parameters.push_back(p);
    }

    CapturingFunction(std::function<RetT(ArgsT...)> f) : m_original(f) {}

    RetT operator()(ArgsT&&... args) {
        // For each argument, register a parameter
        std::tuple<ArgsT...> t(args...);
        std::apply([&](auto ... x){ (registerParam(x), ...); }, t);
        RetT result = m_original(args...);
        registerParam(result);
        return result;
    }
};

template <typename ReturnT, typename... Args>
using Signature = ReturnT(Args...);

template<typename ReturnT, typename... Args>
CapturingFunction<ReturnT, Args...> make_capturing_function(Signature<ReturnT, Args...> f) {
    return CapturingFunction<ReturnT, Args...>(std::function<ReturnT(Args...)>(f));
}


namespace rcf {

struct Parameter {
    virtual bool load(size_t index) = 0; // Returns true iff index is in range
    virtual size_t store() = 0;          // Returns the index of the sample just stored
};

template <typename T>
struct ParameterT : public rcf::Parameter {
    std::vector<T> samples;
    T* destination;
    ParameterT(T* destination): destination(destination) {};

    bool load(size_t index) override {
        if (index >= samples.size()) return false;
        *destination = samples[index];
        return true;
    }
    size_t store() override {
        samples.push_back(*destination);
        return samples.size() - 1;
    }
};

template <typename T>
Parameter* make_parameter(T* t) {
    return new ParameterT<T>(t);
}


template <typename RetT, typename ...ArgsT>
struct ReferenceCapturingFunction {

    std::vector<Parameter*> m_parameters;
    std::function<RetT(ArgsT...)> m_original;
    std::tuple<ArgsT...> m_input_args;

    template <typename T>
    void registerParam(T* t) {
	m_parameters.push_back(new ParameterT<T>(t));
    }

    ReferenceCapturingFunction(std::function<RetT(ArgsT...)> f) : m_original(f) {
        // For each _input_ arg, we create a Parameter object that points to the correct slot in the m_inputs tuple
	std::apply([&](auto& ... x){ (registerParam(&x), ...); }, m_input_args);

	// std::apply([&]<typename... X>(X& ... x){ (m_parameters.push_back(new ParameterT<X>(&x)), ...); }, m_input_args);
	// Ideally we'd inline `registerParam`, however lambdas with template parameters require gcc >= 11
    }

    RetT operator()() {
        // for each _input_ parameter
        for (auto& p : m_parameters) {
	    // Copy data out of Parameter.value into Parameter.dest
	    p->load(0);
	}
        // The m_input_args should be filled now
	return std::apply(m_original, m_input_args);
    }
};

template<typename ReturnT, typename... Args>
ReferenceCapturingFunction<ReturnT, Args...> make_ref_capturing_function(Signature<ReturnT, Args...> f) {
    return ReferenceCapturingFunction<ReturnT, Args...>(std::function<ReturnT(Args...)>(f));
}

} // namespace rcf
} // namespace phasm::test::experiments

#endif //SURROGATE_TOOLKIT_EXPERIMENTS_H
