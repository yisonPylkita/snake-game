#!/usr/bin/env bash

RESTORE=$(echo -en '\033[0m')
LRED=$(echo -en '\033[01;31m')
LGREEN=$(echo -en '\033[01;32m')
LBLUE=$(echo -en '\033[01;34m')
LYELLOW=$(echo -e '\e[93m')
HELP_INFO='
buildme.sh - Build snake-game application\n
Usage: buildme.sh [options]\n
where options can be one of following\n
  --help                    displays this message\n
  --install-dependencies    install system dependencies for this application\n
  --build-release           build application in release version\n
\n
When ran without parameters script will build application in debug version\n
'

function print_help
{
    echo -e $HELP_INFO
    return 0
}

function install_dependencies
{
    # option '--install-dependencies' requires root permissions
    (( "$EUID" == '0' )) || { echo ${LRED}'Script requires root permissions'${RESTORE}; return 1; }

    { echo ${LBLUE}'Installing g++'${RESTORE}; apt-get install -y g++; } &&
    { echo ${LBLUE}'Installing cmake'${RESTORE}; apt-get install -y cmake; } &&
    { echo ${LBLUE}'Installing ninja'${RESTORE}; apt-get install -y ninja-build; } &&
    { echo ${LBLUE}'Installing sfml'${RESTORE}; apt-get install -y libsfml-dev; } &&

    echo ${LGREEN}'Dependencies installation completed successfully'${RESTORE} ||
    { echo ${LRED}"Could not install one of dependencies. More info in $LOG_FILE"${RESTORE}; return 1; }
    return 0
}

function build_debug
{
    mkdir -p build-debug && cd build-debug && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug .. &&
    ninja && echo ${LGREEN}'Application build successful'${RESTORE} ||
    { echo ${LRED}'Application build failure'${RESTORE}; return 1; }
    return 0
}

function build_release
{
    mkdir -p build && cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Release .. &&
    ninja && echo ${LGREEN}'Application build successful'${RESTORE} ||
    { echo ${LRED}'Application build failure'${RESTORE}; return 1; }
    return 0
}

case "$1" in
'--help')
    print_help
    exit $?
    ;;
'--install-dependencies')
    install_dependencies
    exit $?
    ;;
'--build-release')
    build_release
    exit $?
    ;;
'')
    build_debug
    exit $?
    ;;
*)
    echo ${LRED}'Invalid param'${RESTORE};
    print_help
    exit 1
    ;;
esac
