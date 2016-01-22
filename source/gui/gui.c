//
// Created by cpasjuste on 21/01/16.
//

#include <sf2d.h>
#include <sftd.h>
#include <stdarg.h>
#include <stdio.h>
#include "gui.h"
#include "Impact_ttf.h"

extern const struct {
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned int 	 bytes_per_pixel;
    unsigned char	 pixel_data[];
} bgTop;

sf2d_texture *bgTopTex;
sftd_font *font;

void guiInit() {

    sf2d_init();
    sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
    sf2d_set_3D(0);

    // background
    bgTopTex = sf2d_create_texture_mem_RGBA8(
            bgTop.pixel_data, bgTop.width, bgTop.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);

    // font
    sftd_init();
    font = sftd_load_font_mem(Impact_ttf, Impact_ttf_size);

}

void guiDrawBg() {
    sf2d_draw_texture(bgTopTex, 0, 0);
}

void guiDrawText(u32 color, int x, int y, int size, const char *format, ...) {

    char msg[1024];
    va_list argp;

    va_start(argp, format);
    vsnprintf(msg, 1024, format, argp);
    va_end(argp);

    sftd_draw_text(font, x, y, color, size, msg);
}

void guiDrawTextCenter(gfxScreen_t screen, u32 color, int y, int size, const char *format, ...) {

    char msg[1024];
    va_list argp;

    va_start(argp, format);
    vsnprintf(msg, 1024, format, argp);
    va_end(argp);

    int w = sftd_get_text_width(font, size, msg);
    int center = screen == GFX_TOP ? 200 : 160;
    sftd_draw_text(font, center-w/2, y, color, size, msg);
}

void guiDrawRect(u32 color, int x, int y, int w, int h) {
    sf2d_draw_rectangle(x, y, w, h, color);
}

void guiStart(gfxScreen_t screen) {
    sf2d_start_frame(screen, GFX_LEFT);
}

void guiEnd() {
    sf2d_end_frame();
}

void guiSwap() {
    sf2d_swapbuffers();
}

void guiExit() {
    sftd_free_font(font);
    sftd_fini();
    sf2d_free_texture(bgTopTex);
    sf2d_fini();
}
