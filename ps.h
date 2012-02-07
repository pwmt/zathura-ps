/* See LICENSE file for license and copyright information */

#ifndef PS_H
#define PS_H

#include <stdbool.h>
#include <libspectre/spectre.h>
#include <zathura/document.h>

#if HAVE_CAIRO
#include <cairo.h>
#endif

typedef struct ps_document_s
{
  SpectreDocument* document;
} ps_document_t;

zathura_plugin_error_t ps_document_open(zathura_document_t* document);
zathura_plugin_error_t ps_document_free(zathura_document_t* document);
zathura_page_t* ps_page_get(zathura_document_t* document, unsigned int page, zathura_plugin_error_t* error);
zathura_image_buffer_t* ps_page_render(zathura_page_t* page, zathura_plugin_error_t* error);
#if HAVE_CAIRO
zathura_plugin_error_t ps_page_render_cairo(zathura_page_t* page, cairo_t* cairo, bool printing);
#endif
zathura_plugin_error_t ps_page_free(zathura_page_t* page);

#endif // PS_H
