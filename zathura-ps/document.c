/* SPDX-License-Identifier: Zlib */

#include <string.h>
#include <glib.h>

#include "plugin.h"

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
ps_document_free(zathura_document_t* document, void* data)
{
  SpectreDocument* spectre_document = data;
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
ps_document_save_as(zathura_document_t* document, void* data, const char* path)
{
  SpectreDocument* spectre_document = data;
  if (document == NULL || spectre_document == NULL || path == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  if (g_str_has_suffix(path, ".pdf") == true) {
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
