# Building a release

## Artifacts

- Create the debian package

```shell
sh bin/build-pkgs.sh deb

ls build/nuzzle_*_amd64.deb
```

- Create the tarball

```shell
sh bin/build-pkgs.sh src

ls build/nuzzle-*.tar.gz
```

## Man page

To update the man page README.md

```shell
$ export MANWIDTH=80
$ man -P cat man/nuzzle.6
```
