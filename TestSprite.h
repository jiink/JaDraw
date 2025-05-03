#ifndef TEST_SPRITE_H
#define TEST_SPRITE_H

#include "JaDraw.h" // Includes JaSprite definition

namespace Sprites {

    constexpr int test_sprite_width = 4;
    constexpr int test_sprite_height = 4;

    constexpr uint32_t test_sprite_palette_data[] = {
        JADRAW_RGBA(0xff, 0xff, 0xff, 0xff),
        JADRAW_RGBA(0xff, 0x00, 0x0c, 0xff),
        JADRAW_RGBA(0x00, 0x2e, 0xff, 0xff),
        JADRAW_RGBA(0x00, 0x00, 0x00, 0xff),
        JADRAW_RGBA(0x00, 0xff, 0x00, 0x00),  
    };
    constexpr uint8_t test_sprite_pixel_data[test_sprite_width * test_sprite_height] = {
        0x00, 0x00, 0x01, 0x01,
        0x02, 0x02, 0x02, 0x02,
        0x03, 0x00, 0x00, 0x00,
        0x03, 0x03, 0x04, 0x04 
    };
    constexpr JaSprite test_sprite(
        test_sprite_width,
        test_sprite_height,
        std::span{test_sprite_palette_data},
        std::span{test_sprite_pixel_data}
    );
}

#endif // TEST_SPRITE_H