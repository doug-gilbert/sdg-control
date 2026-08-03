#ifndef CONFIG_H
#define CONFIG_H

// This file (config.h.in.cmake) is used as a template to generate the
// config.h file in the first step of a cmake build. This file should be
// kept under source control (e.g. svn or git) while config.h can be
// deleted after a successful build. After a failed build config.h
// should be examined.

#cmakedefine OS_LINUX  1
#cmakedefine CONTAINS_LINUX 1 
#cmakedefine OS_ANDROID  1
#cmakedefine OS_FREEBSD  1
#cmakedefine CONTAINS_BSD  1
#cmakedefine OS_NETBSD  1
#cmakedefine OS_OPENBSD  1
#cmakedefine OS_SOLARIS 1 
#cmakedefine OS_AIX 1
#cmakedefine OS_HAIKU 1
#cmakedefine OS_WIN32 1 
#cmakedefine CONTAINS_WIN32  1
#cmakedefine ENV_MINGW  1
#cmakedefine ENV_CYGWIN  1

#cmakedefine HAVE_GETOPT_H 1
#cmakedefine HAVE_GETOPT_LONG 1
#cmakedefine HAVE_SRAND48_R 1
#cmakedefine HAVE_STDATOMIC_H 1
#cmakedefine HAVE_PTHREAD_CANCEL 1
#cmakedefine HAVE_PTHREAD_KILL 1
#cmakedefine NEED_GETOPT_H 1
#cmakedefine NEED_GETOPT_LONG 1
#cmakedefine HAVE_SETMODE 1
#cmakedefine HAVE__SETMODE 1

#define BUILD_TIME "@BUILD_TIME@"


#endif
