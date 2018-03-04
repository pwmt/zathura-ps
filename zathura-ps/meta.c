/* See LICENSE file for license and copyright information */

#include "plugin.h"

girara_list_t*
ps_document_get_information(zathura_document_t* document, void* data, zathura_error_t* error)
{
  SpectreDocument* spectre_document = data;
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
