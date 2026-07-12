#include "core/printer.h"
#include "encoder/goo/goo_encoder.h"
#include "encoder/goo/layer_encoder.h"
#include "pipelines/pipe_data.h"
#include "encoder/exporter.h"
#include <vector>


namespace encoder {

    class GooExporter : public SliceExporter {
    private:
        encoder::GooEncoder internal_encoder;
    public:
        GooExporter(const std::string& config_path, const std::string& output_path)
            : internal_encoder(config_path, output_path) {}
    
        void write_layer(float z_offset, const std::vector<PixelSpan>& spans) override {
            encoder::LayerEncoder goo_layer_encoder;

            for (const auto& span : spans) {
                goo_layer_encoder.add_run(span.length, span.value);
            }

            internal_encoder.write_layer(z_offset, goo_layer_encoder);
        }

        Printer get_printer(double scale_1_to_mm) const override {
            return this->internal_encoder.get_printer(scale_1_to_mm);
        }

        void close() override {
            internal_encoder.close();
        }
    };
    
} //namespace encoder