# Live555对比试验
## 一、AFLNET运行EQUAFL仿真下的交叉编译的Live555  
Ubuntu 18.04；  
运行在docker容器中root@b46637a806d0:/home/ubuntu/aflnet(一定用/home/ubutnu开始)；  
### 1.AFLNET环境  
apt-get install -y clang  
apt-get install -y graphviz-dev libcap-dev  
git clone https://github.com/B1y0nd/SPUMAFL.git aflnet  
cd aflnet  
make clean all  
cd llvm_mode  
export LLVM_CONFIG=/usr/bin/llvm-config-6.0  
make  
cd ../..  
export AFLNET=$(pwd)/aflnet  
export WORKDIR=$(pwd)  
export PATH=$PATH:$AFLNET  
export AFL_PATH=$AFLNET  
### 2.EQUAFL环境  
apt-get update  
apt-get install -y pkg-config  
apt-get install -y git  
apt-get install -y vim  
apt-get install -y zlib1g-dev  
apt-get install -y libsdl1.2-dev  
apt-get install -y libglib2.0-dev  
apt-get install -y gcc autoconf automake libtool make  
apt-get install -y python  
apt-get install -y flex  
apt-get install -y bison  
cd aflnet/EQUAFL  
./configure --target-list=mipsel-linux-user,mips-linux-user,arm-linux-user --static --disable-werror  
make  
cd ..  
cp EQUAFL/mips-linux-user/qemu-mips ./  
mv qemu-mips afl-qemu-trace  
### 3.Live555交叉编译环境  
apt-get install -y binutils-mips-linux-gnu gcc-mips-linux-gnu  
apt-get install -y g++-mips-linux-gnu  
vim ~/.bashrc  
export PATH=/usr/bin/mips-linux-gnu:$PATH  
source ~/.bashrc  
mips-linux-gnu-gcc --version  
(交叉编译一个文件指令如下mips-linux-gnu-gcc -o socket_mips socket.c)  
### 4.下面是正式交叉编译Live555项目:  
#### 1>下载  
cd aflnet/  
git clone https://github.com/rgaufman/live555.git  
cd live555  
git checkout ceeb4f4  
#### 2>进入$AFLNET/tutorials/live555/目录  
将ceeb4f4.pathch等ceeb4f4开头的文件文件内容的第一部分删掉。即第一个diff块删掉。  
#### 3>修改config.mips-linux-gnu-gcc文件  
cp config.armlinux config.mips-linux-gnu-gcc  
vim config.mips-linux-gnu-gcc  
修改后文件如下   
CROSS_COMPILE?=         `mips-linux-gnu-`  
COMPILE_OPTS =          $(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 `-DLOCALE_NOT_USED`  
C =                     c  
C_COMPILER =            $(CROSS_COMPILE)gcc  
C_FLAGS =               $(COMPILE_OPTS)  
CPP =                   cpp  
CPLUSPLUS_COMPILER =    $(CROSS_COMPILE)g++  
CPLUSPLUS_FLAGS =       $(COMPILE_OPTS) -Wall -DBSD=1  
OBJ =                   o  
LINK =                  $(CROSS_COMPILE)g++ -o  
LINK_OPTS =  
CONSOLE_LINK_OPTS =     $(LINK_OPTS)  
LIBRARY_LINK =          $(CROSS_COMPILE)ar cr  
LIBRARY_LINK_OPTS =     $(LINK_OPTS)  
LIB_SUFFIX =                    a  
LIBS_FOR_CONSOLE_APPLICATION =  
LIBS_FOR_GUI_APPLICATION =  
EXE =  
#### 4>打补丁并编译  
patch -p1 < $AFLNET/tutorials/live555/ceeb4f4.patch  
./genMakefiles mips-linux-gnu-gcc  
make clean all  
#### 5>运行  
cd testProgs  
cp $AFLNET/tutorials/live555/sample_media_sources/* ./  
cp /usr/mips-linux-gnu/lib/ld.so.1 /lib/  
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/mips-linux-gnu/lib  
source /etc/profile  
export AFL_CPU_BINDING="0,1"  指定绑定的CPU核心  
afl-fuzz -d -i $AFLNET/tutorials/live555/in-rtsp -o out-live555 -m none -t 2000 -N tcp://127.0.0.1/8554 -x $AFLNET/tutorials/live555/rtsp.dict -P RTSP -D 10000 -q 3 -s 3 -E -K -R -Q ./testOnDemandRTSPServer 8554  
【注意以下几点】  
1.docker system df -v用于查看docker占用的磁盘空间大小。  
2.ps aux | grep afl-fuzz; kill -9 pid 用于kill进程。  
3.sudo mount -t tmpfs -o size=3G tmpfs /home/young/Live555_expr/aflnet1  
docker run -it --env USER=root --privileged --name AFLNET1 -v /home/young/Live555_expr/aflnet1/aflnet:/home/ubuntu/aflnet b1y0nd/young:Live555_aflnet /bin/bash  
4.保证$AFLNET/tutorials/live555/in-rtsp中的种子文件和$AFLNET/tutorials/live555/sample_media_sources中的文件对应且只有那三个，目前只能处理音频文件。  
## 二、AFLNET运行正常编译的Live555  
### 1.AFLNET环境与(一)中一致。  
git clone https://github.com/aflnet/aflnet.git aflnet  
git checkout 213c9cf  
### 2.Live555编译环境  
cd aflnet/  
git clone https://github.com/rgaufman/live555.git  
cd live555  
git checkout ceeb4f4  
patch -p1 < $AFLNET/tutorials/live555/ceeb4f4.patch  
需要修改种子文件，与我们的实验初始保持一致。  
./genMakefiles linux  
make clean all  
cd testProgs  
cp $AFLNET/tutorials/live555/sample_media_sources/* ./  
export AFL_CPU_BINDING="0,1"  指定绑定的CPU核心  
afl-fuzz -d -i $AFLNET/tutorials/live555/in-rtsp -o out-live555 -m none -t 2000 -N tcp://127.0.0.1/8554 -x $AFLNET/tutorials/live555/rtsp.dict -P RTSP -D 10000 -q 3 -s 3 -E -K -R ./testOnDemandRTSPServer 8554  
## 三、AFL运行EQUAFL仿真下的交叉编译的Live555  
### 1.AFL环境  
git clone https://github.com/google/AFL.git afl   
cd afl  
git checkout v2.56b  
make clean all  
cd llvm_mode  
export LLVM_CONFIG=/usr/bin/llvm-config-6.0  
make  
cd ../..  
export AFL=$(pwd)/afl  
export WORKDIR=$(pwd)  
export PATH=$PATH:$AFL  
export AFL_PATH=$AFL  
### 2.EQUAFL环境与(一)中一致。  
### 3.Live555交叉编译环境与(一)中一致。  
### 4.运行    
cd $AFL/live555/  
cp $AFL/EQUAFL/protocol/rtsp/ceeb4f4.patch ./ 修改方法与(一)中一致  
cd live555/testProgs  
cp $AFL/EQUAFL/protocol/rtsp/sample_media_sources/* ./  
export AFL_CPU_BINDING="0,1"  指定绑定的CPU核心  
afl-fuzz -d -i $AFL/EQUAFL/protocol/rtsp/in-rtsp -o outputs -m none -t 2000 -x $AFL/EQUAFL/protocol/rtsp/rtsp.dict -Q ./testOnDemandRTSPServer 8554 @@  
# Pureftpd对比实验  
## 一、AFLNET运行EQUAFL仿真下的交叉编译的Pureftpd  
### 1.添加ubuntu用户  
groupadd ubuntu && useradd -rm -d /home/ubuntu -s /bin/bash -g ubuntu -G sudo -u 1000 ubuntu -p "$(openssl passwd -1 ubuntu)"  
### 2.AFLNET环境  
与前面相同  
### 3.EQUAFL环境  
与前面相同  
### 4.交叉编译环境  
与前面相同  
### 5.下面是正式交叉编译Pureftpd项目:  
cd aflnet/  
git clone https://github.com/jedisct1/pure-ftpd.git  
cd pure-ftpd  
git checkout c21b45f  
打补丁ftp_parser.c文件和ftpd.c文件  
patch -p1 < $AFLNET/tutorials/pureftpd/fuzzing.patch  
./autogen.sh  
./configure --host=mips-linux-gnu --without-privsep -without-capabilities  
CC=mips-linux-gnu-gcc make clean all  
### 6.添加fuzzing用户  
useradd -rm -d /home/fuzzing -s /bin/bash -g ubuntu -G sudo -u 1001 fuzzing -p "$(openssl passwd -1 fuzzing)"  
cd /home/fuzzing  
mkdir -p aflnet/rrseed  
### 7.运行  
cd $AFLNET/pure-ftpd  
cp /usr/mips-linux-gnu/lib/ld.so.1 /lib/  
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/mips-linux-gnu/lib  
source /etc/profile  
export AFL_CPU_BINDING="0,1" 指定绑定的CPU核心  
afl-fuzz -d -i $AFLNET/tutorials/pureftpd/in-ftp -o out-pureftpd -m none -t 2000 -N tcp://127.0.0.1/21 -x $AFLNET/tutorials/pureftpd/ftp.dict -P FTP -D 10000 -c $AFLNET/tutorials/pureftpd/clean.sh -q 3 -s 3 -E -K -Q src/pure-ftpd -A  
【注意以下几点】  
1.在运行前记得修改USER_config文件。  
2.在运行前记得修改/etc/nsswitch.conf文件。  
3.在补丁文件中需要加入注释掉ftp_parser.c文件中的randomdelay();    
if (guest == 0) {  
     // randomdelay();  
}  
4.在补丁文件中需要加入注释掉ftpd.c文件中的randomsleep(tapping);两处   
5.在补丁文件中需要加入注释掉ftp_parser.c文件中的定时器alarm(idletime * 2);
## 二、AFLNET运行正常编译的Pureftpd  
### 1.添加用户  
groupadd ubuntu && useradd -rm -d /home/ubuntu -s /bin/bash -g ubuntu -G sudo -u 1000 ubuntu -p "$(openssl passwd -1 ubuntu)"  
useradd -rm -d /home/fuzzing -s /bin/bash -g ubuntu -G sudo -u 1001 fuzzing -p "$(openssl passwd -1 fuzzing)"  
### 2.AFLNET环境  
与前面相同  
git clone https://github.com/aflnet/aflnet.git aflnet  
git checkout 213c9cf  
同时需要修改aflnet.c文件  
### 3.执行  
cd aflnet/  
git clone https://github.com/jedisct1/pure-ftpd.git  
cd pure-ftpd  
git checkout c21b45f  
打补丁ftp_parser.c文件和ftpd.c文件   
patch -p1 < $AFLNET/tutorials/pureftpd/fuzzing.patch  
./autogen.sh  
CC="afl-clang-fast" CXX="afl-clang-fast++" CFLAGS="-fsanitize=address -g -O0" CXXFLAGS="-fsanitize=address -g -O0" LDFLAGS="-fsanitize=address" ./configure --without-privsep -without-capabilities   
make clean all  
export AFL_CPU_BINDING="0,1" 指定绑定的CPU核心  
afl-fuzz -d -i $AFLNET/tutorials/pureftpd/in-ftp -o out-pureftpd -m none -t 2000 -N tcp://127.0.0.1/21 -x $AFLNET/tutorials/pureftpd/ftp.dict -P FTP -D 10000 -c $AFLNET/tutorials/pureftpd/clean.sh -q 3 -s 3 -E -K src/pure-ftpd -A  
## 三、AFL运行EQUAFL仿真下的交叉编译的Pureftpd  
### 1.添加用户  
groupadd ubuntu && useradd -rm -d /home/ubuntu -s /bin/bash -g ubuntu -G sudo -u 1000 ubuntu -p "$(openssl passwd -1 ubuntu)"  
useradd -rm -d /home/fuzzing -s /bin/bash -g ubuntu -G sudo -u 1001 fuzzing -p "$(openssl passwd -1 fuzzing)"  
### 2.AFL环境  
git clone https://github.com/google/AFL.git afl  
cd afl  
git checkout v2.56b  
make clean all  
cd llvm_mode  
export LLVM_CONFIG=/usr/bin/llvm-config-6.0  
make  
cd ../..  
export AFL=$(pwd)/afl  
export WORKDIR=$(pwd)  
export PATH=$PATH:$AFL  
export AFL_PATH=$AFL  
### 3.EQUAFL环境   
cd afl/EQUAFL  
./configure --target-list=mipsel-linux-user,mips-linux-user,arm-linux-user --static --disable-werror  
make  
cd ..  
cp EQUAFL/mips-linux-user/qemu-mips ./  
mv qemu-mips afl-qemu-trace  
### 4.交叉编译环境  
与前面相同  
### 5.执行  
cd $AFL  
git clone https://github.com/jedisct1/pure-ftpd.git  
cd pure-ftpd  
git checkout c21b45f  
打补丁ftp_parser.c文件和ftpd.c文件  
patch -p1 < $AFL/EQUAFL/protocol/ftp/fuzzing.patch  
./autogen.sh  
./configure --host=mips-linux-gnu --without-privsep -without-capabilities  
CC=mips-linux-gnu-gcc make clean all  
cp /usr/mips-linux-gnu/lib/ld.so.1 /lib/  
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/mips-linux-gnu/lib  
source /etc/profile  
export AFL_CPU_BINDING="0,1" 指定绑定的CPU核心  
afl-fuzz -d -i $AFL/EQUAFL/protocol/ftp/in-ftp -o outputs -m none -t 2000 -x $AFL/EQUAFL/protocol/ftp/ftp.dict -Q src/pure-ftpd -A @@  
# Mosquitto对比实验  
## AFLNET直接对其模糊测试  
1.aflnet运行mosquitto  
安装依赖  
apt-get install libssl-dev libwebsockets-dev uuid-dev docbook-xsl docbook xsltproc  
编译与安装  
git clone https://github.com/eclipse/mosquitto.git  
git checkout [2665705](v2.0.7)  
export AFL_USE_ASAN=1  
CFLAGS="-g -O0 -fsanitize=address -fno-omit-frame-pointer" LDFLAGS="-g -O0 -fsanitize=address -fno-omit-frame-pointer"  CC=afl-gcc make clean all WITH_TLS=no WITH_STATIC_LIBRARIES=yes WITH_DOCS=no WITH_CJSON=no  
模糊测试  
afl-fuzz -d -i $AFLNET/tutorials/mosquitto/in-mqtt -o ./out-mqtt -m none -N tcp://127.0.0.1/1883 -P MQTT -D 10000 -q 3 -s 3 -E -K -R ./src/mosquitto  
## SPUMAFL对其模糊测试  
2.spumafl运行mosquitto  
在mosquitto目录下修改配置文件config.mk，关闭如下功能(减少依赖文件)：WITH_TLS=no WITH_TLS_PSK:=no WITH_STATIC_LIBRARIES=yes WITH_DOCS=no WITH_CJSON=no WITH_EPOLL:=no  
make CC=mips-linux-gnu-gcc CXX=mips-linux-gnu-g++  
cp /usr/mips-linux-gnu/lib/ld.so.1 /lib/  
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/mips-linux-gnu/lib  
source /etc/profile  
afl-fuzz -d -i $AFLNET/tutorials/mosquitto/in-mqtt -o ./out-mqtt -m none -N tcp://127.0.0.1/1883 -P MQTT -D 10000 -q 3 -s 3 -E -K -R -Q ./src/mosquitto  
