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

PLUGIN_REGISTER(
    "ps",
    0, 1, 0,
    ps_document_open,
    PLUGIN_MIMETYPES({
      "application/postscript",
      "application/eps",
      "application/x-eps",
      "image/eps",
      "image/x-eps"
    })
  )

zathura_plugin_error_t
ps_document_open(zathura_document_t* document)
{
  zathura_plugin_error_t error = ZATHURA_PLUGIN_ERROR_OK;

  if (document == NULL) {
    error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
    goto error_ret;
  }

  document->functions.document_free     = ps_document_free;
  document->functions.page_get          = ps_page_get;
  document->functions.page_render       = ps_page_render;
  document->functions.document_save_as  = ps_document_save_as;
  document->functions.document_meta_get = ps_document_meta_get;
#if HAVE_CAIRO
  document->functions.page_render_cairo = ps_page_render_cairo;
#endif
  document->functions.page_free         = ps_page_free;

  document->data = malloc(sizeof(ps_document_t));
  if (document->data == NULL) {
    error = ZATHURA_PLUGIN_ERROR_OUT_OF_MEMORY;
    goto error_ret;
  }

  ps_document_t* ps_document = (ps_document_t*) document->data;
  ps_document->document      = spectre_document_new();

  if (ps_document->document == NULL) {
    error = ZATHURA_PLUGIN_ERROR_OUT_OF_MEMORY;
    goto error_free;
  }

  spectre_document_load(ps_document->document, document->file_path);

  if (spectre_document_status(ps_document->document) != SPECTRE_STATUS_SUCCESS) {
    error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
    goto error_free;
  }

  document->number_of_pages = spectre_document_get_n_pages(ps_document->document);

  return error;

error_free:

  if (ps_document->document != NULL) {
    spectre_document_free(ps_document->document);
  }

  free(document->data);
  document->data = NULL;

error_ret:

  return error;
}

zathura_plugin_error_t
ps_document_free(zathura_document_t* document)
{
  if (document == NULL) {
    return ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
  }

  if (document->data != NULL) {
    ps_document_t* ps_document = (ps_document_t*) document->data;
    spectre_document_free(ps_document->document);
    free(document->data);
    document->data = NULL;
  }

  return ZATHURA_PLUGIN_ERROR_OK;
}

zathura_plugin_error_t
ps_document_save_as(zathura_document_t* document, const char* path)
{
  if (document == NULL || document->data == NULL || path == NULL) {
    return ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
  }

  ps_document_t* ps_document = (ps_document_t*) document->data;

  const char* extension = get_extension(path);

  if (extension != NULL && g_strcmp0(extension, "pdf") == 0) {
    spectre_document_save_to_pdf(ps_document->document, path);
  } else {
    spectre_document_save(ps_document->document, path);
  }

  if (spectre_document_status(ps_document->document) != SPECTRE_STATUS_SUCCESS) {
    return ZATHURA_PLUGIN_ERROR_UNKNOWN;
  } else {
    return ZATHURA_PLUGIN_ERROR_OK;
  }
}

char*
ps_document_meta_get(zathura_document_t* document, zathura_document_meta_t meta,
    zathura_plugin_error_t* error)
{
  if (document == NULL || document->data == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
    }
    return NULL;
  }

  /* get document information */
  ps_document_t* ps_document = (ps_document_t*) document->data;

  const char* creator = spectre_document_get_creator(ps_document->document);
  if (creator == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
    }
    return NULL;
  }

  /* process value */
  const char* string_value = NULL;

  switch (meta) {
    case ZATHURA_DOCUMENT_TITLE:
      string_value = spectre_document_get_title(ps_document->document);
      break;
    case ZATHURA_DOCUMENT_AUTHOR:
      string_value = (creator != NULL) ? creator :
        spectre_document_get_for(ps_document->document);
      break;
    case ZATHURA_DOCUMENT_CREATION_DATE:
      string_value = spectre_document_get_creation_date(ps_document->document);
      break;
    default:
      if (error != NULL) {
        *error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
      }
      return NULL;
  }

  if (string_value == NULL || strlen(string_value) == 0) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
    }

    return NULL;
  }

  return g_strdup(string_value);
}

zathura_page_t*
ps_page_get(zathura_document_t* document, unsigned int page, zathura_plugin_error_t* error)
{
  if (document == NULL || document->data == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
    }
    return NULL;
  }

  ps_document_t* ps_document    = (ps_document_t*) document->data;
  zathura_page_t* document_page = malloc(sizeof(zathura_page_t));

  if (document_page == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_OUT_OF_MEMORY;
    }
    goto error_ret;
  }

  SpectrePage* ps_page = spectre_document_get_page(ps_document->document, page);

  if (ps_page == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
    }
    goto error_free;
  }

  int page_width;
  int page_height;
  spectre_page_get_size(ps_page, &(page_width), &(page_height));

  document_page->width    = page_width;
  document_page->height   = page_height;
  document_page->document = document;
  document_page->data     = ps_page;

  return document_page;

error_free:

  free(document_page);

error_ret:

  return NULL;
}

zathura_plugin_error_t
ps_page_free(zathura_page_t* page)
{
  if (page == NULL) {
    return ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
  }

  if (page->data != NULL) {
    SpectrePage* ps_page = (SpectrePage*) page->data;
    spectre_page_free(ps_page);
  }

  free(page);

  return ZATHURA_PLUGIN_ERROR_OK;
}

zathura_image_buffer_t*
ps_page_render(zathura_page_t* page, zathura_plugin_error_t* error)
{
  if (page == NULL || page->data == NULL || page->document == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
    }
    goto error_ret;
  }

  /* calculate sizes */
  unsigned int page_width  = page->document->scale * page->width;
  unsigned int page_height = page->document->scale * page->height;

  /* create image buffer */
  zathura_image_buffer_t* image_buffer = zathura_image_buffer_create(page_width, page_height);

  if (image_buffer == NULL) {
    if (error != NULL) {
      *error = ZATHURA_PLUGIN_ERROR_OUT_OF_MEMORY;
    }
    goto error_ret;
  }

  SpectrePage* ps_page          = (SpectrePage*) page->data;
  SpectreRenderContext* context = spectre_render_context_new();

  if (context == NULL) {
    goto error_ret;
  }

  spectre_render_context_set_scale(context, page->document->scale, page->document->scale);
  spectre_render_context_set_rotation(context, 0);

  unsigned char* page_data;
  int row_length;
  spectre_page_render(ps_page, context, &page_data, &row_length);
  spectre_render_context_free(context);

  if (page_data == NULL || spectre_page_status(ps_page) != SPECTRE_STATUS_SUCCESS) {
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

  if (error != NULL && *error == ZATHURA_PLUGIN_ERROR_OK) {
    *error = ZATHURA_PLUGIN_ERROR_UNKNOWN;
  }

  return NULL;
}

#if HAVE_CAIRO
zathura_plugin_error_t
ps_page_render_cairo(zathura_page_t* page, cairo_t* cairo, bool GIRARA_UNUSED(printing))
{
  if (page == NULL || page->data == NULL || cairo == NULL) {
    return ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
  }

  cairo_surface_t* surface = cairo_get_target(cairo);
  if (surface == NULL) {
    return ZATHURA_PLUGIN_ERROR_INVALID_ARGUMENTS;
  }

  int rowstride            = cairo_image_surface_get_stride(surface);
  unsigned char* image     = cairo_image_surface_get_data(surface);
  unsigned int page_width  = cairo_image_surface_get_width(surface);
  unsigned int page_height = cairo_image_surface_get_height(surface);

  SpectrePage* ps_page          = (SpectrePage*) page->data;
  SpectreRenderContext* context = spectre_render_context_new();

  if (context == NULL) {
    return ZATHURA_PLUGIN_ERROR_UNKNOWN;
  }

  double scalex = ((double) page_width) / page->width,
         scaley = ((double) page_height) / page->height;

  spectre_render_context_set_scale(context, scalex, scaley);

  unsigned char* page_data;
  int row_length;
  spectre_page_render(ps_page, context, &page_data, &row_length);
  spectre_render_context_free(context);

  if (page_data == NULL || spectre_page_status(ps_page) != SPECTRE_STATUS_SUCCESS) {
    if (page_data != NULL) {
      free(page_data);
    }

    return ZATHURA_PLUGIN_ERROR_UNKNOWN;
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

  return ZATHURA_PLUGIN_ERROR_OK;
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
