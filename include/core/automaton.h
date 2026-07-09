#ifndef PROJECT_FRAC_SLICER_AUTOMATON_H
#define PROJECT_FRAC_SLICER_AUTOMATON_H

#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <cstddef>
#include <vector>

#include "geometry_types.h"

class JSONImporter;

class Automaton {
public:
    struct StateTransition {
        arma::mat transition_matrix;
        std::size_t targe_state_id;
    };

    struct State {
        std::vector<StateTransition> transitions;
        MeshTemplate reference_topology;
    };
    
private:
    friend class JSONImporter;

    std::vector<State> states;

public:
    Automaton() = default;

    /**
     * @brief Sets or overwrites the data for a specific automaton state.
     * 
     * @param state_id The index of the state to be configured.
     * @param transitions A list of valid transitions originating from this state.
     * @param reference_topology The baseline 3D mesh template used by this state.
     */
    void set_state(std::size_t state_id, std::vector<StateTransition> transitions, MeshTemplate reference_topology);

    /**
     * @brief Gets the upper bound for valid state IDs (total number of registered states).
     * @return The size of the internal states vector.
     */
    std::size_t get_max_state_id() const {return this->states.size();}

    /**
     * @brief Checks if the automaton contains any states.
     * @return True if the automaton is empty, false otherwise.
     */
    bool has_no_states() const {return this->states.empty();}

    /**
     * @brief Validates whether a specific state ID exists within the automaton bounds.
     * @param state_id The index of the state to check.
     * @return True if the state exists, false if it is out of bounds.
     */
    bool has_state(std::size_t state_id) const {return state_id < this->states.size();}

    /**
     * @brief Resizes the internal state storage to accommodate a given maximum ID.
     * @param max_id The highest state ID expected to be written into the automaton.
     */
    void resize_for_max_id(std::size_t max_id) {states.resize(max_id + 1);}


    /**
     * @brief Retrieves a constant reference to a specific state.
     * @param state_id The index of the requested state.
     * @return Constant reference to the State structure.
     * @throws std::out_of_range if the state_id is invalid.
     */
    const State& get_state(std::size_t state_id) const;

    /**
     * @brief Calculates the total number of transitions across all states in the automaton.
     * @return Total count of registered transitions.
     */
    std::size_t get_transition_number() const;

    /**
     * @brief Retrieves the reference 3D mesh template associated with a specific state.
     * @param state_id The index of the state whose topology is being requested.
     * @return Constant reference to the MeshTemplate.
     */
    const MeshTemplate& get_reference_topology(std::size_t state_id) const;

    /**
     * @brief Array subscript operator providing direct, unchecked access to a state.
     * @param state_id The index of the requested state.
     * @return Constant reference to the State structure.
     */
    const State& operator[](std::size_t state_id) const {return this->states[state_id];}

    /**
     * @brief Evaluates a single fractal mesh instance and generates its next-iteration children.
     * 
     * Applies all valid state transitions to the current instance's geometry.
     * Optionally discards generated children using a culling filter.
     * 
     * @param current_instance The parent mesh instance to evaluate.
     * @param out_instances Container where valid generated child instances will be appended.
     * @param filter Optional spatial filter (e.g., plane intersection) to cull unwanted instances.
     */
    void evaluate(
        const MeshInstance& current_instance, 
        std::vector<MeshInstance>& out_instances, 
        const CullingFilter& filter = nullptr
    ) const;
};

#endif //PROJECT_FRAC_SLICER_AUTOMATON_H