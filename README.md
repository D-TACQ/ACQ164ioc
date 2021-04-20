# EPICS base and support

Sample top level build for an EPICS base + asyn, StreamDevice project

**NB: hard coded path**: /home/pgm/PROJECTS .. search everything and replace with your own.. 

Do this BEFORE any build..

```bash
find . -type f -print | xargs grep /home/pgm/PROJECTS
```

## Following this install guide .. 
https://docs.epics-controls.org/projects/how-tos/en/latest/getting-started/installation.html
```
git submodule add --force https://github.com/epics-base/epics-base.git base
cd base; make
```

Submodules reference only submodule add --force : actually works
```

mkdir support
git submodule add -b master https://github.com/D-TACQ/EPICS-seq.git support/seq
git submodule add -b master https://github.com/D-TACQ/StreamDevice.git support/stream
git submodule add --force https://github.com/ISISComputingGroup/EPICS-pcre.git support/pcre
git submodule add --force https://github.com/epics-modules/asyn.git support/asyn
```

```bash
(cd base; make)

mkdir support
(
(cd seq; make)
(cd stream; make)
(cd asyn; make)

sudo dnf install rpcgen
sudo dnf install libtirpc-devel

)

source ./setup.env 
(cd support/pcre/; make)
```

 Good Job Freddie! Thanks!





