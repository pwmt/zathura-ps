/* See LICENSE file for license and copyright information */

#include "plugin.h"

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
  functions->page_render_cairo        = (zathura_plugin_page_render_cairo_t) ps_page_render_cairo;
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
