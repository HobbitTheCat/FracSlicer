#ifndef PROJECT_FRAC_SLICER_FILTERS_H
#define PROJECT_FRAC_SLICER_FILTERS_H

#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <cmath>

#include "../core/geometry_types.h"

// ART can pre-calculate the sphere's dimensions for each primitive and multiply them by the scale and transformation matrix for a given iteration
// In theory, the algorithm will be O(1) instead of O(n)


class FractalFilters {
public:
    using CullingFilter = std::function<bool(const MeshInstance&)>;

    static void compute_bounding_sphere(const MeshInstance& inst, arma::rowvec& out_center, double& out_radius) {
        out_center = arma::mean(inst.control_points, 0);
        arma::mat diff = inst.control_points.each_row() - out_center;
        out_radius = std::sqrt(arma::max(arma::sum(arma::square(diff), 1)));
    }

    static CullingFilter plane_intersection(arma::rowvec normal, double d, int mode = 0) {
        const arma::rowvec n = arma::normalise(normal);

        switch (mode) {
            case 1:
                return [n, d](const MeshInstance& inst) -> bool {
                    arma::rowvec center; double radius;
                    compute_bounding_sphere(inst, center, radius);
                    return (arma::dot(n, center) + d) >= -radius;
                };
            case 2:
                return [n, d](const MeshInstance& inst) -> bool {
                    arma::rowvec center; double radius;
                    compute_bounding_sphere(inst, center, radius);
                    return (arma::dot(n, center) + d) <= radius;
                };
            case 0:
            default:
                return [n, d](const MeshInstance& inst) -> bool {
                    arma::rowvec center; double radius;
                    compute_bounding_sphere(inst, center, radius);
                    return std::abs(arma::dot(n, center) - d) <= radius;
                };
        }
    }

    static CullingFilter sphere_void(const arma::rowvec& void_center, double void_radius) {
        return [void_center, void_radius](const MeshInstance& inst) -> bool {
            arma::rowvec center;
            double radius;
            compute_bounding_sphere(inst, center, radius);

            double dist_between_centers = arma::norm(center - void_center);

            return !(dist_between_centers + radius < void_radius);
        };
    }

    static CullingFilter combine_and(CullingFilter& f1, CullingFilter& f2) {
        return [f1, f2](const MeshInstance& inst) -> bool {
            return f1(inst) && f2(inst);
        };
    }
};


#endif //PROJECT_FRAC_SLICER_FILTERS_H