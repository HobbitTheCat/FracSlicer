#ifndef PROJECT_FRAC_JSON_IMPORT_H
#define PROJECT_FRAC_JSON_IMPORT_H

#include "nlohmann_json/json.hpp"

#include "core/fractal_model.h"
#include <string>

namespace json_import {

    FractalModel import_json(const std::string& file_name);

    MeshTemplate parse_primitive_state(const nlohmann::json& state_json);

    bool validate_mesh(const MeshTemplate& mesh, const std::string& primitive_name = "Unknown");
    
} // namespace json_import

#endif //PROJECT_FRAC_JSON_IMPORT_H
