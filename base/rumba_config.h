/* rumba_config.h.  Generated from config.h.top config.h.in config.h.bot by configure.  */
#ifndef CONFIG_H
#define CONFIG_H
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

#define RUMBA_PREFIX "/usr"

#define RUMBA_VERSION "1.9.28"

#define HAVE_IOS_BASE_OPENMODE 1

/* Define if you have the <sstream> header file.  */
#define HAVE_SSTREAM 1

/* C++ compiler supports template repository */
/* #undef HAVE_TEMPLATE_REPOSITORY */

/* Defines if your system has the crypt function */
/* #undef HAVE_CRYPT */

/* Define the type of the third argument for getsockname
 */
/* #undef ksize_t */

/* Define if the C++ compiler supports BOOL */
/* #undef HAVE_BOOL */

#ifndef _CPP_BACKWARD_BACKWARD_WARNING_H
# define _CPP_BACKWARD_BACKWARD_WARNING_H 1
#endif

#ifndef _BACKWARD_BACKWARD_WARNING_H
# define _BACKWARD_BACKWARD_WARNING_H 11
#endif
/* config.h.bot.  Generated automatically from configure.in by autoheader.  */

#define RUMBA_PREFIX "/usr"


#define RUMBA_SYS_CONFIG_FILE "/etc/rumbalib.conf"
// this usually needs the users home directory prepended to it.
#define RUMBA_USER_CONFIG_FILE "/.rumba/rumbalib.conf"

#define HAVE_IOS_BASE_OPENMODE 1
#define HAVE_STD_ITERATOR 1
/* #undef HAVE_STD_RANDOM_ACCESS_ITERATOR */

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <sstream> header file.  */
#define HAVE_SSTREAM 1

/* C++ compiler supports template repository */
/* #undef HAVE_TEMPLATE_REPOSITORY */

/* Defines if your system has the crypt function */
/* #undef HAVE_CRYPT */

/* Define the type of the third argument for getsockname
 */
/* #undef ksize_t */



/* Define if the C++ compiler supports BOOL */
/* #undef HAVE_BOOL */

#ifndef HAVE_IOS_BASE_OPENMODE
namespace std
{
	namespace ios_base
	{
		typedef int openmode;
	}
}
#endif
#endif
