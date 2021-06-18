#FROM mulliongroup/moja.mulliongroup.baseimage:ubuntu-20.04
FROM gcr.io/flintpro-212105/flintpro/moja.mulliongroup.baseimage_vscode:ubuntu-20.04 as builder

ENV DEBIAN_FRONTEND=noninteractive

ARG CACHE_DATE=2021-05-27.1
ARG FLINT_BRANCH=develop

ENV ROOTDIR /usr

RUN apt-get update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends gdb apt-utils dialog gettext sqlformat postgresql-client 2>&1

#RUN apt-get install -y apt-utils dialog gettext sqlformat

#
# GET  moja.global - Mullion Fork. Build Debug and Release
#
# RUN git clone --recursive --depth 1 -b ${FLINT_BRANCH} https://github.com/MullionGroup/FLINT.git flint \
#     && mkdir -p flint/Source/build && cd flint/Source/build \
#     && cmake -DCMAKE_BUILD_TYPE=DEBUG  \
#             -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
#             -DENABLE_TESTS:BOOL=OFF \
#             -DENABLE_MOJA.MODULES.GDAL=ON \
#             -DDEBUG=ON \
#             -DBoost_USE_STATIC_LIBS=OFF \
#             -DBUILD_SHARED_LIBS=ON \
#             -DCMAKE_TOOLCHAIN_FILE=$ROOTDIR/src/vcpkg/scripts/buildsystems/vcpkg.cmake \
#             .. \
# 	&& make --quiet -j$(nproc) \
# 	&& make --quiet install \
#     && cd $ROOTDIR/src

# RUN mkdir -p flint/Source/build_release && cd flint/Source/build_release \
#     && cmake -DCMAKE_BUILD_TYPE=RELEASE  \
#             -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
#             -DENABLE_TESTS:BOOL=OFF \
#             -DENABLE_MOJA.MODULES.GDAL=ON \
#             -DBoost_USE_STATIC_LIBS=OFF \
#             -DBUILD_SHARED_LIBS=ON \
#             -DCMAKE_TOOLCHAIN_FILE=$ROOTDIR/src/vcpkg/scripts/buildsystems/vcpkg.cmake \
#             .. \
# 	&& make --quiet -j$(nproc) \
# 	&& make --quiet install \
#     && cd $ROOTDIR/src

# PROJ dependencies
RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
        libsqlite3-0 libtiff5 libcurl4 \
        curl unzip ca-certificates

# GDAL dependencies
RUN apt-get update -y; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
        python3-numpy libpython3.8 \
        libexpat1 \
        libxerces-c3.2 \
        libwebp6 \
        libzstd1 bash libpq5 libssl1.1 libopenjp2-7

RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y python-is-python3 python3-pip 

WORKDIR /usr/src/vcpkg  
RUN ./vcpkg install --triplet x64-linux boost-format boost-thread boost-filesystem boost-iostreams boost-program-options boost-random boost-crc boost-test libzip bzip2 liblzma zlib zstd snappy avro-cpp[snappy]

# set environment variables
ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
ENV GDAL_DATA /usr/share/gdal
ENV GDAL_HTTP_VERSION 2

ENV PATH $ROOTDIR/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTDIR/lib:$ROOTDIR/src/vcpkg/installed/x64-linux/lib:$LD_LIBRARY_PATH
ENV PYTHONPATH /usr/lib:$PYTHONPATH

RUN ln -s $ROOTDIR/lib/libmoja.modules.* $ROOTDIR/bin

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN ldconfig

### # TEST TEST
### FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu-20.04 as devtools
### 
### # PROJ dependencies
### RUN apt-get update; \
###     DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
###         libsqlite3-0 libtiff5 libcurl4 \
###         curl unzip ca-certificates
### 
### # GDAL dependencies
### RUN apt-get update -y; \
###     DEBIAN_FRONTEND=noninteractive apt-get install -y  --no-install-recommends \
###         python3-numpy libpython3.8 \
###         libexpat1 \
###         libxerces-c3.2 \
###         libwebp6 \
###         libzstd1 bash libpq5 libssl1.1 libopenjp2-7
### 
### RUN apt-get update; \
###     DEBIAN_FRONTEND=noninteractive apt-get install -y python-is-python3 python3-pip 
### 
### # set environment variables
### ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
### ENV GDAL_DATA /usr/share/gdal
### ENV GDAL_HTTP_VERSION 2
### 
### ENV PATH /usr/bin:$PATH
### ENV LD_LIBRARY_PATH /usr/lib:$LD_LIBRARY_PATH
### ENV PYTHONPATH /usr/lib:$PYTHONPATH
### 
### # ==================================================================================================================
### # Moja and Mullion libraries and Binaries
### # ==================================================================================================================
### 
### COPY --from=builder /usr/lib/ /usr/lib/
### COPY --from=builder /usr/bin/ /usr/bin/
### COPY --from=builder /usr/share/ /usr/share/
### COPY --from=builder /usr/include/gdal_version.h /usr/include 
### 
### RUN ldconfig
### # RUN ln -s /usr/lib/libmoja.modules.*.so /usr/bin/

ENV DEBIAN_FRONTEND=dialog
