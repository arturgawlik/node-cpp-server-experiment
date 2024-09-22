{
    'targets': [{
        'target_name': 'server',
        'sources': ['src/cpp/server.cc'],
        'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
      },
    }]
}