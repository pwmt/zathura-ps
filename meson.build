project('zathura-ps', 'c',
  version: '0.2.8',
  meson_version: '>=0.61',
  default_options: ['c_std=c17', 'warning_level=3']
)

version = meson.project_version()
version_array = version.split('.')

cc = meson.get_compiler('c')

datadir = get_option('datadir')
desktopdir = join_paths(datadir, 'applications')
metainfodir = join_paths(datadir, 'metainfo')

# required dependencies
zathura = dependency('zathura', version: '>=0.3.8')
girara = dependency('girara-gtk3')
glib = dependency('glib-2.0')
cairo = dependency('cairo')
spectre = dependency('libspectre')

build_dependencies = [zathura, girara, glib, cairo, spectre]

if get_option('plugindir') == ''
  plugindir = zathura.get_variable(pkgconfig: 'plugindir')
else
  plugindir = get_option('plugindir')
endif

# defines
defines = [
  '-DVERSION_MAJOR=@0@'.format(version_array[0]),
  '-DVERSION_MINOR=@0@'.format(version_array[1]),
  '-DVERSION_REV=@0@'.format(version_array[2]),
  '-D_DEFAULT_SOURCE',
]

# compile flags
flags = [
  '-Wall',
  '-Wextra',
  '-pedantic',
  '-Werror=implicit-function-declaration',
  '-Werror=vla',
  '-fvisibility=hidden'
]
flags = cc.get_supported_arguments(flags)

sources = files(
  'zathura-ps/document.c',
  'zathura-ps/meta.c',
  'zathura-ps/page.c',
  'zathura-ps/plugin.c',
  'zathura-ps/render.c'
)

ps = shared_module('ps',
  sources,
  dependencies: build_dependencies,
  c_args: defines + flags,
  install: true,
  install_dir: plugindir
)

subdir('data')
