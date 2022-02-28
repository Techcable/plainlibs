# Tests for plainlibs
#
# These use the excellent Criterion framework for testing
#
# If this is not found on the system, then no tests will be run.

criterion = dependency(
  'criterion',
  # Only used for testing
  required: get_option('tests')
)

if get_option('tests').disabled()
  subdir_done()
elif not get_option('staticlib')
  error('running tests requires the `staticlib` option be enabled')
endif

test_sources = [
  'argparse.c',
  'intbuiltins.c',
  'intmath.c'
]

plainlib_tests = executable(
  'plainlib-test',
  test_sources,
  dependencies: [plainlib_dep, criterion]
)

# Tell meson about the tests
test('plainlib', plainlib_tests, args: ['--tap'], protocol: 'tap')

