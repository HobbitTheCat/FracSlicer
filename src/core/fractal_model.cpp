#include "core/fractal_model.h"

#include <cstdlib>
#include <limits>

double FractalModel::get_height() const {
    double min_z = std::numeric_limits<double>::infinity();
    double max_z = -std::numeric_limits<double>::infinity();

    for (const auto& mesh : this->initial_meshes) {
        if (mesh.control_points.is_empty()) continue;

        const auto& z = mesh.control_points.row(2); // TODO maybe replace by column

        double local_min = z.min();
        double local_max = z.max();

        if (local_min < min_z) min_z = local_min;
        if (local_max > max_z) max_z = local_max;
    }

    return std::abs(max_z - min_z);
}