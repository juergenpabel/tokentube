#ifndef __TT_LIBRARY_VERSION_H__
#define __TT_LIBRARY_VERSION_H__


#define TT_VERSION_MAJOR	1
#define TT_VERSION_MINOR	0
#define TT_VERSION_PATCH	0


typedef struct {
	unsigned char major;
	unsigned char minor;
	unsigned char patch;
} tt_version_t;


static inline tt_version_t tt_version_init() {
	tt_version_t result = {
		(unsigned char)TT_VERSION_MAJOR, (unsigned char)TT_VERSION_MINOR, (unsigned char)TT_VERSION_PATCH
	};
	return result;
}
#define TT_VERSION	tt_version_init()


#endif /* __TT_LIBRARY_VERSION_H__ */

