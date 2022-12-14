# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2018-2022 OKTET Labs Ltd. All rights reserved.

c_args = []

if dep_threads.found()
    if cc.has_function('sem_init', dependencies: dep_threads)
        c_args + [ '-DHAVE_SEM_INIT' ]
    endif
endif

if get_option('large-logs')
    c_args + [ '-D_LARGEFILE64_SOURCE', '-D_FILE_OFFSET_BITS=64' ]
endif

logger_sources = [
    'logger.c',
    'logger_cnf.c',
    'logger_cnf_int.c',
    'logger_bufs.c',
    'logger_listener.c',
    'logger_stream.c',
    'logger_stream_rules.c',
    'logger_prc.c',
    'te_log_sniffers.c'
]

dep_yaml = dependency('yaml-0.1', required: false)
required_deps += 'yaml-0.1'
if not dep_yaml.found()
    missed_deps += 'yaml-0.1'
endif

dep_jansson = dependency('jansson', required: false)
required_deps += 'jansson'
if not dep_jansson.found()
    missed_deps += 'jansson'
endif

dep_libcurl = dependency('libcurl', required: false)
required_deps += 'libcurl'
if not dep_libcurl.found()
    missed_deps += 'libcurl'
endif

executable('te_logger', logger_sources, install: true,
           include_directories: te_include,
           c_args: c_args,
           dependencies: [ dep_popt, dep_threads,
                           dep_lib_ipcserver, dep_lib_rcfapi, dep_lib_ipc,
                           dep_lib_tools, dep_lib_logger_core,
                           dep_lib_log_proc, dep_yaml, dep_jansson,
                           dep_libcurl ])

executable('te_log_shutdown', 'te_log_shutdown.c', install: true,
           include_directories: te_include,
           c_args: c_args,
           dependencies: [ dep_lib_ipc, dep_lib_rcfapi,
                           dep_lib_logger_core ])

executable('te_log_flush', 'te_log_flush.c', install: true,
           include_directories: te_include,
           c_args: c_args,
           dependencies: [ dep_lib_rcfapi, dep_lib_logger_ten, dep_lib_ipc,
                           dep_lib_tools, dep_lib_logger_core ])

executable('te_tee', 'te_tee.c', install: true,
           include_directories: te_include,
           c_args: c_args,
           dependencies: [ dep_lib_logger_ten, dep_lib_ipc, dep_lib_tools,
                           dep_lib_logger_core ])

executable('te_log_get_path', ['te_log_get_path.c', 'logger_cnf_int.c'],
           install: true,
           include_directories: te_include,
           c_args: c_args,
           dependencies: [dep_lib_tools, dep_yaml])

scripts = [
    'te_log_archive',
    'te_log_init',
    'te_log_message',
]
install_data(scripts, install_dir: 'bin')
