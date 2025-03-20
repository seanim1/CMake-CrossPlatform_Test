#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin
  make -f /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin
  make -f /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin
  make -f /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin
  make -f /Users/sean/Documents/GitHub/CMake-CrossPlatform_Test/bin/CMakeScripts/ReRunCMake.make
fi

