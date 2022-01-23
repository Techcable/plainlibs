project(
  'plainlibs',
  'c',
  version: '0.1.0-beta.2',
  license: ['MIT', 'CC0-1.0'],
  default_options : ['warning_level=3']
)

plainlib_includes = include_directories('include')

plainlib_sources = []

if get_option('staticlib')
  # Declares plainlibs as a static library
  # 
  # This is only nessicary if the library you're using has a seperate source file.
  # Header only utilities do not need to build this
  plainlib_link = [static_library(
     'plain', # Name ourselves just 'plain' (so we become libplain.a on Linux)
     include_directories: plainlib_includes,
  )]
else
  plainlib_link = []
endif

plainlib_dep = declare_dependency(
  include_directories: plainlib_includes,
  # This is off by defualt (unless you set 'staticlib')
  #
  # If yuu do set it, the linker should optimzie
  # away unused functions
  link_with: plainlib_link,
)
