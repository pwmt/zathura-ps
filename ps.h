/* See LICENSE file for license and copyright information */

#ifndef PS_H
#define PS_H

#include <stdbool.h>
#include <libspectre/spectre.h>
#include <zathura/plugin-api.h>

#if HAVE_CAIRO
#include <cairo.h>
#endif

/**
 * Open a PostScript document
 *
 * @param document Zathura document
 * @return ZATHURA_ERROR_OK if no error occured otherwise see
 *   zathura_error_t
 */
zathura_error_t ps_document_open(zathura_document_t* document);

/**
 * Closes and frees the internal document structure
 *
 * @param document Zathura document
 * @return ZATHURA_ERROR_OK if no error occured otherwise see
 *   zathura_error_t
 */
zathura_error_t ps_document_free(zathura_document_t* document, SpectreDocument* spectre_document);

/**
 * Saves the document to the given path
 *
 * @param document Zathura document
 * @param path File path
 * @return ZATHURA_ERROR_OK when no error occured, otherwise see
 *    zathura_error_t
 */
zathura_error_t ps_document_save_as(zathura_document_t* document, SpectreDocument* spectre_document, const char* path);

/**
 * Returns the content of a given meta field
 *
 * @param document Zathura document
 * @param meta Meta identifier
 * @param error Set to an error value (see zathura_error_t) if an
 *   error occured
 * @return Value of the meta data or NULL if an error occurred
 */
char* ps_document_meta_get(zathura_document_t* document, SpectreDocument* spectre_document,
    zathura_document_meta_t meta, zathura_error_t* error);

/**
 * Returns a reference to a page
 *
 * @param page Page object
 * @return ZATHURA_ERROR_OK if no error occured otherwise see
 *   zathura_error_t
 */
zathura_error_t ps_page_init(zathura_page_t* page, SpectrePage* spectre_page);

/**
 * Renders a page and returns a allocated image buffer which has to be freed
 * with zathura_image_buffer_free
 *
 * @param page Page
 * @param error Set to an error value (see zathura_error_t) if an
 *   error occured
 * @return Image buffer or NULL if an error occurred
 */
zathura_image_buffer_t* ps_page_render(zathura_page_t* page, SpectrePage* spectre_page, zathura_error_t* error);

#if HAVE_CAIRO
/**
 * Renders a page onto a cairo object
 *
 * @param page Page
 * @param cairo Cairo object
 * @param printing Set to true if page should be rendered for printing
 * @return ZATHURA_ERROR_OK if no error occured otherwise see
 *   zathura_error_t
 */
zathura_error_t ps_page_render_cairo(zathura_page_t* page, SpectrePage* spectre_page, cairo_t* cairo, bool printing);
#endif

/**
 * Frees a PostScript page
 *
 * @param page Page
 * @return ZATHURA_ERROR_OK if no error occured otherwise see
 *   zathura_error_t
 */
zathura_error_t ps_page_clear(zathura_page_t* page, SpectrePage* spectre_page);

#endif // PS_H
