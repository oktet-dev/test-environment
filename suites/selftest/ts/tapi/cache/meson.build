# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2019-2022 OKTET Labs Ltd. All rights reserved.

tests = [
    'tc_prologue',
    'tc_actualize_indirect',
    'tc_actualize_root',
    'tc_actualize_star',
    'tc_actualize_sub',
    'tc_actualize_unregistered',
    'tc_invalidate_all',
    'tc_invalidate',
    'tc_operation_add_get',
    'tc_operation_del',
    'tc_operation_find',
    'tc_operation_get',
    'tc_register',
]

foreach test : tests
    test_exe = test
    test_c = test + '.c'
    package_tests_c += [ test_c ]
    executable(test_exe, test_c, install: true, install_dir: package_dir,
               dependencies: test_deps)
endforeach

tests_info_xml = custom_target(package_dir.underscorify() + 'tests-info-xml',
                               install: true, install_dir: package_dir,
                               input: package_tests_c,
                               output: 'tests-info.xml', capture: true,
                               command: [ te_tests_info_sh,
                                          meson.current_source_dir() ])

install_data([ 'package.xml' ], install_dir: package_dir)
