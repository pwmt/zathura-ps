/* SPDX-License-Identifier: Zlib */

#include "plugin.h"

ZATHURA_PLUGIN_REGISTER_WITH_FUNCTIONS(
  "ps",
  VERSION_MAJOR, VERSION_MINOR, VERSION_REV,
  ZATHURA_PLUGIN_FUNCTIONS({
    .document_open            = ps_document_open,
    .document_free            = ps_document_free,
    .page_init                = ps_page_init,
    .page_clear               = ps_page_clear,
    .page_render              = ps_page_render,
    .document_save_as         = ps_document_save_as,
    .document_get_information = ps_document_get_information,
    .page_render_cairo        = ps_page_render_cairo,
  }),
  ZATHURA_PLUGIN_MIMETYPES({
    "application/postscript",
    "application/eps",
    "application/x-eps",
    "image/eps",
    "image/x-eps"
  })
)
