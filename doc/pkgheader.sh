#!/bin/sh

# BIOS update BASH script

_NAME_VER="Dell Libsmbios-based BIOS Update installer v1.0"
_COPYRIGHT="Copyright 2008 Dell Inc. All Rights Reserved."
_BNAME=`basename $0`
_PRG=dellBiosUpdate
_DEPS="mktemp tail tar awk rm"
_DIR=$(cd $(dirname $0); pwd)
_THIS_BIN=$_DIR/$( basename $0 )

set -e

#Functions

ShowHelp()
{
        echo "Usage: $_BNAME [options]"
        echo
        echo $_NAME_VER
        echo $_COPYRIGHT
        echo
        echo "Options:"
        echo "  --help                  Print this text."
        echo "  --version               Print package versions."
        echo "  --update                Update the bios."
        echo "  --extract PATH          Extract the package to PATH"
        echo
}

Extract()
{
    END=$(awk '/^__ARC__/ { print NR + 1; exit 0; }' $_THIS_BIN )
    tail -n +$END $_THIS_BIN | tar xzf - -C $tmpdir
    [ $? = 0 ] || \
    {
        echo "$0: The archive cannot be extracted."
        exit 1
    }
}

Update()
{
    echo 
    echo "Loading OS-provided 'dell_rbu' driver."
    if ! /sbin/modprobe dell_rbu; then
        echo "Could not load OS dell_rbu driver."
        exit 1
    fi
    echo
    pushd $tmpdir >/dev/null 2>&1
    echo "Running BIOS Update:"
    if ! ./$_PRG -u -f ./bios.hdr "$@"; then
        echo
        echo "BIOS Update Failed."
        echo
    else
        echo "You must now reboot your system!"
        echo
    fi
    popd >/dev/null 2>&1
}

checkroot()
{
    if [ $(id -u) -ne 0 ]; then
        echo "Cannot run update as non-root user."
        exit 1
    fi
}

#Main()

#Handle signals
tmpdir=
trap 'rm -rf $tmpdir; exit 99' HUP INT QUIT EXIT BUS SEGV PIPE TERM #1 2 3 10 11 13 15

#Ensure dependencies are available
type $_DEPS >/dev/null
[ $? = 0 ] || \
{
        echo "$0: Cannot find utilities on the system to execute package." >&2
        echo "Make sure the following utilities are in the path: $DEPS" >&2
        exit 1
}

#Check options
while [ $# -gt 0 ];
do
    case $1 in
        --debug)
                set -x
                shift
                ;;
        --version)
                echo $_NAME_VER
                echo $_COPYRIGHT
                tmpdir=$(mktemp -d /tmp/biosupdate-XXXXXX)
                export LD_LIBRARY_PATH=$tmpdir
                Extract 
                $tmpdir/$_PRG --version
                exit 0 
                ;;
        --help)
                ShowHelp
                exit 0
                ;;

        --extract)
                if [ -n "$2" ]; then
                    mkdir -p $2
                    tmpdir=$2
                else
                    tmpdir=$(mktemp -d ./UpdatePackage-XXXXXX)
                fi
                trap - HUP INT QUIT EXIT BUS SEGV PIPE TERM #1 2 3 10 11 13 15
                Extract
                exit 0
                ;;

        --update)
                checkroot
                shift
                tmpdir=$(mktemp -d /tmp/biosupdate-XXXXXX)
                export LD_LIBRARY_PATH=$tmpdir
                Extract 
                Update "$@"
                exit 0
                ;;

        '')
                ShowHelp
                exit 0
                ;;

        *) 
                echo "invalid command line option: $1"
                ShowHelp
                exit 0
                ;;
    esac
done

ShowHelp
exit 0
__ARC__
