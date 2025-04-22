#include <stdint.h>
void fmtrgb5652rgb888(const uint16_t *src, uint8_t *dst, int width, int height) {
   
        for (int i = 0; i < width * height; i++) {
        uint16_t pixel = src[i];
        dst[3 * i]     = (pixel >> 8) & 0xF8;  // Red
        dst[3 * i + 1] = (pixel >> 3) & 0xFC;  // Green
        dst[3 * i + 2] = (pixel << 3) & 0xF8;  // Blue
      }
   
}