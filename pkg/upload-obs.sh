#!/bin/sh

set -e
PROJ_TOPDIR=$(cd $(dirname $0); pwd)/../

cleanup () {
	make distclean ||:
	$PROJ_TOPDIR/autogen.sh
	make dist
}

# sets:
#  OBS_DEFAULT_PACKAGE
#  OBS_DEFAULT_PROJECT
set_version_vars () {
	local PACKAGE
	local PACKAGE_VERSION
	local PACKAGE_STRING
	eval "$(make get-version)"
	OBS_DEFAULT_PACKAGE=$PACKAGE
	OBS_DEFAULT_PROJECT=home:$USER
}

function usage ()
{
    echo "usage: $0 [options]"
    echo "  -p <pkg>         package to use"
    echo "  -r <prj>         project to use"
    echo "  -c <cfg>         osc config to use"
    echo "  -n               do not do cleanup"
    exit 1
}

CLEANUP=1
while getopts "dnp:r:c:" Option
do
  case $Option in
      d)
	set -x
	;;
      p)
        OBS_PACKAGE=$OPTARG
        ;;
      r)
        OBS_PROJECT=$OPTARG
        ;;
      c)
        readlink -e $OPTARG || echo "Cannot find specified OSC Config: $OPTARG"
        OSC_CONFIG=$(readlink -e $OPTARG)
        ;;
      n)
        CLEANUP=0
        ;;
      *)
        usage
        ;;
  esac
done
shift $(($OPTIND - 1))
# Move argument pointer to next.

[ $CLEANUP -eq 0 ] || cleanup

set_version_vars

[ -n "$OBS_PACKAGE" ] || OBS_PACKAGE=$OBS_DEFAULT_PACKAGE
[ -n "$OBS_PROJECT" ] || OBS_PROJECT=$OBS_DEFAULT_PROJECT
[ -z "$OSC_CONFIG"  ] || OSC_CONFIG="-c $OSC_CONFIG"

osc $OSC_CONFIG co ${OBS_PROJECT} ${OBS_PACKAGE}

rm -f  ${OBS_PROJECT}/${OBS_PACKAGE}/*.tar.bz2
rm -f  ${OBS_PROJECT}/${OBS_PACKAGE}/*.spec

cp ${OBS_PACKAGE}*.tar.bz2 ${OBS_PROJECT}/${OBS_PACKAGE}
cp */${OBS_PACKAGE}.spec ${OBS_PROJECT}/${OBS_PACKAGE}

cd ${OBS_PROJECT}/${OBS_PACKAGE}
osc $OSC_CONFIG addremove
yes | osc $OSC_CONFIG updatepacmetafromspec
osc $OSC_CONFIG ci -m "scripted source update"


