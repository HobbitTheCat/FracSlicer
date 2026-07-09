#ifndef PROJECT_FRAC_SLICER_RASTERIZATION_H
#define PROJECT_FRAC_SLICER_RASTERIZATION_H

#include "pipelines/pipe_data.h"
#include "core/printer.h"

namespace RasterizationService {

    struct PreviewSettings {
        int width = 1920;
        int height = 1080;
        bool enabled = true;
    };
    
    struct ScanlineEdge {
        int y_max;
        double x;
        double dx_dy;
    };

    /**
     * @brief Converts 2D continuous edges into compressed pixel rows.
     * 
     * This function processes context.flat_edges, translates them to pixel space
     * using the Printer settings, performs scanline filling, populates
     */
    void rasterize_layer(SliceContext& context, const Printer& printer, const PreviewSettings& preview);

} // namespace RasterizationService

#endif //PROJECT_FRAC_SLICER_RASTERIZATION_H