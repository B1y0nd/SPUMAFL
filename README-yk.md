# 运行系统 
1.ubuntu:18.04
2.运行在docker容器里面root@b46637a806d0:/home/ubuntu/aflnet
(这里/home/ubuntu/aflnet不要改成其他，比如改成/home/young/aflnet，不可以)
# AFLNET环境
apt-get install clang
apt-get install graphviz-dev libcap-dev
cd aflnet
make clean all
cd llvm_mode
make
cd ../..
export AFLNET=$(pwd)/aflnet
export WORKDIR=$(pwd)
export PATH=$PATH:$AFLNET
export AFL_PATH=$AFLNET
这样就可以了。
# EQUAFL环境
apt-get update
apt-get install pkg-config
apt-get install -y vim
apt-get install -y zlib1g-dev
apt-get install -y libsdl1.2-dev
apt-get install -y autoconf automake libtool
apt-get install -y python
apt-get install -y zlib1g-dev
apt-get install -y libglib2.0-dev
apt-get install  gcc automake autoconf libtool make
apt-get install -y flex
apt-get install -y bison
cd aflnet/EQUAFL
./configure --target-list=mipsel-linux-user,mips-linux-user,arm-linux-user --static --disable-werror
make
# 运行(在aflnet目录下)
cd ..
cp EQUAFL/mips-linux-user/qemu-mips ./
mv qemu-mips afl-qemu-trace
最后运行如下指令即可：`afl-fuzz -d -i $AFLNET/in-rtsp -o out_test -m none -z $AFLNET/rrseed -N tcp://127.0.0.1/8554 -P RTSP -D 10000 -q 3 -s 3 -E -K -R -Q $AFLNET/socket_test/test_mips`
# 可能存在如下问题
1.libc.so.6: cannot open shared object file: No such file or directory
**解决方案**
find / -name "libc.so.6"
显示信息/usr/mips-linux-gnu/lib/libc.so.6
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/mips-linux-gnu/lib
路径就是上面找到的路径
source /etc/profile
使其生效。
要是还不行，就直接cp过去