/* See LICENSE file for license and copyright information */

#include <glib.h>
#include <stdlib.h>

#include <cairo.h>

#include "plugin.h"

zathura_image_buffer_t*
ps_page_render(zathura_page_t* page, SpectrePage* spectre_page, zathura_error_t* error)
{
  if (page == NULL) {
    if (error != NULL) {
      *error = ZATHURA_ERROR_INVALID_ARGUMENTS;
    }
    goto error_ret;
  }

  zathura_document_t* document = zathura_page_get_document(page);
  if (document == NULL || spectre_page == NULL) {
    goto error_ret;
  }

  /* calculate sizes */
  double scale             = zathura_document_get_scale(document);
  unsigned int page_width  = scale * zathura_page_get_width(page);
  unsigned int page_height = scale * zathura_page_get_height(page);

  /* create image buffer */
  zathura_image_buffer_t* image_buffer = zathura_image_buffer_create(page_width, page_height);

  if (image_buffer == NULL) {
    if (error != NULL) {
      *error = ZATHURA_ERROR_OUT_OF_MEMORY;
    }
    goto error_ret;
  }

  SpectreRenderContext* context = spectre_render_context_new();

  if (context == NULL) {
    goto error_ret;
  }

  spectre_render_context_set_scale(context, scale, scale);
  spectre_render_context_set_rotation(context, 0);

  unsigned char* page_data = NULL;
  int row_length;
  spectre_page_render(spectre_page, context, &page_data, &row_length);
  spectre_render_context_free(context);

  if (page_data == NULL || spectre_page_status(spectre_page) != SPECTRE_STATUS_SUCCESS) {
    if (page_data != NULL) {
      free(page_data);
    }

    goto error_ret;
  }

  for (unsigned int y = 0; y < page_height; y++) {
    for (unsigned int x = 0; x < page_width; x++) {
      unsigned char *s = page_data + y * row_length + x * 4;
      guchar* p = image_buffer->data + y * image_buffer->rowstride + x * 3;
      p[0] = s[0];
      p[1] = s[1];
      p[2] = s[2];
    }
  }

  free(page_data);

  return image_buffer;

error_ret:

  if (error != NULL && *error == ZATHURA_ERROR_OK) {
    *error = ZATHURA_ERROR_UNKNOWN;
  }

  return NULL;
}

zathura_error_t
ps_page_render_cairo(zathura_page_t* page, SpectrePage* spectre_page, cairo_t* cairo, bool GIRARA_UNUSED(printing))
{
  if (page == NULL || cairo == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  SpectrePage* ps_page     = (SpectrePage*) zathura_page_get_data(page);;
  cairo_surface_t* surface = cairo_get_target(cairo);

  if (ps_page == NULL || surface == NULL ||
      cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS ||
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

  double scalex = ((double) page_width)  / zathura_page_get_width(page);
  double scaley = ((double) page_height) / zathura_page_get_height(page);

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
    for (unsigned int x = 0; x < page_width; x++) {
      unsigned char *s = page_data + y * row_length + x * 4;
      guchar* p = image + y * rowstride + x * 4;
      p[0] = s[0];
      p[1] = s[1];
      p[2] = s[2];
      p[3] = s[3];
    }
  }

  free(page_data);

  return ZATHURA_ERROR_OK;
}
