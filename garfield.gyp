# -*- python -*-
{
  'target_defaults': {
    'cflags': ['-pedantic', '-Wall', '-std=c++0x'],
    'libraries': [
      '-lboost_system',
      '-lboost_regex',
      '-pthread',
    ],
  },
  'targets': [
    {
      'type': 'shared_library',
      'cflags': ['-fPIC'],
      'target_name': 'libgarfield',
      'sources': [
        'src/connection.cc',
        'src/handlers.cc',
        'src/headers.cc',
        'src/logging.cc',
        'src/request.cc',
        'src/response.cc',
        'src/server.cc',
        'src/time.cc',
      ],
    },
    {
      'type': 'static_library',
      'target_name': 'garfield_static',
      'sources': [
        'src/connection.cc',
        'src/handlers.cc',
        'src/headers.cc',
        'src/logging.cc',
        'src/request.cc',
        'src/response.cc',
        'src/server.cc',
        'src/time.cc',
      ],
    },
    {
      'target_name': 'example-server',
      'type': 'executable',
      'cflags': ['-g'],
      'libraries': [
        '-lboost_program_options',
      ],
      'dependencies': ['garfield_static'],
      'sources': [
        'example-server.cc',
      ],
    },
    {
      'target_name': 'file-server',
      'type': 'executable',
      'cflags': ['-g'],
      'libraries': [
        '-lboost_program_options',
      ],
      'dependencies': ['garfield_static'],
      'sources': [
        'file-server.cc',
      ],
    },
    {
      'target_name': 'http_bench',
      'type': 'executable',
      'libraries': [
        '-lboost_program_options'
      ],
      'sources': [
        'http_bench.cc',
      ],
    },

  ],
}
