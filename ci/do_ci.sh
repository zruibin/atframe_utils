#!/bin/bash

cd "$(cd "$(dirname $0)" && pwd)/..";

set -ex ;

if [[ "$1" == "format" ]]; then
  python3 -m pip install --user -r ./ci/requirements.txt ;
  export PATH="$HOME/.local/bin:$PATH"
  bash ./ci/format.sh ;
  CHANGED="$(git -c core.autocrlf=true ls-files --modified)" ;
  if [[ ! -z "$CHANGED" ]]; then
    echo "The following files have changes:" ;
    echo "$CHANGED" ;
    git diff ;
    # exit 1 ; # Just warning, some versions of clang-format have different default style for unsupport syntax
  fi
  exit 0 ;
elif [[ "$1" == "coverage" ]]; then
  if [[ "x$USE_SSL" == "xmbedtls" ]]; then
    vcpkg install --triplet=$VCPKG_TARGET_TRIPLET mbedtls ;
    CRYPTO_OPTIONS="-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_CRYPTO_USE_MBEDTLS=ON" ;
  else
    vcpkg install --triplet=$VCPKG_TARGET_TRIPLET openssl ;
    CRYPTO_OPTIONS="-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_CRYPTO_USE_OPENSSL=ON" ;
  fi
  vcpkg install --triplet=$VCPKG_TARGET_TRIPLET fmt ;
  bash cmake_dev.sh -lus -b Debug -r build_jobs_coverage -c $USE_CC -- $CRYPTO_OPTIONS "-DCMAKE_C_FLAGS=$GCOV_FLAGS" "-DCMAKE_CXX_FLAGS=$GCOV_FLAGS" "-DCMAKE_EXE_LINKER_FLAGS=$GCOV_FLAGS" -DCMAKE_TOOLCHAIN_FILE=$VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=$VCPKG_TARGET_TRIPLET ;
  cd build_jobs_coverage ;
  cmake --build . ;
  ctest . -V ;
elif [[ "$1" == "ssl.openssl" ]]; then
  if [[ "x$USE_CC" == "xclang" ]]; then
    echo '#include <iostream>
    int main() { std::cout<<"Hello"; }' > test-libc++.cpp
    SELECT_CLANG_VERSION="";
    SELECT_CLANG_HAS_LIBCXX=1;
    clang -x c++ -stdlib=libc++ test-libc++.cpp -lc++ -lc++abi || SELECT_CLANG_HAS_LIBCXX=0;
    if [[ $SELECT_CLANG_HAS_LIBCXX -eq 0 ]]; then
      CURRENT_CLANG_VERSION=$(clang -x c /dev/null -dM -E | grep __clang_major__ | awk '{print $NF}');
      for ((i=$CURRENT_CLANG_VERSION+3;$i>=$CURRENT_CLANG_VERSION-3;--i)); do
        SELECT_CLANG_HAS_LIBCXX=1;
        SELECT_CLANG_VERSION="-$i";
        clang$SELECT_CLANG_VERSION -x c++ -stdlib=libc++ test-libc++.cpp -lc++ -lc++abi || SELECT_CLANG_HAS_LIBCXX=0;
        if [[ $SELECT_CLANG_HAS_LIBCXX -eq 1 ]]; then
          break;
        fi
      done
    fi
    SELECT_CLANGPP_BIN=clang++$SELECT_CLANG_VERSION;
    LINK_CLANGPP_BIN=0;
    which $SELECT_CLANGPP_BIN || LINK_CLANGPP_BIN=1;
    if [[ $LINK_CLANGPP_BIN -eq 1 ]]; then
      mkdir -p .local/bin ;
      ln -s "$(which "clang$SELECT_CLANG_VERSION")" "$PWD/.local/bin/clang++$SELECT_CLANG_VERSION" ;
      export PATH="$PWD/.local/bin:$PATH";
    fi
    export USE_CC=clang$SELECT_CLANG_VERSION;
  fi
  CRYPTO_OPTIONS="-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_CRYPTO_USE_OPENSSL=ON" ;
  vcpkg install --triplet=$VCPKG_TARGET_TRIPLET fmt openssl ;
  bash cmake_dev.sh -lus -b Debug -r build_jobs_ci -c $USE_CC -- $CRYPTO_OPTIONS -DCMAKE_TOOLCHAIN_FILE=$VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=$VCPKG_TARGET_TRIPLET ;
  cd build_jobs_ci ;
  cmake --build . -j ;
  ctest . -V ;
elif [[ "$1" == "ssl.mbedtls" ]]; then
  CRYPTO_OPTIONS="-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_CRYPTO_USE_MBEDTLS=ON=ON" ;
  vcpkg install --triplet=$VCPKG_TARGET_TRIPLET fmt mbedtls ;
  bash cmake_dev.sh -lus -b Debug -r build_jobs_ci -c $USE_CC -- $CRYPTO_OPTIONS -DCMAKE_TOOLCHAIN_FILE=$VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=$VCPKG_TARGET_TRIPLET ;
  cd build_jobs_ci ;
  cmake --build . -j ;
  ctest . -V ;
elif [[ "$1" == "gcc.legacy.test" ]]; then
  bash cmake_dev.sh -lus -b Debug -r build_jobs_ci -c $USE_CC ;
  cd build_jobs_ci ;
  cmake --build . -j ;
  ctest . -V ;
elif [[ "$1" == "msys2.mingw.test" ]]; then
  pacman -S --needed --noconfirm mingw-w64-x86_64-cmake git mingw-w64-x86_64-git-lfs m4 curl wget tar autoconf automake mingw-w64-x86_64-toolchain mingw-w64-x86_64-libtool python || true ;
  git config --global http.sslBackend openssl ;
  mkdir -p build_jobs_ci ;
  cd build_jobs_ci ;
  cmake .. -G 'MinGW Makefiles' "-DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS" -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON -DPROJECT_ENABLE_TOOLS=ON ;
  cmake --build . -j ;
  for EXT_PATH in $(find ../third_party/install/ -name "*.dll" | xargs dirname | sort -u); do
    export PATH="$PWD/$EXT_PATH:$PATH"
  done
  ctest . -V ;
fi
