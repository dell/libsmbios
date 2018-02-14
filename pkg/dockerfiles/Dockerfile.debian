FROM debian:testing

RUN echo "deb-src http://deb.debian.org/debian/ testing main" >> /etc/apt/sources.list
RUN apt-get update -qq && apt-get install -yq --no-install-recommends \
	build-essential \
	debhelper \
	devscripts \
	doxygen \
	fakeroot \
	git \
	graphviz \
	help2man \
	python \
	libxml2-dev \
	lsb-release \
	pkg-config \
	autoconf \
	automake \
	libtool \
	gettext \
	autopoint
RUN mkdir /build
WORKDIR /build
COPY . .
CMD ["./pkg/mk-rel-deb.sh"]
