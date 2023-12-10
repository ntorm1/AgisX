#include "gmath.h"


namespace gmath
{
    sRGBColor hexCodeToSRGB(std::string_view c)
    {
        uint8_t r = 0, g = 0, b = 0, a = 255;
        auto    hexToDec = [](char h) -> uint8_t {
            if (h >= '0' && h <= '9')
                return h - '0';
            else if (h >= 'a' && h <= 'f')
                return h - 'a' + 10;
            else if (h >= 'A' && h <= 'F')
                return h - 'A' + 10;
            else
                return 0;
            };
        if (c.length() < 4 || c[0] != '#') {
            r = g = b = 255; // on error return white
        }
        else {
            switch (c.length()) {
            case 5: // #rgba format
                a = hexToDec(c[4]);
                a += a * 16;
                [[fallthrough]];
            case 4: // #rgb format
                r = hexToDec(c[1]);
                g = hexToDec(c[2]);
                b = hexToDec(c[3]);
                r += r * 16;
                g += g * 16;
                b += b * 16;
                break;
            case 9: // #rrggbbaa format
                a = hexToDec(c[7]) * 16 + hexToDec(c[8]);
                [[fallthrough]];
            case 7: // #rrggbb format or #rrggbbaa format
                r = hexToDec(c[1]) * 16 + hexToDec(c[2]);
                g = hexToDec(c[3]) * 16 + hexToDec(c[4]);
                b = hexToDec(c[5]) * 16 + hexToDec(c[6]);
                break;
            default: r = g = b = a = 255;
            }
        }
        return { r, g, b, a };
    }
}