# ---------------------------------------------------------------------
# Tests for ReversingSpace/GameFileSystem.
# This is split from the make CMakeLists.txt to keep it cleaner.
#
# This can be split further, if needs be.
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
# Internals (basic page test)
# ---------------------------------------------------------------------

set(INTERNALS_TEST_SOURCES
    "${PROJECT_SOURCE_DIR}/tests/internals/main.cpp"
)

set(INTERNALS_TEST_INCLUDE_DIRS 
    "${PROJECT_SOURCE_DIR}/tests/internals/"
)

option(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS
    "Simple test for storage system (internals)"
    OFF
)

rs_gfs_test(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS
    "revspace-storage-test-internals"
    ${INTERNALS_TEST_INCLUDE_DIRS}
    ${INTERNALS_TEST_SOURCES}
    "" # No libs
)

# ---------------------------------------------------------------------
# Internals read/write (internal-rw)
#
# This tests reading and writing.  The file tested is removed at the
# end of the testing.
# ---------------------------------------------------------------------

set(INTERNAL_RW_TEST_SOURCES
    "${PROJECT_SOURCE_DIR}/tests/internals-rw/main.cpp"
)

set(INTERNAL_RW_TEST_INCLUDE_DIRS 
    "${PROJECT_SOURCE_DIR}/tests/internals-rw/"
)

option(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW
    "Simple r/w test for storage system"
    OFF
)

rs_gfs_test(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW
    "revspace-storage-test-internal-rw"
    ${INTERNAL_RW_TEST_INCLUDE_DIRS}
    ${INTERNAL_RW_TEST_SOURCES}
    "" # No libs
)

# ---------------------------------------------------------------------