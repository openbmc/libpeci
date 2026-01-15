project(
    'libpeci',
    'c',
    'cpp',
    version: '1.0',
    meson_version: '>=1.1.1',
    default_options: [
        'b_ndebug=if-release',
        'cpp_rtti=false',
        'cpp_std=c++23',
        'warning_level=3',
        'werror=true',
    ],
)

if (get_option('raw-peci').allowed())
    sdbusplus = dependency('sdbusplus')

    systemd = dependency('systemd', required: true)
    systemd_system_unit_dir = systemd.get_variable(
        pkgconfig: 'systemd_system_unit_dir',
    )

    boost = dependency('boost', version: '>=1.82')
    add_project_arguments(
        [
            '-DBOOST_ASIO_DISABLE_THREADS',
            '-DBOOST_ASIO_EXCEPTION_DISABLE',
            '-DBOOST_ASIO_NO_DEPRECATED',
            '-DBOOST_NO_RTTI',
            '-DBOOST_NO_TYPEID',
        ],
        language: 'cpp',
    )
endif

c = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

common_c_warn = [
    '-Wcast-align',
    '-Wconversion',
    '-Wdouble-promotion',
    '-Wformat=2',
    '-Wsign-conversion',
    '-Wunused',
    '-Wno-unused-parameter',
    '-Wno-psabi',
]

common_cpp_warn = [
    '-Wcast-align',
    '-Wconversion',
    '-Wdouble-promotion',
    '-Wformat=2',
    '-Wnull-dereference',
    '-Wsign-conversion',
    '-Wunused',
    '-Wno-psabi',
    '-Wno-unused-parameter',
    '-fno-rtti',
]

gcc_only = ['-Wduplicated-branches', '-Wduplicated-cond', '-Wlogical-op']

if c.get_id() == 'gcc'
    add_project_arguments(common_c_warn + gcc_only, language: 'c')
else
    add_project_arguments(common_c_warn, language: 'c')
endif

if cxx.get_id() == 'gcc'
    add_project_arguments(common_cpp_warn + gcc_only, language: 'cpp')
else
    add_project_arguments(common_cpp_warn, language: 'cpp')
endif

libpeci = library(
    'peci',
    'peci.c',
    version: meson.project_version(),
    install: true,
)
install_headers('peci.h')

libpeci_dep = declare_dependency(
    link_with: libpeci,
    include_directories: include_directories('.'),
)

bindir = get_option('prefix') + '/' + get_option('bindir')

executable(
    'peci_cmds',
    'peci_cmds.c',
    link_with: libpeci,
    install: true,
    install_dir: bindir,
)

if (get_option('raw-peci').allowed())
    executable(
        'raw-peci',
        'dbus_raw_peci.cpp',
        dependencies: [boost, sdbusplus, systemd],
        link_with: libpeci,
        install: true,
        install_dir: bindir,
    )
    subdir('service_files')
endif

import('pkgconfig').generate(
    libpeci,
    name: meson.project_name(),
    version: meson.project_version(),
    description: 'PECI utilities',
)
