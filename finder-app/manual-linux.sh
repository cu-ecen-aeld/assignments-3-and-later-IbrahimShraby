#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    # remove existing configuration
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    # configure architectureb to be virt
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    # build the kernel
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    # build modules
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
    # build device tree
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs
fi

echo "Adding the Image in outdir"
# add the vmlinux supported linux image to the output directory
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}/Image
echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
# create the filesystem hirerachy standard folders
# first create the rootfs directory that will contain the FHS
mkdir -p ${OUTDIR}/rootfs
cd ${OUTDIR}/rootfs
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
cd ${OUTDIR}/rootfs
PRG_INT=$( ${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter" )
SHRD_LIBS=$( ${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library" )


# TODO: Add library dependencies to rootfs
# 1. this will give the path of the bin directory
CROSS_COMPILER_DIR=$( dirname $( which ${CROSS_COMPILE}gcc ) )
# 2. this will give the path of the gnu directory
cd ${CROSS_COMPILER_DIR}/..

# 3. copy all the required program interpreters
cp $(find . -name ld-linux-aarch64.so.1) ${OUTDIR}/rootfs/lib/
# 4. copy all the required shard libraries
cp $(find . -name libm.so.6) ${OUTDIR}/rootfs/lib64/
cp $(find . -name libresolv.so.2) ${OUTDIR}/rootfs/lib64/
cp $(find . -name libc.so.6) ${OUTDIR}/rootfs/lib64/


# TODO: Make device nodes
cd ${OUTDIR}/rootfs
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
cd ${FINDER_APP_DIR}
# 1. clean the writer utility
make CROSS_COMPILE=${CROSS_COMPILE} clean
# 2. build the writer utility
make CROSS_COMPILE=${CROSS_COMPILE} default

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp writer finder.sh finder-test.sh ${OUTDIR}/rootfs/home
cp -r conf ${OUTDIR}/rootfs/home
cp autorun-qemu.sh ${OUTDIR}/rootfs/home

# TODO: Chown the root directory
cd ${OUTDIR}/rootfs
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
# 1. create an initramfs format for the rootfs folder
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
# 2. create a zipped version for the rootfs in the ${OUTDIR} directory
cd ${OUTDIR}
gzip -f initramfs.cpio







