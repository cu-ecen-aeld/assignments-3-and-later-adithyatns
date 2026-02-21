#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
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
    # deep clean the kernel build tree
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/vmlinux ${OUTDIR}
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}


echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir -p ${OUTDIR}/rootfs
ROOTDIR=${OUTDIR}/rootfs

mkdir -p ${ROOTDIR}/bin  ${ROOTDIR}/dev  ${ROOTDIR}/etc  ${ROOTDIR}/home  ${ROOTDIR}/lib  ${ROOTDIR}/lib64  ${ROOTDIR}/proc  ${ROOTDIR}/sbin  ${ROOTDIR}/sys  ${ROOTDIR}/tmp  ${ROOTDIR}/usr  ${ROOTDIR}/var
mkdir -p  ${ROOTDIR}/usr/bin  ${ROOTDIR}/usr/lib  ${ROOTDIR}/usr/sbin
mkdir -p  ${ROOTDIR}/var/log
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
	make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${ROOTDIR} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
${CROSS_COMPILE}readelf -a ${ROOTDIR}/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a ${ROOTDIR}/bin/busybox | grep "Shared library"

SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)
# TODO: Add library dependencies to rootfs

cd $ROOTDIR
cp -a $SYSROOT/lib/ld-linux-aarch64.so.1 lib
cp -a $SYSROOT/lib64/libc.so.6 lib64
cp -a $SYSROOT/lib64/libm.so.6 lib64
cp -a $SYSROOT/lib64/libresolv.so.2 lib64

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
if [ -e $FINDER_APP_DIR/writer ]; then
	make -C $FINDER_APP_DIR clean
fi
make -C $FINDER_APP_DIR CROSS=${CROSS_COMPILE}gcc

# TODO: Copy the finder related scripts and executables to the /home directory
cp -a $FINDER_APP_DIR/writer home
cp -a $FINDER_APP_DIR/autorun-qemu.sh home
cp -a $FINDER_APP_DIR/finder.sh home
cp -a $FINDER_APP_DIR/finder-test.sh home
mkdir -p $ROOTDIR/home/conf
cp -r $FINDER_APP_DIR/conf/* home/conf

# on the target rootfs
cd "$OUTDIR/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio

# TODO: Chown the root directory
sudo chown -R root:root *
cd $OUTDIR
# TODO: Create initramfs.cpio.gz
gzip -f initramfs.cpio
