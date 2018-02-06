FROM centos:7

VOLUME /output

ENV OUTPUT_DIR /build/dist
ENV BUILD_DIR /build

RUN yum install -y epel-release
RUN yum --enablerepo=epel install -y python34-devel
RUN yum install -y \
  rpm-build gettext-devel libxml2-devel xz libtool git gcc-c++ doxygen make help2man \
  && yum clean all

WORKDIR $BUILD_DIR
COPY . .

CMD ["./pkg/dockerfiles/centos7-entrypoint.sh"]
