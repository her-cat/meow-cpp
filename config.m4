PHP_ARG_ENABLE(meow, whether to enable meow support,
[  --enable-meow          Enable meow support], no)

# AC_CANONICAL_HOST

if test "$PHP_MEOW" != "no"; then
    PHP_ADD_LIBRARY_WITH_PATH(uv, /usr/local/lib/, MEOW_SHARED_LIBADD)
    PHP_SUBST(MEOW_SHARED_LIBADD)
    PHP_ADD_LIBRARY(pthread)
    MEOW_ASM_DIR="thirdparty/boost/asm/"
    CFLAGS="-Wall -pthread $CFLAGS"

dnl 判断机器所使用的操作系统

    AS_CASE([$host_os],
        [linux*], [MEOW_OS="LINUX"],
        []
    )

dnl 判断 CPU 类型

    AS_CASE([$host_cpu],
        [x86_64*], [MEOW_CPU="x86_64"],
        [x86*], [MEOW_CPU="x86"],
        [i?86*], [MEOW_CPU="x86"],
        [arm*], [MEOW_CPU="arm"],
        [aarch64*], [MEOW_CPU="arm64"],
        [arm64*], [MEOW_CPU="arm64"],
        []
    )

dnl 根据操作系统及 CPU 类型使用对应的汇编文件

	if test "$MEOW_CPU" = "x86_64"; then
        if test "$MEOW_OS" = "LINUX"; then
            MEOW_CONTEXT_ASM_FILE="x86_64_sysv_elf_gas.S"
      fi
    elif test "$MEOW_CPU" = "x86"; then
        if test "$MEOW_OS" = "LINUX"; then
            MEOW_CONTEXT_ASM_FILE="i386_sysv_elf_gas.S"
        fi
    elif test "$MEOW_CPU" = "arm"; then
        if test "$MEOW_OS" = "LINUX"; then
            MEOW_CONTEXT_ASM_FILE="arm_aapcs_elf_gas.S"
        fi
    elif test "$MEOW_CPU" = "arm64"; then
        if test "$MEOW_OS" = "LINUX"; then
            MEOW_CONTEXT_ASM_FILE="arm64_aapcs_elf_gas.S"
        fi
    elif test "$MEOW_CPU" = "mips32"; then
        if test "$MEOW_OS" = "LINUX"; then
            MEOW_CONTEXT_ASM_FILE="mips32_o32_elf_gas.S"
        fi
    fi

    meow_source_file="\
        meow.cc \
        meow_coroutine.cc \
        meow_coroutine_util.cc \
dnl        meow_coroutine_server.cc \
        meow_coroutine_channel.cc \
        meow_coroutine_socket.cc \
        src/coroutine/coroutine.cc \
        src/coroutine/context.cc \
        src/coroutine/socket.cc \
        src/coroutine/channel.cc \
        src/socket.cc \
        src/log.cc \
        src/error.cc \
        src/timer.cc \
        src/core/base.cc \
        ${MEOW_ASM_DIR}make_${MEOW_CONTEXT_ASM_FILE} \
        ${MEOW_ASM_DIR}jump_${MEOW_CONTEXT_ASM_FILE}
    "

dnl 声明扩展的名称、需要的源文件名、编译形式

    PHP_NEW_EXTENSION(meow, $meow_source_file, $ext_shared, ,, cxx)

dnl 添加额外的包含头文件的目录

    PHP_ADD_INCLUDE([$ext_srcdir])
    PHP_ADD_INCLUDE([$ext_srcdir/include])

dnl 复制头文件

    PHP_INSTALL_HEADERS([ext/meow], [*.h config.h include/*.h thirdparty/*.h])

dnl 使用 C++

    PHP_REQUIRE_CXX()

dnl 指定编译 C++ 用到的编译选项

    CXXFLAGS="$CXXFLAGS -Wall -Wno-unused-function -Wno-deprecated -Wno-deprecated-declarations"
    CXXFLAGS="$CXXFLAGS -std=c++11"

dnl 编译 boost 提供的代码

    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost)
    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost/asm)
fi
