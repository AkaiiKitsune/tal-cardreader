compiler = meson.get_compiler('c')
hid_lib = compiler.find_library('hid')
setupapi_lib = compiler.find_library('setupapi')

deps = [
    dependency('tomlc99'), 
    dependency('minhook'),
    hid_lib,
    setupapi_lib
]

shared_library(
    'cardio',
    implicit_include_directories : false,    
    vs_module_defs : 'cardio.def',
    sources: [
        'helpers.c',
        'cardio_plugin/dllmain.c',
        'cardio_plugin/cardio.c',
    ],
    dependencies: deps,
    include_directories: [
        inc
    ]
)