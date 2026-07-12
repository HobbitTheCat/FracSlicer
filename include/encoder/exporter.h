#pragma once

#include "core/printer.h"
#include "pipelines/pipe_data.h"
#include <vector>

namespace encoder {

class SliceExporter {
public:
    virtual ~SliceExporter() = default;

    virtual void write_layer(float z_offset, const std::vector<PixelSpan>& spans) = 0;

    virtual Printer get_printer(double scale_1_to_mm) const = 0;

    virtual void close() = 0;
};

} //namespace encoder