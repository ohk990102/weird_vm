#! /bin/sh
set -e
[ `whoami` = root ] || { sudo "$0" "$@"; exit $?; }
if [ ! -d /chall ]; then
    mv bin /chall
fi
if [ ! -f /tmp/flag ]; then
    mv flag /tmp/flag
fi

apt update
apt -y upgrade && apt -y dist-upgrade
apt install -y git make pkg-config bison flex libprotobuf-dev libprotoc-dev protobuf-compiler
mkdir /tmp/nsjail
cd /tmp/nsjail
git clone https://github.com/google/nsjail.git && cd nsjail && git checkout 2.8
make
mv nsjail /usr/bin/nsjail
/usr/bin/nsjail -Ml -R /tmp/flag:/flag -T /tmp --chroot / --keep_caps --cwd /chall --port 31337 -t 30 -- ./server