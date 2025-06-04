# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================
set(LLVM_PLUGINS
    CountCond
    CostEstimation
    TimeEstimation
    Unreachable
    Swap
    Merge
    OpAnalyzer
    GetFuncRange
    ReplaceFunc
    ReplaceDivtoMul
    ReplaceDiv
    ReplaceIcmp
    GetDivisorRange
    )

set(CountCond_SOURCES
  CountCond.cpp
  )
set(CostEstimation_SOURCES
  CostEstimation.cpp
  )
set(TimeEstimation_SOURCES
  TimeEstimation.cpp
  )
set(Unreachable_SOURCES
  Unreachable.cpp
  )
set(Swap_SOURCES
  Swap.cpp
  )
set(Merge_SOURCES
  Merge.cpp
  )
set(OpAnalyzer_SOURCES
  OpAnalyzer.cpp
  )
set(GetFuncRange_SOURCES
  GetFuncRange.cpp
  )
set(ReplaceFunc_SOURCES
  ReplaceFunc.cpp
  )
set(ReplaceDivtoMul_SOURCES
  ReplaceDivtoMul.cpp
  )
set(ReplaceDiv_SOURCES
  ReplaceDiv.cpp
  )
set(ReplaceIcmp_SOURCES
  ReplaceIcmp.cpp
  )
set(GetDivisorRange_SOURCES
  GetDivisorRange.cpp
  )

# CONFIGURE THE PLUGIN LIBRARIES
# ==============================
foreach( plugin ${LLVM_PLUGINS} )
    # Create a library corresponding to 'plugin'
    add_library(
      ${plugin}
      SHARED
      ${${plugin}_SOURCES}
      )

    # Configure include directories for 'plugin'
    target_include_directories(
      ${plugin}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    # On Darwin (unlike on Linux), undefined symbols in shared objects are not
    # allowed at the end of the link-edit. The plugins defined here:
    #  - _are_ shared objects
    #  - reference symbols from LLVM shared libraries, i.e. symbols which are
    #    undefined until those shared objects are loaded in memory (and hence
    #    _undefined_ during static linking)
    # The build will fail with errors like this:
    #    "Undefined symbols for architecture x86_64"
    # with various LLVM symbols being undefined. Since those symbols are later
    # loaded and resolved at runtime, these errors are false positives.
    # This behaviour can be modified via the '-undefined' OS X linker flag as
    # follows.
    target_link_libraries(
      ${plugin}
      "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
      )
endforeach()
