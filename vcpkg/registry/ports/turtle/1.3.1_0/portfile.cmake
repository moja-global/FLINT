#header-only library
include(vcpkg_common_functions)

vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO mat007/turtle
  REF 5c0f29012511339ba5cc2672f99a1356c5387b62
  SHA512 d452fe35f5f87c6a0d77d95039ab1615830b2c78cf640a3d5703250cd3b7ff041b4782953e206526688e786bbae09aeea7b9905ef1e116bd38abb5512c26707e
  HEAD_REF master
)

# Put the licence file where vcpkg expects it
file(INSTALL
    ${SOURCE_PATH}/LICENSE_1_0.txt
    DESTINATION ${CURRENT_PACKAGES_DIR}/share/turtle RENAME copyright)		

# Copy the turtle header files
file(INSTALL 
	${SOURCE_PATH}/include 
	DESTINATION ${CURRENT_PACKAGES_DIR} FILES_MATCHING PATTERN "*.hpp")
