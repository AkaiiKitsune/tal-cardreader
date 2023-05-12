project(
    'minhook',
    'c',
    default_options : ['c_std=c99']
)

sources = files(
    'src/buffer.c',
    'src/hde/hde32.c',
    'src/hde/hde64.c',
    'src/hook.c',
    'src/trampoline.c'
)

includes = include_directories('include')

minhook_lib = static_library(
    'minhook',
    sources,
    include_directories: includes,
)

minhook_dep = declare_dependency(
    include_directories: includes,
    link_with: minhook_lib,
)

meson.override_dependency('minhook', minhook_dep)
 