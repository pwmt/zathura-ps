/* See LICENSE file for license and copyright information */

#include "plugin.h"

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
