/* See LICENSE file for license and copyright information */

#include "plugin.h"

ZATHURA_PLUGIN_REGISTER_WITH_FUNCTIONS(
  "ps",
  VERSION_MAJOR, VERSION_MINOR, VERSION_REV,
  ZATHURA_PLUGIN_FUNCTIONS({
    .document_open            = (zathura_plugin_document_open_t) ps_document_open,
    .document_free            = (zathura_plugin_document_free_t) ps_document_free,
    .page_init                = (zathura_plugin_page_init_t) ps_page_init,
    .page_clear               = (zathura_plugin_page_clear_t) ps_page_clear,
    .page_render              = (zathura_plugin_page_render_t) ps_page_render,
    .document_save_as         = (zathura_plugin_document_save_as_t) ps_document_save_as,
    .document_get_information = (zathura_plugin_document_get_information_t) ps_document_get_information,
    .page_render_cairo        = (zathura_plugin_page_render_cairo_t) ps_page_render_cairo,
  }),
  ZATHURA_PLUGIN_MIMETYPES({
    "application/postscript",
    "application/eps",
    "application/x-eps",
    "image/eps",
    "image/x-eps"
  })
)
