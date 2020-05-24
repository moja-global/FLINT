#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------

# https://hub.docker.com/_/microsoft-vscode-devcontainers
# docker build --build-arg NUM_CPU=8 -t moja/baseimage_vscode:ubuntu-18.04 .

FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu-18.04
#FROM moja/baseimage:mg

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# This Dockerfile's base image has a non-root user with sudo access. Use the "remoteUser"
# property in devcontainer.json to use it. On Linux, the container user's GID/UIDs
# will be updated to match your local UID/GID (when using the dockerFile property).
# See https://aka.ms/vscode-remote/containers/non-root-user for details.
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID
ARG NUM_CPU=1

# Configure apt and install packages
RUN apt-get update \
    #
    # Install C++ tools
    && apt-get -y install --fix-missing \
    #--no-install-recommends \
        automake                                \
        bash-completion                         \
        build-essential                         \
        ca-certificates                         \
        cmake                                   \
        cppcheck                                \
        doxygen                                 \
        doxygen-latex                           \
        git                                     \
        graphviz                                \
        libcurl4-gnutls-dev                     \
        libexpat-dev                            \
        libgeos-dev                             \
        libpoppler-dev                          \
        libpq-dev                               \
        libproj-dev                             \
        libspatialite-dev                       \
        libsqlite3-dev                          \
        libssl-dev                              \
        libtool                                 \
        libxerces-c-dev                         \
        libxml2-dev                             \
        nasm                                    \
        openssl                                 \
        pkg-config                              \
        postgis                                 \
        postgresql-client-10                    \
        postgresql-server-dev-10                \
        python-dev                              \
        python-numpy                            \
        python-pip                              \
        python3-dev                             \
        python3-numpy                           \
        python3-pip                             \
        software-properties-common              \
        sqlite3                                 \
        valgrind                                \
        wget                                    \
        zlib1g-dev                              \
    # [Optional] Update UID/GID if needed
    && if [ "$USER_GID" != "1000" ] || [ "$USER_UID" != "1000" ]; then \
        groupmod --gid $USER_GID $USERNAME \
        && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
        && chown -R $USER_UID:$USER_GID /home/$USERNAME; \
    fi  \
    #
    # Clean up
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

ENV ROOTDIR /usr/local
ENV GDAL_VERSION 2.4.1
ENV CMAKE_VERSION 3.15.4
ENV POCO_VERSION 1.9.2
ENV BOOST_VERSION 1_72_0
ENV BOOST_VERSION_DOT 1.72.0
ENV FMT_VERSION 6.1.2
ENV SQLITE_VERSION 3.31.1

WORKDIR $ROOTDIR/

# set environment variables
ENV PATH $ROOTDIR/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTDIR/lib:$LD_LIBRARY_PATH
ENV PYTHONPATH $ROOTDIR/lib:$PYTHONPATH

WORKDIR $ROOTDIR/src

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz \
    && tar xzf cmake-${CMAKE_VERSION}.tar.gz \
    && cd cmake-${CMAKE_VERSION} \
    && ./bootstrap --system-curl --parallel=$NUM_CPU \
	&& make --quiet install \
	&& make clean \
    && cd ..

RUN wget https://pocoproject.org/releases/poco-${POCO_VERSION}/poco-${POCO_VERSION}-all.tar.gz \
    && tar -xzf poco-${POCO_VERSION}-all.tar.gz && mkdir poco-${POCO_VERSION}-all/cmake-build && cd poco-${POCO_VERSION}-all/cmake-build \
    &&  cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
            -DPOCO_UNBUNDLED=ON \
            -DPOCO_STATIC=OFF \
            -DENABLE_ENCODINGS=OFF \
            -DENABLE_ENCODINGS_COMPILER=OFF \
            -DENABLE_XML=OFF \
            -DENABLE_JSON=ON \
            -DENABLE_MONGODB=OFF \
            -DENABLE_REDIS=OFF \
            -DENABLE_PDF=OFF \
            -DENABLE_UTIL=OFF \
            -DENABLE_NET=OFF \
            -DENABLE_NETSSL=OFF \
            -DENABLE_CRYPTO=OFF \
            -DENABLE_DATA=ON \
            -DENABLE_DATA_SQLITE=ON \
            -DENABLE_DATA_MYSQL=OFF \
            -DENABLE_DATA_ODBC=OFF \
            -DENABLE_SEVENZIP=OFF \
            -DENABLE_ZIP=OFF \
            -DENABLE_PAGECOMPILER=OFF \
            -DENABLE_PAGECOMPILER_FILE2PAGE=OFF \
            -DENABLE_TESTS:BOOL=OFF .. \
	&& make --quiet -j $NUM_CPU \
    && make --quiet install/strip \
    # && make clean \
    && cd $ROOTDIR/src

# build user-config.jam files
RUN echo "using python : 3.6 : /usr ;" > ~/user-config.jam

RUN wget https://dl.bintray.com/boostorg/release/${BOOST_VERSION_DOT}/source/boost_${BOOST_VERSION}.tar.bz2 \
    && tar --bzip2 -xf boost_${BOOST_VERSION}.tar.bz2 && cd boost_${BOOST_VERSION}  \
    && ./bootstrap.sh --prefix=$ROOTDIR \
    && ./b2 -d0 -j $NUM_CPU cxxstd=14 install variant=release link=shared  \
    # && ./b2 clean \
    && cd $ROOTDIR/src

RUN wget https://github.com/fmtlib/fmt/archive/${FMT_VERSION}.tar.gz \
    && mkdir libfmt-${FMT_VERSION} && tar -xzf ${FMT_VERSION}.tar.gz -C libfmt-${FMT_VERSION} --strip-components=1 &&  cd libfmt-${FMT_VERSION} \
    && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
            -DFMT_DOC=OFF \
            -DFMT_TEST=OFF . \
    && make --quiet -j $NUM_CPU install/strip \
    # && make clean \
    && cd $ROOTDIR/src

RUN	git clone --recursive https://github.com/sebastiandev/zipper.git \
	&& cd zipper && git checkout e9f150516cb55d194b5e01d21a9527783e98311d && mkdir build  && cd build \
	&& cmake .. \
	&& make --quiet -j $NUM_CPU \
    && make --quiet install \
    && make clean \
    && cd $ROOTDIR/src

RUN wget http://download.osgeo.org/gdal/${GDAL_VERSION}/gdal-${GDAL_VERSION}.tar.gz \
    && tar -xvf gdal-${GDAL_VERSION}.tar.gz && cd gdal-${GDAL_VERSION} \
    && ./configure  --without-libtool --with-hide-internal-symbols \
        --with-python --with-spatialite --with-pg --with-curl --prefix=$ROOTDIR \
        --with-libtiff=internal --with-rename-internal-libtiff-symbols \
        --with-geotiff=internal --with-rename-internal-libgeotiff-symbols \
    && make --quiet -j $NUM_CPU \
    && make install \
    # && make clean \
    && cd $ROOTDIR/src
RUN strip -s $ROOTDIR/lib/libgdal.so
RUN for i in $ROOTDIR/lib/python3/dist-packages/osgeo/*.so; do strip -s $i 2>/dev/null || /bin/true; done
RUN strip -s $ROOTDIR/bin/gdal_contour && strip -s $ROOTDIR/bin/gdal_grid \
    && strip -s $ROOTDIR/bin/gdal_rasterize && strip -s $ROOTDIR/bin/gdal_translate && strip -s $ROOTDIR/bin/gdaladdo \
    && strip -s $ROOTDIR/bin/gdalbuildvrt && strip -s $ROOTDIR/bin/gdaldem && strip -s $ROOTDIR/bin/gdalenhance \
    && strip -s $ROOTDIR/bin/gdalinfo && strip -s $ROOTDIR/bin/gdallocationinfo && strip -s $ROOTDIR/bin/gdalmanage \
    && strip -s $ROOTDIR/bin/gdalserver && strip -s $ROOTDIR/bin/gdalsrsinfo && strip -s $ROOTDIR/bin/gdaltindex \
    && strip -s $ROOTDIR/bin/gdaltransform && strip -s $ROOTDIR/bin/gdalwarp && strip -s $ROOTDIR/bin/gnmanalyse \
    && strip -s $ROOTDIR/bin/gnmmanage && strip -s $ROOTDIR/bin/nearblack && strip -s $ROOTDIR/bin/ogr2ogr \
    && strip -s $ROOTDIR/bin/ogrinfo && strip -s $ROOTDIR/bin/ogrlineref && strip -s $ROOTDIR/bin/ogrtindex \
    && strip -s $ROOTDIR/bin/testepsg

ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
ENV GDAL_DATA=/usr/local/share/gdal
ENV GDAL_HTTP_VERSION 2

RUN apt-get update -y \
    && apt-get remove -y --purge build-essential \
    && cd $ROOTDIR/src/gdal-${GDAL_VERSION}/swig/python \
    && python3 setup.py build \
    && python3 setup.py install

RUN    cd $ROOTDIR/src/gdal-${GDAL_VERSION}/swig/python \
    && python3 setup.py build \
    && python3 setup.py install

RUN ldconfig

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
