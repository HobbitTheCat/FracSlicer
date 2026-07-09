#include "services/rasterization_service.h"

#include <algorithm>
#include <list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace RasterizationService {
    
    void rasterize_layer(SliceContext &context, const Printer &printer, const PreviewSettings& preview) {
        if (context.flat_edges.empty()) return;
        
        auto resulution = printer.get_resolution();
        const int width = resulution.width;
        const int height = resulution.height;

        if (preview.enabled) {
            context.preview_buffer.assign(preview.width * preview.height, 0x00);
        }
        const double scale_x = static_cast<double>(preview.width) / width;
        const double scale_y = static_cast<double>(preview.height) / height;
        
        std::vector<std::vector<ScanlineEdge>> ET(height);

        for (const auto& edge : context.flat_edges) {
            auto [x0, y0] = printer.world_to_pixel(edge.p0[0], edge.p0[1]); // TODO возможно оптимальнее переписать world_to_pixel на структуру edge
            auto [x1, y1] = printer.world_to_pixel(edge.p1[0], edge.p1[1]);

            if (y0 == y1) continue;

            if (y0 > y1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            if (y1 <= 0 || y0 >= height) continue;

            double x_start = x0;
            double dx_dy = static_cast<double>(x1-x0) / (y1 - y0);

            if (y0 < 0) {
                x_start += dx_dy * (0 - y0);
                y0 = 0;
            }

            if (y1 > height) y1 = height;
            ET[y0].push_back({y1, x_start, dx_dy});
        }

        context.flat_edges.clear();
        std::list<ScanlineEdge> AET;
        int total_decompressed_pixels = 0;
        
        for (int y = 0; y < height; y++) {
            for (const auto& edge : ET[y]) AET.push_back(edge);
            AET.remove_if([y](const ScanlineEdge& e){ return e.y_max <= y;});

            std::vector<int> intersection;
            intersection.reserve(AET.size());
            for (auto& edge : AET) {
                intersection.push_back(static_cast<int>(std::round(edge.x)));
                edge.x += edge.dx_dy;
            }

            std::sort(intersection.begin(), intersection.end());
            int current_x = 0;

            int preview_y = -1;
            if (preview.enabled) {
                preview_y = static_cast<int>(y * scale_y);
                if (preview_y >= preview.height) preview_y = preview.height - 1;
            }

            for (size_t i = 0; i + 1 < intersection.size(); i+= 2) {
                int start_x = std::max(current_x, intersection[i]);
                int end_x = std::min(width, intersection[i + 1]);

                if (start_x >= width || end_x <= 0 || start_x >= end_x) continue; // TODO Maybe add throw error

                if (start_x > current_x) {
                    context.layer_encoder.add_run(start_x - current_x, 0x00);
                    total_decompressed_pixels += (start_x - current_x);
                }
                context.layer_encoder.add_run(end_x - start_x, 0xFF);
                total_decompressed_pixels += (end_x - start_x);

                if (preview.enabled && preview_y >= 0) {
                    int p_start_x = std::round(start_x * scale_x);
                    int p_end_x = std::round(end_x * scale_x);

                    p_end_x = std::min(p_end_x, preview.width);

                    int row_offset = preview_y * preview.width;
                    for (int px = p_start_x; px < p_end_x; px++) {
                        context.preview_buffer[row_offset + px] = 0xFF;
                    }
                }
                
                current_x = end_x;
            }

            if (current_x < width) {
                context.layer_encoder.add_run(width - current_x, 0x00);
                total_decompressed_pixels += (width - current_x);
            }
        }

        int expected_pixels = width * height;
        if (total_decompressed_pixels != expected_pixels) { // relocate this throw into .goo export class member function
            throw std::runtime_error("Rasterization error: Pixel number after decomposition: " + 
                  std::to_string(total_decompressed_pixels) +
                  ", expected: " + std::to_string(expected_pixels) + ".");
        }
    }
    
} //namespace RasterizationService