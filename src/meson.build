compiler = meson.get_compiler('c')
hid_lib = compiler.find_library('hid')
winscard_lib = compiler.find_library('winscard')
setupapi_lib = compiler.find_library('setupapi')

deps = [
    dependency('tomlc99'), 
    dependency('minhook'),
    hid_lib,
    winscard_lib,
    setupapi_lib,
]

shared_library(
    'cardreader',
    implicit_include_directories : false,    
    vs_module_defs : 'cardreader.def',
    sources: [
        'helpers.c',
        'cardreader_plugin/dllmain.c',
        'cardreader_plugin/CardIO/cardio.c',
        'cardreader_plugin/SmartCard/scard.c',
    ],
    dependencies: deps,
    include_directories: [
        inc
    ]
)