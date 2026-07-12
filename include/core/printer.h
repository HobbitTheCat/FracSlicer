#ifndef PROJECT_FRAC_SLICER_PRINTER_H
#define PROJECT_FRAC_SLICER_PRINTER_H

#include <utility>

class Printer {
public:
    struct Resolution {int width; int height;};
    struct PrintableArea {double width_mm; double height_mm;};

private:
    Resolution resolution;
    PrintableArea printable_area;
    double layer_height;
    double scale;
    
public:
    Printer(Resolution res, PrintableArea area, double layer_height_mm, double scale_1_to_mm) :
        resolution(res), printable_area(area), layer_height(layer_height_mm), scale(scale_1_to_mm){}

    Resolution get_resolution() const {return resolution;}
    double get_layer_height() const {return this->layer_height;}
    
    std::pair<int, int> world_to_pixel(double x, double y) const;
    double mm_to_world_z(double z_mm) const;

    double world_to_mm(double w) const {return this->scale * w;}
    PrintableArea get_printable_area() const {return this->printable_area;}
};

#endif //PROJECT_FRAC_SLICER_PRINTER_H