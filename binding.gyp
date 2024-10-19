{
    "targets": [{
        "target_name": "tcp-server",
        "sources": ["src/cpp/tcp-server.cc"],
        "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
        "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
        "cflags_cc": [ "-fexceptions" ]
    }]
}
