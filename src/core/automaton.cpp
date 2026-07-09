#include "core/automaton.h"

void Automaton::set_state(std::size_t state_id, std::vector<StateTransition> transitions, MeshTemplate reference_topology) {
    if (state_id >= this->states.size()) 
        throw std::out_of_range(
            "Automaton error: state_id " + std::to_string(state_id) +
            " exceeds allocated size " + std::to_string(states.size())
        );
    states[state_id] = State{std::move(transitions), std::move(reference_topology)};
}

const Automaton::State& Automaton::get_state(std::size_t state_id) const {
    if (!this->has_state(state_id))throw std::invalid_argument("State with ID " + std::to_string(state_id) + " does not exist");
    return this->states[state_id];
}

std::size_t Automaton::get_transition_number() const {
    std::size_t transition_count = 0;
    for (const auto& state : this->states) {
        transition_count += state.transitions.size();
    }
    return transition_count;
}

const MeshTemplate& Automaton::get_reference_topology(std::size_t state_id) const {
    return this->get_state(state_id).reference_topology;
}

void Automaton::evaluate(const MeshInstance& current_instance, std::vector<MeshInstance>& out_instances, const CullingFilter& filter) const {
    const std::size_t state_id = current_instance.automaton_state;
    const State& current_state = this->get_state(state_id);

    for (const auto& trans : current_state.transitions) {
        MeshInstance new_instance;
        new_instance.automaton_state = trans.targe_state_id;
        new_instance.control_points = trans.transition_matrix * current_instance.control_points;

        if (filter && !filter(new_instance)) continue;
        out_instances.push_back(std::move(new_instance));
    }
}