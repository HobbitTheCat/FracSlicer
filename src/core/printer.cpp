#include "core/printer.h"

std::pair<int, int> Printer::world_to_pixel(double x, double y) const {
    double x_mm = x * this->scale + (this->printable_area.width_mm / 2.0);
    // TODO изображение может получиться зеркальным
    double y_mm = y * this->scale + (this->printable_area.height_mm / 2.0);

    int px = static_cast<int>((x_mm / this->printable_area.width_mm) * this->resolution.width);
    int py = static_cast<int>((y_mm / this->printable_area.height_mm) * this->resolution.height);

    return {px, py};
}

double Printer::mm_to_world_z(double z_mm) const {return z_mm / scale;}