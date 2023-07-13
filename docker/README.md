# Debian package tests with docker

A simple way to test nuzzle and the debian nuzzle package is to use docker.

The first step is to create the debian package. The script requires `lintian`
to be installed.

```
$ sh bin/build-pkgs.sh deb
```

Copy the debian package to the `tmp` directory.

```
$ rm -f docker/tmp/nuzzle_0.3.*_amd64.deb
$ cp build/nuzzle_*_amd64.deb docker/tmp/
```

Build the docker image:

```
$ sudo docker build -t nuzzle_debian -f docker/deb-test.debian.dockerfile docker/
[sudo] password for dead-end:
Sending build context to Docker daemon  68.61kB
Step 1/7 : FROM debian
 ---> ee11c54e6bb7
Step 2/7 : ARG VERSION=0.3.1
 ---> Using cache
 ---> e2e324162370
Step 3/7 : ENV TERM=xterm-256color
 ---> Using cache
 ---> 933dd4c0fcfe
Step 4/7 : ENV LANG=C.UTF-8
 ---> Using cache
 ---> fd76b0483d6b
Step 5/7 : ENV PATH=$PATH:/usr/games
 ---> Using cache
 ---> 0166b7872a07
Step 6/7 : COPY tmp/nuzzle_${VERSION}_amd64.deb /tmp
 ---> Using cache
 ---> dda1b2d754d8
Step 7/7 : RUN apt-get update && 	apt-get install -y bash && 	apt-get install -y man && 	rm -f /etc/dpkg/dpkg.cfg.d/excludes && 	apt-get install -y /tmp/nuzzle_${VERSION}_amd64.deb && 	apt-get install -y procps
 ---> Using cache
 ---> 691596e40a47
Successfully built 691596e40a47
Successfully tagged nuzzle_debian:latest
```

Run docker an call nuzzle inside the container:

```
$ docker run -it nuzzle_debian
root@1e2fe6fbc8e5:/# nuzzle
```
