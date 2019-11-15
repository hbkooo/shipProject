#!/bin/bash

#------------------------------------------------------------------------------
#
# - git clone --recursive
# - ./install_deps.sh
# - cmake && make
#


set -e

# Check for 'uname' and abort if it is not available.
uname -v > /dev/null 2>&1 || { echo >&2 "ERROR - verifier requires 'uname' to identify the platform."; exit 1; }

case $(uname -s) in

#------------------------------------------------------------------------------
# Linux
#------------------------------------------------------------------------------
Linux)

    # Detect if sudo is needed.
    if [ $(id -u) != 0 ]; then
        SUDO="sudo"
    fi

#------------------------------------------------------------------------------
# Arch Linux
#------------------------------------------------------------------------------

    if [ -f "/etc/arch-release" ]; then

        echo "Installing verifier dependencies on Arch Linux."

        # The majority of our dependencies can be found in the
        # Arch Linux official repositories.
        # See https://wiki.archlinux.org/index.php/Official_repositories
        $SUDO pacman -Sy --noconfirm \
            autoconf \
            automake \
            gcc \
            libtool \
            boost 

    elif [ -f "/etc/os-release" ]; then

        DISTRO_NAME=$(. /etc/os-release; echo $NAME)
        case $DISTRO_NAME in

        Debian*)
            echo "Installing verifier dependencies on Debian Linux."

            $SUDO apt-get -q update
            $SUDO apt-get -qy install \
                build-essential \
                libboost-all-dev  \
                libopencv-dev   

            ;;

        Fedora)
            echo "Installing verifier dependencies on Fedora Linux."
            $SUDO dnf -qy install \
                gcc-c++ \
                boost-devel 
            ;;

#------------------------------------------------------------------------------
# Ubuntu
#
# TODO - I wonder whether all of the Ubuntu-variants need some special
# treatment?
#
# TODO - We should also test this code on Ubuntu Server, Ubuntu Snappy Core
# and Ubuntu Phone.
#
# TODO - Our Ubuntu build is only working for amd64 and i386 processors.
# It would be good to add armel, armhf and arm64.
# See https://github.com/ethereum/webthree-umbrella/issues/228.
#------------------------------------------------------------------------------
        Ubuntu|LinuxMint)
            echo "Installing verifier dependencies on Ubuntu."
            if [ "$TRAVIS" ]; then
                # Setup prebuilt LLVM on Travis CI:
                $SUDO apt-get -qy remove llvm  # Remove confilicting package.
                echo "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.9 main" | \
                    $SUDO tee -a /etc/apt/sources.list > /dev/null
                LLVM_PACKAGES="llvm-3.9-dev libz-dev"
            fi
        
            # $SUDO add-apt-repository -y ppa:ubuntu-toolchain-r/test
            $SUDO apt-get -q update
            $SUDO apt-get install -qy --no-install-recommends --allow-unauthenticated \
                build-essential \
                $LLVM_PACKAGES \
                libopencv-dev  \
                redis-server redis-tools \
                libgflags-dev libgoogle-glog-dev liblmdb-dev \
                libprotobuf-dev libleveldb-dev libsnappy-dev \
                libopencv-dev libhdf5-serial-dev protobuf-compiler \
                libatlas-base-dev \
                libtool pkg-config autotools-dev autoconf automake \
                uuid-dev libpcre3-dev valgrind \
                libopenblas-dev \
                liblapack-dev \
                libcurl4-openssl-dev
                # libboost-all-dev \
            
            # UBUNTU_VERSION=$(. /etc/os-release; echo $VERSION_ID)
            # if [ $UBUNTU_VERSION == "14.04" ]; then
            #     $SUDO apt-get install -qy g++-4.9 gcc-4.9
            # fi
            ;;

        CentOS*)
            echo "Installing verifier dependencies on CentOS."
            # Enable EPEL repo that contains leveldb-devel
                        
            $SUDO yum -y -q install epel-release
            $SUDO yum -y -q install \
                make \
                gcc-c++ \
                boost-devel \
                opencv

            ;;

        *)
            echo "Unsupported Linux distribution: $DISTRO_NAME."
            exit 1
            ;;

        esac

    elif [ -f "/etc/alpine-release" ]; then
        # Alpine Linux
        echo "Installing verifier dependencies on Alpine Linux."
        $SUDO apk add --no-cache --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing/ \
            g++ \
            make \
            boost-dev 

    else

        case $(lsb_release -is) in

#------------------------------------------------------------------------------
# Other (unknown) Linux
# Major and medium distros which we are missing would include Mint, CentOS,
# RHEL, Raspbian, Cygwin, OpenWrt, gNewSense, Trisquel and SteamOS.
#------------------------------------------------------------------------------
        *)
            #other Linux
            echo "ERROR - Unsupported or unidentified Linux distro."
            exit 1
            ;;
        esac
    fi
    ;;

#------------------------------------------------------------------------------
# Other platform (not Linux, FreeBSD or macOS).
# Not sure what might end up here?
# Maybe OpenBSD, NetBSD, AIX, Solaris, HP-UX?
#------------------------------------------------------------------------------
*)
    #other
    echo "ERROR - Unsupported or unidentified operating system."
    ;;
esac
