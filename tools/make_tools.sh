#!/bin/bash

# Check that there are parameters
if [[ $# -eq 0 ]] ; then
    echo
    echo 'Usage: ./make_tools.sh ARCH build_type host_type'
    echo
    echo ' ARCH       - arm, x86'
    echo ' build_type - clean, binutils, gcc, gdb, all'
    echo ' host_type  - cross, native'
    echo
    exit 1
fi

# Store the architecture
case "$1" in

"arm")
    ARCH=arm
    ;;

"x86")
    ARCH=x86
    ;;
esac

# Store the build type
case "$2" in

"clean")
    BUILDTYPE=BUILD_CLEAN
    ;;
"binutils")
    BUILDTYPE=BUILD_BINUTILS
    ;;
"gcc")
    BUILDTYPE=BUILD_GCC
    ;;
"gdb")
    BUILDTYPE=BUILD_GDB
    ;;
"all")
    BUILDTYPE=BUILD_ALL
    ;;
esac

# Store the host type
case "$3" in

"cross")
    HOSTTYPE=cross
    ;;
"native")
    HOSTTYPE=native
    ;;
esac

if [[ $ARCH = "arm" ]] ; then
echo 'for ARM'
export TARGET=arm-anvil-eabi
export GCC_ARCH_ARGS="\
 --enable-plugins \
 --disable-decimal-float \
 --disable-libffi \
 --disable-libgomp \
 --disable-libmudflap \
 --disable-libquadmath \
 --disable-libssp \
 --enable-tls \
 --disable-nls \
 --disable-shared \
 --enable-threads=posix \
 --disable-wchar_t \
 --with-fpu=vfp \
 --disable-clocale \
 --disable-libstdcxx-verbose \
 --enable-cxx-flags=-fno-exceptions \
 --disable-tls \
 "
fi

if [[ $ARCH = "x86" ]] ; then
echo 'for x86'
export TARGET=x86_64-anvil
export GCC_ARCH_ARGS="--enable-shared --disable-nls"
export COMMON_ARGS="--disable-initfini-array"
#--disable-libssp --enable-multilib
fi

export BUILDROOT=$PWD/../

if [[ $HOSTTYPE = "cross" ]] ; then
    echo 'Building Anvil cross toolchain'
    export SYSROOT=$BUILDROOT/sysroot-$ARCH/
    export PREFIX=$BUILDROOT/cross-tools/
fi

if [[ $HOSTTYPE = "native" ]] ; then
    echo 'Building Anvil native toolchain'
    export SYSROOT=/
    export PREFIX=/usr
    export HOST=x86_64-anvil
    export BUILD_SYSROOT=$BUILDROOT/sysroot-$ARCH/
    export DESTDIR_STRING="DESTDIR=$BUILD_SYSROOT"
    export CONFIG_STRING="--with-build-sysroot=$BUILD_SYSROOT --host=$HOST"
fi

export BINUTILS_VER=2.32
export GCC_VER=9.1.0
export GMP_VER=6.1.1
export MPC_VER=1.0.3
export MPFR_VER=3.1.4
export GDB_VER=7.11

# ======== CLEAN ====================

function clean_all {
    
#    rm -rf $PREFIX
    mkdir -p $PREFIX

#    rm -rf $SYSROOT
    mkdir -p $SYSROOT/usr/include
    cp -R $BUILDROOT/src/libc/inc/* $SYSROOT/usr/include
}

# ======== BINUTILS ====================

function build_binutils {

    # Delete old binutils build first
    rm -rf binutils-build-$TARGET
    rm -rf binutils-$BINUTILS_VER
    
    tar -Jxf binutils-$BINUTILS_VER.tar.xz
    
    cd binutils-$BINUTILS_VER
    patch -p 1 < ../binutils.patch
    cd ..
    
    cp -R binutils.files/* binutils-$BINUTILS_VER
    
    mkdir binutils-build-$TARGET
    cd binutils-build-$TARGET
    ../binutils-$BINUTILS_VER/configure $COMMON_ARGS --disable-werror --enable-multilib --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT $CONFIG_STRING
    make -j 4 all
    echo $DESTDIR_STRING make install
    make install $DESTDIR_STRING
    cd ..
    
    # Clean up
    #rm -rf binutils-build-$TARGET
    #rm -rf binutils-$BINUTILS_VER
}

# ======== GCC =========================

function build_gcc {

    # Delete old gcc build first
    rm -rf gcc-build-$TARGET
    rm -rf gcc-$GCC_VER
    
    tar -Jxf gcc-$GCC_VER.tar.xz
    
    tar -jxf gmp-$GMP_VER.tar.bz2
    mv gmp-$GMP_VER gmp
    mv gmp gcc-$GCC_VER
    
    tar -xf mpc-$MPC_VER.tar.gz
    mv mpc-$MPC_VER mpc
    mv mpc gcc-$GCC_VER
    
    tar -jxf mpfr-$MPFR_VER.tar.bz2
    mv mpfr-$MPFR_VER mpfr
    mv mpfr gcc-$GCC_VER
    
    cd gcc-$GCC_VER
    patch -p 1 < ../gcc.patch
    cd ..
    
    cp -R gcc.files/* gcc-$GCC_VER
    
    mkdir gcc-build-$TARGET
    cd gcc-build-$TARGET
    CXXFLAGS="-O2 -fbracket-depth=1024" ../gcc-$GCC_VER/configure $COMMON_ARGS --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-libstdcxx-pch --disable-werror --enable-languages=c,c++ --with-newlib --disable-libstdcxx-verbose $GCC_ARCH_ARGS $CONFIG_STRING --enable-multilib --disable-nls --disable-libssp --enable-languages=c,c++ --with-newlib --with-system-zlib
    
    make -j 4 all-gcc
    make install-gcc $DESTDIR_STRING
    make -j 4 all-target-libgcc
    make install-target-libgcc $DESTDIR_STRING
    cd ..
    
    # Clean up
    #rm -rf gcc-build-$TARGET
    #rm -rf gcc-$GCC_VER
}

# ======== GDB =========================

function build_gdb {

    # Delete old Gdb build first
    rm -rf gdb-build-$TARGET
    rm -rf gdb-$GDB_VER
    
    tar -Jxf gdb-$GDB_VER.tar.xz

    cd gdb-$GDB_VER
    patch -p 1 < ../gdb.patch
    cd ..

    cp -R gdb.files/* gdb-$GDB_VER

    mkdir gdb-build-$TARGET
    cd gdb-build-$TARGET
    ../gdb-$GDB_VER/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --enable-install-libiberty $CONFIG_STRING
    make all
    make install $DESTDIR_STRING
    cd ..
    
    # Clean up
    #rm -rf gdb-build-$TARGET
    #rm -rf gdb-$GDB_VER
}

# ======================================

if [[ $BUILDTYPE = "BUILD_CLEAN" || $BUILDTYPE = "BUILD_ALL" ]] ; then
    echo 'Cleaning all cross tools'
#    clean_all
fi

if [[ $BUILDTYPE = "BUILD_BINUTILS" || $BUILDTYPE = "BUILD_ALL" ]] ; then
    echo 'Building binutils...'
#    build_binutils
fi

if [[ $BUILDTYPE = "BUILD_GCC" || $BUILDTYPE = "BUILD_ALL" ]] ; then
    echo 'Building gcc...'
    build_gcc
fi

if [[ $BUILDTYPE = "BUILD_GDB" || $BUILDTYPE = "BUILD_ALL" ]] ; then
    echo 'Building gdb...'
#    build_gdb
fi

echo 'Done.'
