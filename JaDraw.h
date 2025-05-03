#ifndef JADRAW_H
#define JADRAW_H

#include <array>
#include <cstdint>
#include <cstddef>
#include <cmath>      // For std::abs
#include <algorithm>  // For std::min, std::max
#include <cassert>    // Optional: for internal assertions

// --- Color Component Macros/Functions (place inside or outside class) ---
// Using macros for direct translation, inline functions are often preferred in C++
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
    // Basic overflow check, might need more robust check for very large W*H
    static_assert(static_cast<unsigned long long>(W) * H <= SIZE_MAX / sizeof(uint32_t), "Canvas size exceeds limits");

private:
    // Helper: Blend a color onto a specific pixel coordinates
    // Assumes x, y are already in bounds!
    inline void blendPixel(int x, int y, uint32_t source_color) {
        size_t index = static_cast<std::size_t>(y) * W + x;
        uint32_t* dest_pixel_ptr = &canvas[index]; // Get pointer to the destination pixel

        // --- Adapted olivec_blend_color logic ---
        uint32_t r1 = JADRAW_RED(*dest_pixel_ptr);
        uint32_t g1 = JADRAW_GREEN(*dest_pixel_ptr);
        uint32_t b1 = JADRAW_BLUE(*dest_pixel_ptr);
        uint32_t a1 = JADRAW_ALPHA(*dest_pixel_ptr); // Background alpha

        uint32_t r2 = JADRAW_RED(source_color);
        uint32_t g2 = JADRAW_GREEN(source_color);
        uint32_t b2 = JADRAW_BLUE(source_color);
        uint32_t a2 = JADRAW_ALPHA(source_color); // Foreground alpha

        // If source is fully transparent, do nothing
        if (a2 == 0) return;
        // If source is fully opaque, just overwrite (optimization)
        if (a2 == 255) {
             *dest_pixel_ptr = source_color;
             // Optional: Preserve destination alpha if needed by your blending rules
             // uint32_t current_alpha = JADRAW_ALPHA(*dest_pixel_ptr);
             // *dest_pixel_ptr = JADRAW_RGBA(JADRAW_RED(source_color), JADRAW_GREEN(source_color), JADRAW_BLUE(source_color), current_alpha);
             return;
        }


        // Standard alpha blending: R = (Rsrc * Asrc + Rbg * (255 - Asrc)) / 255
        // Use unsigned int for calculations to potentially avoid intermediate overflow issues slightly better
        // although with 8-bit channels it's unlikely.
        unsigned int blend_r = (r2 * a2 + r1 * (255 - a2)) / 255;
        unsigned int blend_g = (g2 * a2 + g1 * (255 - a2)) / 255;
        unsigned int blend_b = (b2 * a2 + b1 * (255 - a2)) / 255;

        // Note: OliveC kept the original background alpha (a1).
        // Alternative: Calculate blended alpha: a = a2 + a1 * (255 - a2) / 255;
        // We will stick to olivec's original behaviour of preserving background alpha.

        // Clamp results just in case (though division by 255 should keep them <= 255)
        // Not strictly necessary if types are unsigned and calculation is correct.
        // blend_r = std::min(blend_r, 255u);
        // blend_g = std::min(blend_g, 255u);
        // blend_b = std::min(blend_b, 255u);

        *dest_pixel_ptr = JADRAW_RGBA(blend_r, blend_g, blend_b, a1);
        // --- End adapted olivec_blend_color logic ---
    }

    // Helper: Draw pixel without blending (overwrite)
    // Assumes x, y are already in bounds!
    inline void drawPixelUnsafe(int x, int y, uint32_t color) {
         canvas[static_cast<std::size_t>(y) * W + x] = color;
    }


public:
    static constexpr int width = W;
    static constexpr int height = H;
    // Keep canvas public for easy access from SDL example, or make private later
    std::array<uint32_t, static_cast<std::size_t>(W) * H> canvas;

    JaDraw() : canvas{} { } // Zero initialize

    // Original drawPixel (overwrites)
    inline void drawPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < W && y >= 0 && y < H) {
            drawPixelUnsafe(x, y, color);
        }
    }

    // New drawPixel with alpha blending
    inline void drawPixelBlend(int x, int y, uint32_t color) {
        // Check bounds first
        if (x >= 0 && x < W && y >= 0 && y < H) {
            blendPixel(x, y, color);
        }
    }

    // Add a clearCanvas function to reset the canvas to a specific color
    void clear(uint32_t color) {
        std::fill(canvas.begin(), canvas.end(), color);
    }

    // --- Adapted olivec_line ---
    // Basic Bresenham line algorithm (integer only) with thickness and blending
    void drawLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color)
    {
        uint32_t alpha = JADRAW_ALPHA(color);
        if (thickness <= 0 || alpha == 0) return; // Nothing to draw

        bool is_opaque = (alpha == 255);

        // Inline helper for drawing/blending a pixel safely
        auto plot = [&](int x, int y) {
            if (x >= 0 && x < W && y >= 0 && y < H) {
                if (is_opaque) {
                    drawPixelUnsafe(x, y, color);
                } else {
                    blendPixel(x, y, color);
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
                plot(x1, y1); // Use the helper lambda to plot/blend
                if (x1 == x2 && y1 == y2) break;
                e2_thin = 2 * err_thin;
                if (e2_thin >= dy_thin) {
                    if (x1 == x2) break; // Should prevent infinite loop on vertical lines
                    err_thin += dy_thin;
                    x1 += sx_thin;
                }
                if (e2_thin <= dx_thin) {
                    if (y1 == y2) break; // Should prevent infinite loop on horizontal lines
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

        // Calculate offsets for thickness
        int half_thick_floor = (thickness - 1) / 2; // integer division
        int half_thick_ceil = thickness / 2;      // integer division equivalent to ceil(t/2.0) for t>=1

        // --- Special Cases for Horizontal and Vertical Lines (Efficient) ---
        if (abs_dx == 0) { // Vertical Line
            int start_x = x1 - half_thick_floor;
            int end_x = x1 + half_thick_ceil; // Exclusive end for loop below
            int start_y = std::min(y1, y2);
            int end_y = std::max(y1, y2);
            for (int y = start_y; y <= end_y; ++y) {
                for (int x = start_x; x < end_x; ++x) { // Use < end_x for width = thickness
                    plot(x, y);
                }
            }
            return;
        }
        if (abs_dy == 0) { // Horizontal Line
            int start_y = y1 - half_thick_floor;
            int end_y = y1 + half_thick_ceil; // Exclusive end for loop below
            int start_x = std::min(x1, x2);
            int end_x = std::max(x1, x2);
            for (int x = start_x; x <= end_x; ++x) {
                for (int y = start_y; y < end_y; ++y) { // Use < end_y for height = thickness
                    plot(x, y);
                }
            }
            return;
        }

        // --- General Case (Diagonal Lines) ---
        // Modified Bresenham drawing perpendicular spans at each step.
        if (abs_dx > abs_dy) { // X-dominant line
            int err = 2 * abs_dy - abs_dx;
            int y = y1;
            for (int x = x1; x != x2 + sx; x += sx) {
                // Draw vertical span centered at (x, y)
                int span_start_y = y - half_thick_floor;
                int span_end_y = y + half_thick_ceil; // Exclusive end
                for (int py = span_start_y; py < span_end_y; ++py) {
                    plot(x, py);
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
                // Draw horizontal span centered at (x, y)
                int span_start_x = x - half_thick_floor;
                int span_end_x = x + half_thick_ceil; // Exclusive end
                for (int px = span_start_x; px < span_end_x; ++px) {
                    plot(px, y);
                }

                if (err >= 0) {
                    x += sx;
                    err -= 2 * abs_dy;
                }
                err += 2 * abs_dx;
            }
        }
    } // --- End adapted olivec_line ---

};

#endif // JADRAW_H