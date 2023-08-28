#pragma once

#if !defined(ALIGNED_TYPE_BEGIN)
    #if defined(_MSC_VER) || defined(__INTEL_COMPILER_BUILD_DATE)
        #define PACKED_STRUCT_BEGIN \
            __pragma(pack(push,1)) \
            struct __declspec(align(1))
        #define PACKED_STRUCT_END \
            ;__pragma(pack(pop))
    #else
        #define PACKED_STRUCT_BEGIN \
            struct
        #define PACKED_STRUCT_END  __attribute__((packed));
    #endif
#endif
