#ifndef JADRAW_H
#define JADRAW_H

#include <array>
#include <cstdint>
#include <cstddef>
#include <cmath>     
#include <algorithm> 
#include <cassert>   

// --- Color Component Macros/Functions ---
#define JADRAW_RED(color)   (((color) >> 16) & 0xFFU)
#define JADRAW_GREEN(color) (((color) >> 8)  & 0xFFU)
#define JADRAW_BLUE(color)  ((color) & 0xFFU)
#define JADRAW_ALPHA(color) (((color) >> 24) & 0xFFU)
#define JADRAW_RGBA(r,g,b,a) ( (static_cast<uint32_t>(a) << 24) | \
                             (static_cast<uint32_t>(r) << 16) | \
                             (static_cast<uint32_t>(g) << 8)  | \
                             (static_cast<uint32_t>(b)) )

template <int W, int H>
class JaDraw {
    static_assert(W > 0, "Need positive width");
    static_assert(H > 0, "Need positive height");
    static_assert(static_cast<unsigned long long>(W) * H <= SIZE_MAX / sizeof(uint32_t), "Canvas size exceeds limits");

private:
    // --- Wu's Algorithm Helpers ---
    // Integer part of x
    static inline int ipart(float x) { return static_cast<int>(std::floor(x)); }
    // Fractional part of x
    static inline float fpart(float x) { return x - std::floor(x); }
    // 1.0 - fractional part of x
    static inline float rfpart(float x) { return 1.0f - fpart(x); }
     // Round float to nearest int
    static inline int round_int(float x) { return static_cast<int>(std::round(x)); }
    // --- End Wu's Helpers ---


    // Helper: Blend a color onto specific coordinates with a given intensity (alpha multiplier)
    // Assumes x, y are already in bounds!
    // intensity is typically between 0.0 (transparent) and 1.0 (opaque)
    inline void blendPixelWithIntensity(int x, int y, uint32_t source_color, float intensity) {
        // Clamp intensity just in case
        if (intensity <= 0.0f) return;
        if (intensity > 1.0f) intensity = 1.0f;

        size_t index = static_cast<std::size_t>(y) * W + x;
        uint32_t* dest_pixel_ptr = &canvas[index];

        uint32_t src_r = JADRAW_RED(source_color);
        uint32_t src_g = JADRAW_GREEN(source_color);
        uint32_t src_b = JADRAW_BLUE(source_color);
        uint32_t src_a_base = JADRAW_ALPHA(source_color); // Original alpha of the source color

        // Modulate the source alpha by the calculated line intensity
        uint32_t src_a_effective = static_cast<uint32_t>(src_a_base * intensity);

        // If effectively transparent, do nothing
        if (src_a_effective == 0) return;

        // If effectively opaque (considering original alpha and intensity)
        // and destination is also fully opaque (or we don't care about its alpha),
        // we could optimize by just setting the color directly.
        // For general blending:
        if (src_a_effective == 255) {
             uint32_t dest_a = JADRAW_ALPHA(*dest_pixel_ptr); // Preserve destination alpha? Or use 255?
             // Simple overwrite preserving RGB of source, keep dest alpha
             *dest_pixel_ptr = JADRAW_RGBA(src_r, src_g, src_b, dest_a);
             return;
        }

        uint32_t dest_r = JADRAW_RED(*dest_pixel_ptr);
        uint32_t dest_g = JADRAW_GREEN(*dest_pixel_ptr);
        uint32_t dest_b = JADRAW_BLUE(*dest_pixel_ptr);
        uint32_t dest_a = JADRAW_ALPHA(*dest_pixel_ptr); // Background alpha

        // Alpha blend using the *effective* source alpha
        // R = (Rsrc * Asrc_eff + Rbg * (255 - Asrc_eff)) / 255
        unsigned int blend_r = (src_r * src_a_effective + dest_r * (255 - src_a_effective)) / 255;
        unsigned int blend_g = (src_g * src_a_effective + dest_g * (255 - src_a_effective)) / 255;
        unsigned int blend_b = (src_b * src_a_effective + dest_b * (255 - src_a_effective)) / 255;

        // Calculate blended alpha (common method): a = a_src_eff + a_dest * (255 - a_src_eff) / 255
        unsigned int blend_a = src_a_effective + (dest_a * (255 - src_a_effective)) / 255;

        *dest_pixel_ptr = JADRAW_RGBA(blend_r, blend_g, blend_b, blend_a);
    }


    // Helper: Draw pixel without blending (overwrite)
    // Assumes x, y are already in bounds!
    inline void drawPixelUnsafe(int x, int y, uint32_t color) {
         canvas[static_cast<std::size_t>(y) * W + x] = color;
    }

public:
    static constexpr int width = W;
    static constexpr int height = H;
    std::array<uint32_t, static_cast<std::size_t>(W) * H> canvas;

    JaDraw() : canvas{} { } // Zero initialize

    // Original drawPixel (overwrites)
    inline void drawPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < W && y >= 0 && y < H) {
            drawPixelUnsafe(x, y, color);
        }
    }

    // Draw pixel with standard alpha blending (using its own alpha)
    inline void drawPixelBlend(int x, int y, uint32_t color) {
        if (x >= 0 && x < W && y >= 0 && y < H) {
            // Call the intensity blend with full intensity (1.0)
            // This reuses the logic but might be slightly less optimal than a dedicated blend function
            blendPixelWithIntensity(x, y, color, 1.0f);
        }
    }

    void clear(uint32_t color) {
        std::fill(canvas.begin(), canvas.end(), color);
    }

    // --- Integer Bresenham Line ---
    void drawLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color)
    {
        uint32_t alpha = JADRAW_ALPHA(color);
        if (thickness <= 0 || alpha == 0) return; // Nothing to draw

        bool is_opaque = (alpha == 255);

        // Inline helper for drawing/blending a pixel safely
        auto plot_int = [&](int x, int y) { // Renamed lambda to avoid conflict
            if (x >= 0 && x < W && y >= 0 && y < H) {
                if (is_opaque) {
                    drawPixelUnsafe(x, y, color);
                } else {
                    // Using blendPixelWithIntensity with 1.0f for consistency
                    // or call a simpler blendPixel if you defined one separately
                    blendPixelWithIntensity(x, y, color, 1.0f);
                }
            }
        };

        if (thickness == 1) {
            // --- Adapted thin line logic ---
            int dx_thin = std::abs(x2 - x1);
            int dy_thin = -std::abs(y2 - y1);
            int sx_thin = x1 < x2 ? 1 : -1;
            int sy_thin = y1 < y2 ? 1 : -1;
            int err_thin = dx_thin + dy_thin;
            int e2_thin;

            while (true) {
                plot_int(x1, y1); // Use the helper lambda to plot/blend
                if (x1 == x2 && y1 == y2) break;
                e2_thin = 2 * err_thin;
                if (e2_thin >= dy_thin) {
                    if (x1 == x2) break;
                    err_thin += dy_thin;
                    x1 += sx_thin;
                }
                if (e2_thin <= dx_thin) {
                    if (y1 == y2) break;
                    err_thin += dx_thin;
                    y1 += sy_thin;
                }
            }
            return; // Finished drawing thin line
        }

        // --- Thick Line Logic ---
        int dx = x2 - x1;
        int dy = y2 - y1;
        int abs_dx = std::abs(dx);
        int abs_dy = std::abs(dy);
        int sx = (dx > 0) ? 1 : -1;
        int sy = (dy > 0) ? 1 : -1;

        int half_thick_floor = (thickness - 1) / 2;
        int half_thick_ceil = thickness / 2;

        if (abs_dx == 0) { // Vertical Line
            int start_x = x1 - half_thick_floor;
            int end_x = x1 + half_thick_ceil;
            int start_y = std::min(y1, y2);
            int end_y = std::max(y1, y2);
            for (int y = start_y; y <= end_y; ++y) {
                for (int x = start_x; x < end_x; ++x) {
                    plot_int(x, y);
                }
            }
            return;
        }
        if (abs_dy == 0) { // Horizontal Line
            int start_y = y1 - half_thick_floor;
            int end_y = y1 + half_thick_ceil;
            int start_x = std::min(x1, x2);
            int end_x = std::max(x1, x2);
            for (int x = start_x; x <= end_x; ++x) {
                for (int y = start_y; y < end_y; ++y) {
                    plot_int(x, y);
                }
            }
            return;
        }

        if (abs_dx > abs_dy) { // X-dominant line
            int err = 2 * abs_dy - abs_dx;
            int y = y1;
            for (int x = x1; x != x2 + sx; x += sx) {
                int span_start_y = y - half_thick_floor;
                int span_end_y = y + half_thick_ceil;
                for (int py = span_start_y; py < span_end_y; ++py) {
                    plot_int(x, py);
                }
                if (err >= 0) {
                    y += sy;
                    err -= 2 * abs_dx;
                }
                err += 2 * abs_dy;
            }
        } else { // Y-dominant line
            int err = 2 * abs_dx - abs_dy;
            int x = x1;
            for (int y = y1; y != y2 + sy; y += sy) {
                int span_start_x = x - half_thick_floor;
                int span_end_x = x + half_thick_ceil;
                for (int px = span_start_x; px < span_end_x; ++px) {
                    plot_int(px, y);
                }
                if (err >= 0) {
                    x += sx;
                    err -= 2 * abs_dy;
                }
                err += 2 * abs_dx;
            }
        }
    }

    // --- Xiaolin Wu's Anti-Aliased Line ---
    void drawLineAA(float x1, float y1, float x2, float y2, uint32_t color) {
        // If base color is fully transparent, nothing to do
        if (JADRAW_ALPHA(color) == 0) {
            return;
        }

        // Helper lambda for plotting pixels with bounds check and intensity blending
        auto plot = [&](int x, int y, float intensity) {
            if (intensity > 0.0f && x >= 0 && x < W && y >= 0 && y < H) {
                 blendPixelWithIntensity(x, y, color, intensity);
            }
        };

        float dx = x2 - x1;
        float dy = y2 - y1;

        // Handle trivial case: points are the same (or very close)
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) {
             plot(round_int(x1), round_int(y1), 1.0f);
             return;
        }

        // --- Determine major axis ---
        if (std::abs(dx) > std::abs(dy)) { // X-major or horizontal
            // Ensure x1 <= x2
            if (x1 > x2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
                // Recalculate dx/dy after swap is implied by swapped points
            }
            dx = x2 - x1; // Recalc dx after potential swap
            dy = y2 - y1; // Recalc dy after potential swap
            float gradient = dy / dx;
            if (dx == 0.0f) gradient = 1.0f; // Avoid division by zero for vertical part of horizontal lines? Should be handled by dx > dy check but safety first.

            // --- Handle first endpoint ---
            int x_end1 = round_int(x1);
            float y_end1 = y1 + gradient * (x_end1 - x1);
            float gap1 = rfpart(x1 + 0.5f); // Gap calculation for endpoint intensity
            int ix1 = x_end1;
            int iy1 = ipart(y_end1);
            plot(ix1, iy1, rfpart(y_end1) * gap1);
            plot(ix1, iy1 + 1, fpart(y_end1) * gap1);
            float inter_y = y_end1 + gradient; // First intersection point

            // --- Handle second endpoint ---
            int x_end2 = round_int(x2);
            float y_end2 = y2 + gradient * (x_end2 - x2);
            float gap2 = fpart(x2 + 0.5f); // Gap calculation for endpoint intensity
            int ix2 = x_end2;
            int iy2 = ipart(y_end2);
            plot(ix2, iy2, rfpart(y_end2) * gap2);
            plot(ix2, iy2 + 1, fpart(y_end2) * gap2);

            // --- Main loop along X-axis ---
            for (int x = ix1 + 1; x < ix2; ++x) {
                plot(x, ipart(inter_y), rfpart(inter_y)); // Intensity based on distance from grid line
                plot(x, ipart(inter_y) + 1, fpart(inter_y));
                inter_y += gradient;
            }

        } else { // Y-major or vertical
             // Ensure y1 <= y2
            if (y1 > y2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
                 // Recalculate dx/dy after swap is implied by swapped points
           }
            dx = x2 - x1; // Recalc dx after potential swap
            dy = y2 - y1; // Recalc dy after potential swap
            float gradient = dx / dy;
            if (dy == 0.0f) gradient = 1.0f; // Avoid division by zero

            // --- Handle first endpoint ---
            int y_end1 = round_int(y1);
            float x_end1 = x1 + gradient * (y_end1 - y1);
            float gap1 = rfpart(y1 + 0.5f);
            int iy1 = y_end1;
            int ix1 = ipart(x_end1);
            plot(ix1, iy1, rfpart(x_end1) * gap1);
            plot(ix1 + 1, iy1, fpart(x_end1) * gap1);
            float inter_x = x_end1 + gradient;

            // --- Handle second endpoint ---
            int y_end2 = round_int(y2);
            float x_end2 = x2 + gradient * (y_end2 - y2);
            float gap2 = fpart(y2 + 0.5f);
            int iy2 = y_end2;
            int ix2 = ipart(x_end2);
            plot(ix2, iy2, rfpart(x_end2) * gap2);
            plot(ix2 + 1, iy2, fpart(x_end2) * gap2);

            // --- Main loop along Y-axis ---
            for (int y = iy1 + 1; y < iy2; ++y) {
                plot(ipart(inter_x), y, rfpart(inter_x));
                plot(ipart(inter_x) + 1, y, fpart(inter_x));
                inter_x += gradient;
            }
        }
    } // --- End drawLineAA ---


}; // End class JaDraw


#endif // JADRAW_H