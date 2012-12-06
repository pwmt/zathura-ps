/* See LICENSE file for license and copyright information */

#include <stdlib.h>
#include <girara/datastructures.h>
#include <string.h>
#include <glib.h>

#if HAVE_CAIRO
#include <cairo.h>
#endif

#include "ps.h"

/* forward declaration */
static const char* get_extension(const char* path);

void
register_functions(zathura_plugin_functions_t* functions)
{
  functions->document_open            = (zathura_plugin_document_open_t) ps_document_open;
  functions->document_free            = (zathura_plugin_document_free_t) ps_document_free;
  functions->page_init                = (zathura_plugin_page_init_t) ps_page_init;
  functions->page_clear               = (zathura_plugin_page_clear_t) ps_page_clear;
  functions->page_render              = (zathura_plugin_page_render_t) ps_page_render;
  functions->document_save_as         = (zathura_plugin_document_save_as_t) ps_document_save_as;
  functions->document_get_information = (zathura_plugin_document_get_information_t) ps_document_get_information;
#if HAVE_CAIRO
  functions->page_render_cairo        = (zathura_plugin_page_render_cairo_t) ps_page_render_cairo;
#endif
}

ZATHURA_PLUGIN_REGISTER(
  "ps",
  VERSION_MAJOR, VERSION_MINOR, VERSION_REV,
  register_functions,
  ZATHURA_PLUGIN_MIMETYPES({
    "application/postscript",
    "application/eps",
    "application/x-eps",
    "image/eps",
    "image/x-eps"
  })
)

zathura_error_t
ps_document_open(zathura_document_t* document)
{
  zathura_error_t error = ZATHURA_ERROR_OK;
  if (document == NULL) {
    error = ZATHURA_ERROR_UNKNOWN;
    goto error_ret;
  }

  SpectreDocument* spectre_document = spectre_document_new();
  if (spectre_document == NULL) {
    error = ZATHURA_ERROR_OUT_OF_MEMORY;
    goto error_free;
  }

  spectre_document_load(spectre_document, zathura_document_get_path(document));

  if (spectre_document_status(spectre_document) != SPECTRE_STATUS_SUCCESS) {
    error = ZATHURA_ERROR_UNKNOWN;
    goto error_free;
  }

  zathura_document_set_data(document, spectre_document);
  zathura_document_set_number_of_pages(document, spectre_document_get_n_pages(spectre_document));

  return error;

error_free:

  if (spectre_document != NULL) {
    spectre_document_free(spectre_document);
  }

error_ret:

  return error;
}

zathura_error_t
ps_document_free(zathura_document_t* document, SpectreDocument* spectre_document)
{
  if (document == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  if (spectre_document != NULL) {
    spectre_document_free(spectre_document);
    zathura_document_set_data(document, NULL);
  }

  return ZATHURA_ERROR_OK;
}

zathura_error_t
ps_document_save_as(zathura_document_t* document, SpectreDocument* spectre_document, const char* path)
{
  if (document == NULL || spectre_document == NULL || path == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  const char* extension = get_extension(path);

  if (extension != NULL && g_strcmp0(extension, "pdf") == 0) {
    spectre_document_save_to_pdf(spectre_document, path);
  } else {
    spectre_document_save(spectre_document, path);
  }

  if (spectre_document_status(spectre_document) != SPECTRE_STATUS_SUCCESS) {
    return ZATHURA_ERROR_UNKNOWN;
  } else {
    return ZATHURA_ERROR_OK;
  }
}

girara_list_t*
ps_document_get_information(zathura_document_t* document, SpectreDocument*
    spectre_document, zathura_error_t* error)
{
  if (document == NULL || spectre_document == NULL) {
    if (error != NULL) {
      *error = ZATHURA_ERROR_INVALID_ARGUMENTS;
    }
    return NULL;
  }

  girara_list_t* list = zathura_document_information_entry_list_new();
  if (list == NULL) {
    return NULL;
  }

  /* get document information */
  zathura_document_information_entry_t* entry = NULL;

  const char* creator = spectre_document_get_creator(spectre_document);
  entry = zathura_document_information_entry_new(ZATHURA_DOCUMENT_INFORMATION_CREATOR, creator);
  girara_list_append(list, entry);

  const char* title = spectre_document_get_title(spectre_document);
  entry = zathura_document_information_entry_new(ZATHURA_DOCUMENT_INFORMATION_TITLE, title);
  girara_list_append(list, entry);

  const char* author = spectre_document_get_for(spectre_document);
  entry = zathura_document_information_entry_new(ZATHURA_DOCUMENT_INFORMATION_AUTHOR, author);
  girara_list_append(list, entry);

  const char* creation_date = spectre_document_get_creation_date(spectre_document);
  entry = zathura_document_information_entry_new(ZATHURA_DOCUMENT_INFORMATION_CREATION_DATE, creation_date);
  girara_list_append(list, entry);

  return list;
}

zathura_error_t
ps_page_init(zathura_page_t* page, SpectrePage* spectre_page)
{
  if (page == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  zathura_document_t* document      = zathura_page_get_document(page);
  SpectreDocument* spectre_document = zathura_document_get_data(document);

  SpectrePage* ps_page = spectre_document_get_page(spectre_document, zathura_page_get_index(page));
  if (ps_page == NULL) {
    return ZATHURA_ERROR_UNKNOWN;
  }

  int page_width;
  int page_height;
  spectre_page_get_size(ps_page, &(page_width), &(page_height));

  zathura_page_set_width(page, page_width);
  zathura_page_set_height(page, page_height);
  zathura_page_set_data(page, ps_page);

  return ZATHURA_ERROR_OK;
}

zathura_error_t
ps_page_clear(zathura_page_t* page, SpectrePage* spectre_page)
{
  if (page == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  if (spectre_page != NULL) {
    spectre_page_free(spectre_page);
  }

  return ZATHURA_ERROR_OK;
}

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

#if HAVE_CAIRO
zathura_error_t
ps_page_render_cairo(zathura_page_t* page, SpectrePage* spectre_page, cairo_t* cairo, bool GIRARA_UNUSED(printing))
{
  if (page == NULL || cairo == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  SpectrePage* ps_page     = (SpectrePage*) zathura_page_get_data(page);;
  cairo_surface_t* surface = cairo_get_target(cairo);

  if (ps_page == NULL || surface == NULL) {
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
#endif

static const char*
get_extension(const char* path)
{
  if (path == NULL) {
    return NULL;
  }

  unsigned int i = strlen(path);
  for (; i > 0; i--) {
    if (*(path + i) != '.') {
      continue;
    } else {
      break;
    }
  }

  if (i == 0) {
    return NULL;
  }

  return path + i + 1;
}
