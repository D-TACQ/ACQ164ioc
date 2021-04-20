# EPICS base and support

Sample top level build for an EPICS base + asyn, StreamDevice project

**NB: hard coded path**: /home/pgm/PROJECTS .. search everything and replace with your own.. 

Do this BEFORE any build..

```bash
find . -type f -print | xargs grep /home/pgm/PROJECTS
```

## Following this install guide .. 
https://docs.epics-controls.org/projects/how-tos/en/latest/getting-started/installation.html

Submodules reference only
```
mkdir support
git submodule add -b master https://github.com/D-TACQ/EPICS-seq.git support/seq
git submodule add -b master https://github.com/D-TACQ/StreamDevice.git support/stream
```

```bash
git clone --recursive https://github.com/epics-base/epics-base.git
(cd epics-base; make

mkdir support
(
(cd seq; make)

#git clone github.com:D-TACQ/asyn.git
#(cd asyn; git checkout -n ACQ164 origin/ACQ164; make)

git clone https://github.com/D-TACQ/StreamDevice.git 
ln -s StreamDevice stream
(cd stream; make)

git clone https://github.com/epics-modules/asyn.git
(cd asyn; make)

sudo dnf install rpcgen
sudo dnf install libtirpc-devel

git clone https://github.com/ISISComputingGroup/EPICS-pcre.git
ln -s EPICS-pcre/8-32 pcre
)

source ./setup.env 
(cd support/pcre/; make)
```

 Good Job Freddie! Thanks!





