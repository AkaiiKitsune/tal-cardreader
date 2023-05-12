project(
    'tomlc99',
    'c',
    default_options : ['c_std=c99']
)

sources = files('toml.c')

includes = include_directories('.')

tomlc99_lib = static_library(
    'tomlc99',
    sources,
    include_directories: includes,
)

tomlc99_dep = declare_dependency(
    include_directories: includes,
    link_with: tomlc99_lib,
)

meson.override_dependency('tomlc99', tomlc99_dep)
