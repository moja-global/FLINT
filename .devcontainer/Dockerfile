# FROM mojaglobal/vscode-baseimage:bionic
FROM mulliongroup/vscode-baseimage_standalone_vcpkg:bionic

ENV DEBIAN_FRONTEND=noninteractive
ARG NUM_CPU=6

ENV ROOTDIR /usr/local
ENV PATH $ROOTDIR/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTDIR/lib:$ROOTDIR/src/vcpkg/installed/x64-linux/lib:$ROOTDIR/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
ENV PYTHONPATH $ROOTDIR/lib:$PYTHONPATH
ENV CURL_CA_BUNDLE /etc/ssl/certs/ca-certificates.crt
ENV GDAL_DATA=$ROOTDIR/share/gdal
ENV GDAL_HTTP_VERSION 2
ENV GDAL_HTTP_MERGE_CONSECUTIVE_RANGES YES
ENV GDAL_HTTP_MULTIPLEX YES

WORKDIR $ROOTDIR/src

RUN apt-get update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends apt-utils dialog 2>&1

RUN apt-get install -y gettext libzstd1-dev sqlformat


# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
