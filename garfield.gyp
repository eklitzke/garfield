# -*- python -*-
{
  'target_defaults': {
    'cflags': ['-pedantic', '-Wall', '-std=c++0x'],
    'libraries': [
      '-lboost_system',
      '-pthread',
    ],
  },
  'targets': [
    {
      'type': 'static_library',
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
      ],
    },
    {
      'target_name': 'example-server',
      'type': 'executable',
      'cflags': ['-g'],
      'libraries': [
        '-lboost_program_options',
        '-lboost_regex',
      ],
      'dependencies': ['libgarfield'],
      'sources': [
        'example-server.cc',
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
