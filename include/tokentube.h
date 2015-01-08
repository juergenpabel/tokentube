#ifndef __TT_LIBRARY_TOKENTUBE_H__
#define __TT_LIBRARY_TOKENTUBE_H__


#include <tokentube/version.h>
#include <tokentube/defines.h>
#include <tokentube/enums.h>
#include <tokentube/pba.h>
#include <tokentube/library.h>


/*[TT_OK|TT_ERR]*/ int tt_initialize(tt_version_t version);
/*[TT_OK|TT_ERR]*/ int tt_configure(const char* filename);
/*[TT_OK|TT_ERR]*/ int tt_discover(tt_library_t* library);
/*[TT_OK|TT_ERR]*/ int tt_finalize();


#endif /* __TT_LIBRARY_TOKENTUBE_H__ */

