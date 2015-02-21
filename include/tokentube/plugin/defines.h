#ifndef __TT_PLUGIN_DEFINES_H__
#define __TT_PLUGIN_DEFINES_H__


#define TT_PLUGIN_REGISTER( instance, f_init, f_conf, f_fini ) \
	__attribute__ ((constructor)) \
	static void tt_plugin_ctor(void) { \
		memset( &instance, '\0', sizeof(instance) ); \
		instance.meta.version = TT_VERSION; \
		instance.meta.initialize = f_init; \
		instance.meta.configure = f_conf; \
		instance.meta.finalize = f_fini; \
		tt_plugin_register( &instance ); \
	}


#define	TT_PBA__INITRD		"initrd"
#define	TT_PBA__INITRAMFS	"initramfs"


#endif /* __TT_PLUGIN_DEFINES_H__ */

