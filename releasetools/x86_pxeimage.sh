#!/usr/bin/env bash
set -e

#
# This script creates a bootable image and should at some point in the future
# be replaced by the proper NetBSD infrastructure.
#

: ${ARCH=i386}
: ${OBJ=../obj.${ARCH}}
: ${TOOLCHAIN_TRIPLET=i586-elf32-minix-}
: ${BUILDSH=build.sh}

: ${SETS="minix-base"}
: ${IMG=minix_x86_usb.img}

if [ ! -f ${BUILDSH} ]
then
	echo "Please invoke me from the root source dir, where ${BUILDSH} is."
	exit 1
fi

# set up disk creation environment
. releasetools/image.defaults
. releasetools/image.functions

: ${RAMDISK_SIZE=$(( 192*(2**20) / 512 / 8 ))}

# where the kernel & boot modules will be
MODDIR=${DESTDIR}/boot/minix/.temp

echo "Building work directory..."
build_workdir "$SETS"

echo "Adding extra files..."

# create a fstab entry in /etc
cat >${ROOT_DIR}/etc/fstab <<END_FSTAB
none		/sys		devman	rw,rslabel=devman	0	0
none		/dev/pts	ptyfs	rw,rslabel=ptyfs	0	0
END_FSTAB
add_file_spec "etc/fstab" extra.fstab
cp minix/drivers/storage/ramdisk/rc ${ROOT_DIR}/etc/rc.ramdisk
add_file_spec "etc/rc.ramdisk" extra.fstab

echo "Bundling packages..."
bundle_packages "$BUNDLE_PACKAGES"

echo "Creating specification files..."
create_input_spec
create_protos

echo "Writing ramdisk image..."
cp ${MODDIR}/* ${WORK_DIR}
create_ramdisk_image ${RAMDISK_SIZE}

echo "Preparing PXE boot directory..."
cp -r releasetools/3rdparty/syslinux/* ${WORK_DIR}/
rm -rf ${WORK_DIR}/extra.* ${WORK_DIR}/set.* ${WORK_DIR}/proto.* ${WORK_DIR}/imgrd.mfs ${WORK_DIR}/imgrd.o

echo "PXE boot directory at ${WORK_DIR}"
echo "To boot this image on kvm:"
echo "cd ${WORK_DIR} && qemu-system-i386 --enable-kvm -m 512M -tftp . -bootp pxelinux.0 -boot n"
