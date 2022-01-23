# Tests for plainlibs
#
# Enabled with the 'tests'  option

if get_option('tests').disabled()
  subdir_done()
elif not get_option('staticlib')
  error('running tests requires the `staticlib` option be enabled')
endif

test_targets = [
  'argparse'
]

foreach target : test_targets
  test_exec = executable(
    'plainlib-test-' + target,
    [target + '.c'],
    dependencies: [plainlib_dep]
  )
  # Tell meson about the tests
  test(target, test_exec)
endforeach


