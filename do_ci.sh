#!/bin/bash

# Run a CI build/test target, e.g. docs, asan.

set -ex

. "$(dirname "$0")"/envoy/ci/build_setup.sh
echo "building using ${NUM_CPUS} CPUs"

function bazel_release_binary_build() {
  echo "Building..."
  cd "${ENVOY_CI_DIR}"
  bazel --batch build ${BAZEL_BUILD_OPTIONS} -c opt //:envoy.stamped
  # Copy the envoy-static binary somewhere that we can access outside of the
  # container.
  cp -f \
    "${ENVOY_CI_DIR}"/bazel-bin/envoy.stamped \
    "${ENVOY_DELIVERY_DIR}"/envoy
}

function bazel_debug_binary_build() {
  echo "Building..."
  cd "${ENVOY_CI_DIR}"
  bazel --batch build ${BAZEL_BUILD_OPTIONS} -c dbg //:envoy.stamped
  # Copy the envoy-static binary somewhere that we can access outside of the
  # container.
  cp -f \
    "${ENVOY_CI_DIR}"/bazel-bin/envoy.stamped \
    "${ENVOY_DELIVERY_DIR}"/envoy-debug
}

if [[ "$1" == "bazel.release" ]]; then
  setup_gcc_toolchain
  echo "bazel release build with tests..."
  bazel_release_binary_build
  echo "Testing..."
  bazel --batch test ${BAZEL_TEST_OPTIONS} -c opt //... @envoy//test/...
  exit 0
elif [[ "$1" == "bazel.release.server_only" ]]; then
  setup_gcc_toolchain
  echo "bazel release build..."
  bazel_release_binary_build
  exit 0
elif [[ "$1" == "bazel.debug" ]]; then
  setup_gcc_toolchain
  echo "bazel debug build with tests..."
  bazel_debug_binary_build
  echo "Testing..."
  bazel --batch test ${BAZEL_TEST_OPTIONS} -c dbg //... @envoy//test/...
  exit 0
elif [[ "$1" == "bazel.dev" ]]; then
  setup_clang_toolchain
  # This doesn't go into CI but is available for developer convenience.
  echo "bazel fastbuild build with tests..."
  cd /source
  echo "Building..."
  bazel --batch build ${BAZEL_BUILD_OPTIONS} -c fastbuild //:envoy
  # Copy the envoy-static binary somewhere that we can access outside of the
  # container for developers.
  cp -f \
    "${ENVOY_CI_DIR}"/bazel-bin/envoy \
    "${ENVOY_DELIVERY_DIR}"/envoy-fastbuild
  echo "Building and testing..."
  bazel --batch test ${BAZEL_TEST_OPTIONS} -c fastbuild //... @envoy//test/...
  exit 0
elif [[ "$1" == "fix_format" ]]; then
  echo "fix_format..."
  cd "${ENVOY_SRCDIR}"
  ./tools/check_format.py fix
  exit 0
elif [[ "$1" == "check_format" ]]; then
  echo "check_format..."
  cd "${ENVOY_SRCDIR}"
  ./tools/check_format.py check
  exit 0
else
  echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
  exit 1
fi
