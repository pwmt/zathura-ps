/* SPDX-License-Identifier: Zlib */

#include <glib.h>
#include <stdlib.h>

#include <cairo.h>

#include "plugin.h"

zathura_error_t ps_page_render_cairo(zathura_page_t* page, void* data, cairo_t* cairo, bool GIRARA_UNUSED(printing)) {
  SpectrePage* ps_page = data;
  if (page == NULL || cairo == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  cairo_surface_t* surface = cairo_get_target(cairo);

  if (ps_page == NULL || surface == NULL || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS ||
      cairo_surface_get_type(surface) != CAIRO_SURFACE_TYPE_IMAGE) {
    return ZATHURA_ERROR_UNKNOWN;
  }

  int rowstride            = cairo_image_surface_get_stride(surface);
  unsigned char* image     = cairo_image_surface_get_data(surface);
  unsigned int page_width  = cairo_image_surface_get_width(surface);
  unsigned int page_height = cairo_image_surface_get_height(surface);

  SpectreRenderContext* context = spectre_render_context_new();

  if (context == NULL) {
    return ZATHURA_ERROR_UNKNOWN;
  }

  double scalex = ((double)page_width) / zathura_page_get_width(page);
  double scaley = ((double)page_height) / zathura_page_get_height(page);

  spectre_render_context_set_scale(context, scalex, scaley);

  unsigned char* page_data = NULL;
  int row_length;
  spectre_page_render(ps_page, context, &page_data, &row_length);
  spectre_render_context_free(context);

  if (page_data == NULL || spectre_page_status(ps_page) != SPECTRE_STATUS_SUCCESS) {
    if (page_data != NULL) {
      free(page_data);
    }

    return ZATHURA_ERROR_UNKNOWN;
  }

  for (unsigned int y = 0; y < page_height; y++) {
    memcpy(image + y * rowstride, page_data + y * row_length, 4 * page_width);
  }

  free(page_data);

  return ZATHURA_ERROR_OK;
}
