/* See LICENSE file for license and copyright information */

#ifndef PS_H
#define PS_H

#include <stdbool.h>
#include <libspectre/spectre.h>
#include <zathura/document.h>

#if HAVE_CAIRO
#include <cairo.h>
#endif

/**
 * PostScript document
 */
typedef struct ps_document_s
{ 
  SpectreDocument* document; /**< libspectre document */
} ps_document_t;

/**
 * Open a PostScript document
 *
 * @param document Zathura document
 * @return ZATHURA_PLUGIN_ERROR_OK if no error occured otherwise see \ref
 *   zathura_plugin_error_t
 */
zathura_plugin_error_t ps_document_open(zathura_document_t* document);

/**
 * Closes and frees the internal document structure
 *
 * @param document Zathura document
 * @return ZATHURA_PLUGIN_ERROR_OK if no error occured otherwise see \ref
 *   zathura_plugin_error_t
 */
zathura_plugin_error_t ps_document_free(zathura_document_t* document);

/**
 * Returns a reference to a page
 *
 * @param document Zathura document
 * @param page Page number
 * @return A page object or NULL if an error occurred
 */
zathura_page_t* ps_page_get(zathura_document_t* document, unsigned int page, zathura_plugin_error_t* error);

/**
 * Renders a page and returns a allocated image buffer which has to be freed
 * with zathura_image_buffer_free
 *
 * @param page Page
 * @return Image buffer or NULL if an error occurred
 */
zathura_image_buffer_t* ps_page_render(zathura_page_t* page, zathura_plugin_error_t* error);

#if HAVE_CAIRO
/**
 * Renders a page onto a cairo object
 *
 * @param page Page
 * @param cairo Cairo object
 * @return  true if no error occurred, otherwise false
 */
zathura_plugin_error_t ps_page_render_cairo(zathura_page_t* page, cairo_t* cairo, bool printing);
#endif

/**
 * Frees a PostScript page
 *
 * @param page Page
 * @return ZATHURA_PLUGIN_ERROR_OK if no error occured otherwise see \ref
 *   zathura_plugin_error_t
 */
zathura_plugin_error_t ps_page_free(zathura_page_t* page);

#endif // PS_H
