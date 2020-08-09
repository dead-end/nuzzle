################################################################################
# File: deb-test.debian.dockerfile
#
# Build the image:
#
#   cp build/nuzzle_*_amd64.deb docker/tmp/
#
#   sudo docker build -t nuzzle_debian -f docker/deb-test.debian.dockerfile docker/
#
# Run test:
#
#   docker run -it nuzzle_debian 
################################################################################

#
# It wroks the same for ubuntu (as expected)
#
FROM debian

#
# The nuzzle version
#
ARG VERSION=0.3.1

#
# Add color support for the terminal
#
ENV TERM=xterm-256color

#
# A UTF8 locale is neccessary to get the transparent character: L'\u2592'
#
ENV LANG=C.UTF-8

#
# Set path to make starting nuzzle earsier.
#
ENV PATH=$PATH:/usr/games

#
# Copy the .deb package to the image
#
COPY tmp/nuzzle_${VERSION}_amd64.deb /tmp

#
# Install the .deb package
#
RUN apt-get update && \
	apt-get install -y bash && \
	apt-get install -y man && \
	rm -f /etc/dpkg/dpkg.cfg.d/excludes && \
	apt-get install -y /tmp/nuzzle_${VERSION}_amd64.deb && \
	apt-get install -y procps
