project(
  'libpeci',
  'c', 'cpp',
  version : '1.0',
  meson_version: '>=1.1.1',
  default_options: [
    'b_ndebug=if-release',
    'cpp_rtti=false',
    'cpp_std=c++20',
    'warning_level=3',
    'werror=true',
  ]
)

if (get_option('raw-peci').enabled())
  sdbusplus = dependency('sdbusplus')

  systemd = dependency('systemd', required: true)
  systemd_system_unit_dir = systemd.get_variable(pkgconfig:
      'systemdsystemunitdir'
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
    language : 'cpp'
  )
endif

add_project_arguments(
  [
    '-Wcast-align',
    '-Wconversion',
    '-Wdouble-promotion',
    '-Wduplicated-branches',
    '-Wduplicated-cond',
    '-Wformat=2',
    '-Wlogical-op',
    '-Wsign-conversion',
    '-Wunused',
    '-Wno-unused-parameter',
    '-Wno-psabi',
  ],
  language : 'c'
)

add_project_arguments(
  [
    '-Wcast-align',
    '-Wconversion',
    '-Wdouble-promotion',
    '-Wduplicated-branches',
    '-Wduplicated-cond',
    '-Wformat=2',
    '-Wlogical-op',
    '-Wnull-dereference',
    '-Wsign-conversion',
    '-Wunused',
    '-Wno-psabi',
    '-Wno-unused-parameter',
    '-fno-rtti',
  ],
  language : 'cpp'
)

libpeci = library(
  'peci',
  'peci.c'
)

bindir = get_option('prefix') + '/' + get_option('bindir')

if (get_option('raw-peci').enabled())
  executable(
    'raw-peci',
    'dbus_raw_peci.cpp',
    dependencies: [
      boost,
      sdbusplus,
      systemd,
    ],
    link_with: libpeci,
    install: true,
    install_dir: bindir
  )
  subdir('service_files')
endif
