#ifndef PROJECT_FRAC_SLICER_FRACTAL_MODEL_H
#define PROJECT_FRAC_SLICER_FRACTAL_MODEL_H

#include "automaton.h"
#include "geometry_types.h"

#include <vector>

enum TestMeshesEnum{
    MENGER, SERPINSKI
};

class FractalModel {
private:
    Automaton automaton;
    std::vector<MeshInstance> initial_meshes;

public:
    FractalModel(Automaton automaton, std::vector<MeshInstance> initial_meshes)
        : automaton(automaton), initial_meshes(std::move(initial_meshes)) {}

    const Automaton& get_automaton() const {return this->automaton;}
    const std::vector<MeshInstance>& get_initial_meshes() const {return this->initial_meshes;}

    double get_height() const;
};

#endif //PROJECT_FRAC_SLICER_FRACTAL_MODEL_H