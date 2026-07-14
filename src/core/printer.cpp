#include "core/printer.h"
#include <armadillo>
#include <cmath>

void Printer::update_matrices() {
    double tx = this->printable_area.width_mm / 2.0;
    double ty = this->printable_area.height_mm / 2.0;
    double tz = 0.0;

    
    this->M_world_to_printer.eye();

    M_world_to_printer(0, 0) = scale;
    M_world_to_printer(1, 1) = scale;
    M_world_to_printer(2, 2) = scale;

    M_world_to_printer(0, 3) = tx;
    M_world_to_printer(1, 3) = ty;

    M_printer_to_world.eye();
    double inv_scale = 1.0 / scale;

    M_printer_to_world(0, 0) = inv_scale;
    M_printer_to_world(1, 1) = inv_scale;
    M_printer_to_world(2, 2) = inv_scale;

    M_printer_to_world(0, 3) = -tx * inv_scale;
    M_printer_to_world(1, 3) = -ty * inv_scale;
    M_printer_to_world(2, 3) = -tz * inv_scale;
    
    M_printer_to_pixel.eye();
    M_printer_to_pixel(0, 0) = static_cast<double>(resolution.width) / printable_area.width_mm;
    M_printer_to_pixel(1, 1) = static_cast<double>(resolution.height) / printable_area.height_mm; // if is mirrored put -static_cast<...
}

std::pair<int, int> Printer::world_to_pixel(double x, double y) const {
    arma::vec4 world_pos = { x, y, 0.0, 1.0};
    arma::vec4 pixel_pos = M_printer_to_pixel * (M_world_to_printer * world_pos);
    return { static_cast<int>(std::round(pixel_pos(0))), static_cast<int>(std::round(pixel_pos(1)))};
}

double Printer::mm_to_world_z(double z_mm) const {
    arma::vec4 printer_pos = {0.0, 0.0, z_mm, 1.0};
    arma::vec4 world_pos = M_printer_to_world * printer_pos;
    return world_pos(2);
}

double Printer::world_to_mm(double w) const {
    return w * scale;
}