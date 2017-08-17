{
    "variables": {
        "libdirs": [],
        "incdirs": ["<!(node -e \"require('nan')\")"],
        "sources": [ "<!@(python -c \"import glob,sys,os,os.path; \
                        [sys.stdout.write(f.replace(os.sep, 2*os.sep) + os.linesep) \
                        for f in glob.glob(os.path.join('.','cpp','*.cpp'))]\")"],
        "extradefs": ["<!(node -e \"console.log((function() { \
                        if ( process.env.ENABLE_ADDONTRC ) { \
                            return 'ENABLE_ADDONTRC=1'; \
                        }})())\")"]
    },
    "targets": [
        {
            "target_name": "synccore",
            "sources": [ "<@(sources)" ],
            "include_dirs": [ "<@(incdirs)" ],
            "defines": [
                "<@(extradefs)",
                "V8_DEPRECATION_WARNINGS=1"
            ],
            "cflags!": [
                "-fno-exceptions"
            ],
            "cflags_cc!": [
                "-fno-exceptions"
            ],
            "conditions": [
                [
                    "OS=='linux'",
                    {
                        "libraries": [
                            "-lrt",
                            "-ldl"
                        ],
                        "configurations": {
                            "Debug": {
                                'defines': [ 'DEBUG', '_DEBUG',
                                             'STACK_CHECK_BUILTIN',
                                             'STACK_CHECK_STATIC_BUILTIN' ],
                                'cflags': [ '-g', '-O0',
                                            '-fstack-check',
                                            '-fstack-protector-all',
                                            '-fstack-usage' ],
                                'cflags_cc': [ '-g', '-O0',
                                            '-fstack-check',
                                            '-fstack-protector-all',
                                            '-fstack-usage' ]
                            },
                            "Release": {
                            }
                        },
                        "defines": [ "SYNCCORE_OS_LINUX=1" ]
                    },
                    "OS=='win'",
                    {
                        "variables": {
                        },
                        "defines": [ "SYNCCORE_OS_WIN=1" ],
                        "link_settings": {
                            "libraries": [
                            ]
                        },
                        "configurations": {
                            "Debug": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "ExceptionHandling": "0",
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLibrarianTool": {
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLinkerTool": {
                                        "LinkTimeCodeGeneration": 1,
                                        "LinkIncremental": 1,
                                        "AdditionalLibraryDirectories": [
                                            "<@(libdirs)"
                                        ]
                                    }
                                }
                            },
                            "Release": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "RuntimeLibrary": 0,
                                        "Optimization": 3,
                                        "FavorSizeOrSpeed": 1,
                                        "InlineFunctionExpansion": 2,
                                        "WholeProgramOptimization": "true",
                                        "OmitFramePointers": "true",
                                        "EnableFunctionLevelLinking": "true",
                                        "EnableIntrinsicFunctions": "true",
                                        "RuntimeTypeInfo": "false",
                                        "ExceptionHandling": "0",
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLibrarianTool": {
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLinkerTool": {
                                        "LinkTimeCodeGeneration": 1,
                                        "OptimizeReferences": 2,
                                        "EnableCOMDATFolding": 2,
                                        "LinkIncremental": 1,
                                        "AdditionalLibraryDirectories": [
                                            "<@(libdirs)"
                                        ]
                                    }
                                }
                            }
                        }
                    }
                ]
            ]
        }
    ]
}
