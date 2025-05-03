#ifndef JADRAW_H
#define JADRAW_H

#include <array>
#include <cstdint>
#include <cstddef>

template <int W, int H>
class JaDraw {
    static_assert(W > 0, "Need positive width");
    static_assert(H > 0, "Need positive height");
    static_assert(static_cast<std::size_t>(W) * H > 0, "Too big");
public:
    static constexpr int width = W;
    static constexpr int height = H;
    std::array<uint32_t, static_cast<std::size_t>(W) * H> canvas;
    JaDraw() : canvas{} { }
    void drawPixel(int x, int y, uint32_t color);
};

///////////////////////////////////////////////////////////////////////////////

template <int W, int H>
void JaDraw<W, H>::drawPixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < W && y >= 0 && y < H) {
        canvas[static_cast<std::size_t>(y) * W + x] = color;
    }
}

#endif // JADRAW_H