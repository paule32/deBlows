#ifndef STDIO
#define STDIO

#include "k_defines.h"


#define __signed__ signed

typedef signed char 		s8;
typedef unsigned char 		u8;
typedef signed short 		s16;
typedef unsigned short 		u16;
typedef signed int 		s32;
typedef unsigned int 		u32;
typedef signed long long 	s64;
typedef unsigned long long 	u64;

typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef __signed__ char   __s8;
typedef __signed__ short __s16;
typedef __signed__ int   __s32;

typedef unsigned char   __u8;
typedef unsigned short __u16;
typedef unsigned int   __u32;
typedef unsigned long  __u64;

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;

typedef unsigned int	__kernel_size_t;
typedef __kernel_size_t		size_t;

typedef s32 acpi_native_int;
typedef u32 acpi_size;

typedef u32 acpi_io_address;
typedef u32 acpi_physical_address;

typedef char *va_list;

/* Storage alignment properties */
#define  _AUPBND                (sizeof (acpi_native_int) - 1)
#define  _ADNBND                (sizeof (acpi_native_int) - 1)

/* Variable argument list macro definitions */
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (ap = (va_list) NULL)
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))


# define CC_SET(c) "\n\tset" #c " %[_cc_" #c "]\n"
# define CC_OUT(c) [_cc_ ## c] "=qm"

#ifdef __ASSEMBLY__
#define _AC(X,Y)	X
#define _AT(T,X)	X
#else
#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))
#endif

#define _BITUL(x)	(_AC(1,UL) << (x))
#define _BITULL(x)	(_AC(1,ULL) << (x))

/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF_BIT	0 /* Carry Flag */
#define X86_EFLAGS_CF		_BITUL(X86_EFLAGS_CF_BIT)
#define X86_EFLAGS_FIXED_BIT	1 /* Bit 1 - always on */
#define X86_EFLAGS_FIXED	_BITUL(X86_EFLAGS_FIXED_BIT)
#define X86_EFLAGS_PF_BIT	2 /* Parity Flag */
#define X86_EFLAGS_PF		_BITUL(X86_EFLAGS_PF_BIT)
#define X86_EFLAGS_AF_BIT	4 /* Auxiliary carry Flag */
#define X86_EFLAGS_AF		_BITUL(X86_EFLAGS_AF_BIT)
#define X86_EFLAGS_ZF_BIT	6 /* Zero Flag */
#define X86_EFLAGS_ZF		_BITUL(X86_EFLAGS_ZF_BIT)


/* ram_size flags */
#define RAMDISK_IMAGE_START_MASK	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000

/* loadflags */
#define LOADED_HIGH	(1<<0)
#define KASLR_FLAG	(1<<1)
#define QUIET_FLAG	(1<<5)
#define KEEP_SEGMENTS	(1<<6)
#define CAN_USE_HEAP	(1<<7)

/* xloadflags */
#define XLF_KERNEL_64			(1<<0)
#define XLF_CAN_BE_LOADED_ABOVE_4G	(1<<1)
#define XLF_EFI_HANDOVER_32		(1<<2)
#define XLF_EFI_HANDOVER_64		(1<<3)
#define XLF_EFI_KEXEC			(1<<4)

#define E820MAP	0x2d0		/* our map */
#define E820MAX	128			/* number of entries in E820MAP */
#define E820NR	0x1e8		/* # entries in E820MAP */

#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3
#define E820_NVS	4
#define E820_UNUSABLE	5
#define E820_PMEM	7

#define EDDNR				0x1e9		/* addr of number of edd_info structs at EDDBUF in boot_params - treat this as 1 byte  */

#define EDDBUF  			0xd00		/* addr of edd_info structs in boot_params */
#define EDDMAXNR			6		/* number of edd_info structs starting at EDDBUF  */
#define EDDEXTSIZE 			8		/* change these if you muck with the structures */
#define EDDPARMSIZE			74
#define CHECKEXTENSIONSPRESENT 		0x41
#define GETDEVICEPARAMETERS 		0x48
#define LEGACYGETDEVICEPARAMETERS 	0x08
#define EDDMAGIC1 0x55AA
#define EDDMAGIC2 0xAA55

#define E820_X_MAX E820MAX


#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & _AC(0xff000000,ULL)) << (56-24)) |	\
	 (((flags) & _AC(0x0000f0ff,ULL)) << 40) |	\
	 (((limit) & _AC(0x000f0000,ULL)) << (48-16)) |	\
	 (((base)  & _AC(0x00ffffff,ULL)) << 16) |	\
	 (((limit) & _AC(0x0000ffff,ULL))))

#define GDT_ENTRY_BOOT_CS	2
#define GDT_ENTRY_BOOT_DS	3
#define GDT_ENTRY_BOOT_TSS	4
#define __BOOT_CS		(GDT_ENTRY_BOOT_CS*8)
#define __BOOT_DS		(GDT_ENTRY_BOOT_DS*8)
#define __BOOT_TSS		(GDT_ENTRY_BOOT_TSS*8)


#define READ_SECTORS 0x02         /* int13 AH=0x02 is READ_SECTORS command */
#define EDD_MBR_SIG_OFFSET 0x1B8  /* offset of signature in the MBR */
#define EDD_MBR_SIG_BUF    0x290  /* addr in boot params */
#define EDD_MBR_SIG_MAX 16        /* max number of signatures to store */
#define EDD_MBR_SIG_NR_BUF 0x1ea  /* addr of number of MBR signtaures at EDD_MBR_SIG_BUF
				     in boot_params - treat this as 1 byte  */

#define EDD_EXT_FIXED_DISK_ACCESS           (1 << 0)
#define EDD_EXT_DEVICE_LOCKING_AND_EJECTING (1 << 1)
#define EDD_EXT_ENHANCED_DISK_DRIVE_SUPPORT (1 << 2)
#define EDD_EXT_64BIT_EXTENSIONS            (1 << 3)

#define EDD_INFO_DMA_BOUNDARY_ERROR_TRANSPARENT (1 << 0)
#define EDD_INFO_GEOMETRY_VALID                (1 << 1)
#define EDD_INFO_REMOVABLE                     (1 << 2)
#define EDD_INFO_WRITE_VERIFY                  (1 << 3)
#define EDD_INFO_MEDIA_CHANGE_NOTIFICATION     (1 << 4)
#define EDD_INFO_LOCKABLE                      (1 << 5)
#define EDD_INFO_NO_MEDIA_PRESENT              (1 << 6)
#define EDD_INFO_USE_INT13_FN50                (1 << 7)

struct edd_device_params {
	__u16 length;
	__u16 info_flags;
	__u32 num_default_cylinders;
	__u32 num_default_heads;
	__u32 sectors_per_track;
	__u64 number_of_sectors;
	__u16 bytes_per_sector;
	__u32 dpte_ptr;		/* 0xFFFFFFFF for our purposes */
	__u16 key;		/* = 0xBEDD */
	__u8 device_path_info_length;	/* = 44 */
	__u8 reserved2;
	__u16 reserved3;
	__u8 host_bus_type[4];
	__u8 interface_type[8];
	union {
		struct {
			__u16 base_address;
			__u16 reserved1;
			__u32 reserved2;
		} __attribute__ ((packed)) isa;
		struct {
			__u8 bus;
			__u8 slot;
			__u8 function;
			__u8 channel;
			__u32 reserved;
		} __attribute__ ((packed)) pci;
		/* pcix is same as pci */
		struct {
			__u64 reserved;
		} __attribute__ ((packed)) ibnd;
		struct {
			__u64 reserved;
		} __attribute__ ((packed)) xprs;
		struct {
			__u64 reserved;
		} __attribute__ ((packed)) htpt;
		struct {
			__u64 reserved;
		} __attribute__ ((packed)) unknown;
	} interface_path;
	union {
		struct {
			__u8 device;
			__u8 reserved1;
			__u16 reserved2;
			__u32 reserved3;
			__u64 reserved4;
		} __attribute__ ((packed)) ata;
		struct {
			__u8 device;
			__u8 lun;
			__u8 reserved1;
			__u8 reserved2;
			__u32 reserved3;
			__u64 reserved4;
		} __attribute__ ((packed)) atapi;
		struct {
			__u16 id;
			__u64 lun;
			__u16 reserved1;
			__u32 reserved2;
		} __attribute__ ((packed)) scsi;
		struct {
			__u64 serial_number;
			__u64 reserved;
		} __attribute__ ((packed)) usb;
		struct {
			__u64 eui;
			__u64 reserved;
		} __attribute__ ((packed)) i1394;
		struct {
			__u64 wwid;
			__u64 lun;
		} __attribute__ ((packed)) fibre;
		struct {
			__u64 identity_tag;
			__u64 reserved;
		} __attribute__ ((packed)) i2o;
		struct {
			__u32 array_number;
			__u32 reserved1;
			__u64 reserved2;
		} __attribute__ ((packed)) raid;
		struct {
			__u8 device;
			__u8 reserved1;
			__u16 reserved2;
			__u32 reserved3;
			__u64 reserved4;
		} __attribute__ ((packed)) sata;
		struct {
			__u64 reserved1;
			__u64 reserved2;
		} __attribute__ ((packed)) unknown;
	} device_path;
	__u8 reserved4;
	__u8 checksum;
} __attribute__ ((packed));

struct edd_info {
	__u8 device;
	__u8 version;
	__u16 interface_support;
	__u16 legacy_max_cylinder;
	__u8 legacy_max_head;
	__u8 legacy_sectors_per_track;
	struct edd_device_params params;
} __attribute__ ((packed));

struct edd {
	unsigned int mbr_signature[EDD_MBR_SIG_MAX];
	struct edd_info edd_info[EDDMAXNR];
	unsigned char mbr_signature_nr;
	unsigned char edd_info_nr;
};



struct e820entry {
	__u64 addr;	/* start of memory segment */
	__u64 size;	/* size of memory segment */
	__u32 type;	/* type of memory segment */
} __attribute__((packed));
 
struct e820map {
	__u32 nr_map;
	struct e820entry map[E820_X_MAX];
};

typedef struct {
    u16 off, seg;
} far_ptr;


struct ist_info {
	__u32 signature;
	__u32 command;
	__u32 event;
	__u32 perf_level;
};


/* VESA General Information table */
struct vesa_general_info {
    u32 signature;		/* 0 Magic number = "VESA" */
    u16 version;		/* 4 */
    far_ptr vendor_string;	/* 6 */
    u32 capabilities;	/* 10 */
    far_ptr video_mode_ptr;	/* 14 */
    u16 total_memory;	/* 18 */

    u8 reserved[236];	/* 20 */
} __attribute__ ((packed));

#define VESA_MAGIC ('V' + ('E' << 8) + ('S' << 16) + ('A' << 24))

struct vesa_mode_info {
    u16 mode_attr;		/* 0 */
    u8 win_attr[2];		/* 2 */
    u16 win_grain;		/* 4 */
    u16 win_size;		/* 6 */
    u16 win_seg[2];		/* 8 */
    far_ptr win_scheme;	/* 12 */
    u16 logical_scan;	/* 16 */

    u16 h_res;		/* 18 */
    u16 v_res;		/* 20 */
    u8 char_width;		/* 22 */
    u8 char_height;		/* 23 */
    u8 memory_planes;	/* 24 */
    u8 bpp;			/* 25 */
    u8 banks;		/* 26 */
    u8 memory_layout;	/* 27 */
    u8 bank_size;		/* 28 */
    u8 image_planes;	/* 29 */
    u8 page_function;	/* 30 */

    u8 rmask;		/* 31 */
    u8 rpos;		/* 32 */
    u8 gmask;		/* 33 */
    u8 gpos;		/* 34 */
    u8 bmask;		/* 35 */
    u8 bpos;		/* 36 */
    u8 resv_mask;		/* 37 */
    u8 resv_pos;		/* 38 */
    u8 dcm_info;		/* 39 */

    u32 lfb_ptr;		/* 40 Linear frame buffer address */
    u32 offscreen_ptr;	/* 44 Offscreen memory address */
    u16 offscreen_size;	/* 48 */

    u8 reserved[206];	/* 50 */
} __attribute__ ((packed));


/* extensible setup data list node */
struct setup_data {
    __u64 next;
    __u32 type;
    __u32 len;
    __u8 data[0];
};

struct setup_header {
    __u8	setup_sects;
    __u16	root_flags;
    __u32	syssize;
    __u16	ram_size;
    __u16	vid_mode;
    __u16	root_dev;
    __u16	boot_flag;
    __u16	jump;
    __u32	header;
    __u16	version;
    __u32	realmode_swtch;
    __u16	start_sys;
    __u16	kernel_version;
    __u8	type_of_loader;
    __u8	loadflags;
    __u16	setup_move_size;
    __u32	code32_start;
    __u32	ramdisk_image;
    __u32	ramdisk_size;
    __u32	bootsect_kludge;
    __u16	heap_end_ptr;
    __u8	ext_loader_ver;
    __u8	ext_loader_type;
    __u32	cmd_line_ptr;
    __u32	initrd_addr_max;
    __u32	kernel_alignment;
    __u8	relocatable_kernel;
    __u8	min_alignment;
    __u16	xloadflags;
    __u32	cmdline_size;
    __u32	hardware_subarch;
    __u64	hardware_subarch_data;
    __u32	payload_offset;
    __u32	payload_length;
    __u64	setup_data;
    __u64	pref_address;
    __u32	init_size;
    __u32	handover_offset;
} __attribute__((packed));

struct sys_desc_table {
    __u16 length;
    __u8  table[14];
};

/* Gleaned from OFW's set-parameters in cpu/x86/pc/linux.fth */
struct olpc_ofw_header {
    __u32 ofw_magic;	/* OFW signature */
    __u32 ofw_version;
    __u32 cif_handler;	/* callback into OFW */
    __u32 irq_desc_table;
} __attribute__((packed));

struct efi_info {
    __u32 efi_loader_signature;
    __u32 efi_systab;
    __u32 efi_memdesc_size;
    __u32 efi_memdesc_version;
    __u32 efi_memmap;
    __u32 efi_memmap_size;
    __u32 efi_systab_hi;
    __u32 efi_memmap_hi;
};

struct edid_info {
	unsigned char dummy[128];
};

struct screen_info {
	__u8  orig_x;		/* 0x00 */
	__u8  orig_y;		/* 0x01 */
	__u16 ext_mem_k;	/* 0x02 */
	__u16 orig_video_page;	/* 0x04 */
	__u8  orig_video_mode;	/* 0x06 */
	__u8  orig_video_cols;	/* 0x07 */
	__u8  flags;		/* 0x08 */
	__u8  unused2;		/* 0x09 */
	__u16 orig_video_ega_bx;/* 0x0a */
	__u16 unused3;		/* 0x0c */
	__u8  orig_video_lines;	/* 0x0e */
	__u8  orig_video_isVGA;	/* 0x0f */
	__u16 orig_video_points;/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	__u16 lfb_width;	/* 0x12 */
	__u16 lfb_height;	/* 0x14 */
	__u16 lfb_depth;	/* 0x16 */
	__u32 lfb_base;		/* 0x18 */
	__u32 lfb_size;		/* 0x1c */
	__u16 cl_magic, cl_offset; /* 0x20 */
	__u16 lfb_linelength;	/* 0x24 */
	__u8  red_size;		/* 0x26 */
	__u8  red_pos;		/* 0x27 */
	__u8  green_size;	/* 0x28 */
	__u8  green_pos;	/* 0x29 */
	__u8  blue_size;	/* 0x2a */
	__u8  blue_pos;		/* 0x2b */
	__u8  rsvd_size;	/* 0x2c */
	__u8  rsvd_pos;		/* 0x2d */
	__u16 vesapm_seg;	/* 0x2e */
	__u16 vesapm_off;	/* 0x30 */
	__u16 pages;		/* 0x32 */
	__u16 vesa_attributes;	/* 0x34 */
	__u32 capabilities;     /* 0x36 */
	__u32 ext_lfb_base;	/* 0x3a */
	__u8  _reserved[2];	/* 0x3e */
} __attribute__((packed));

struct apm_bios_info {
	__u16	version;
	__u16	cseg;
	__u32	offset;
	__u16	cseg_16;
	__u16	dseg;
	__u16	flags;
	__u16	cseg_len;
	__u16	cseg_16_len;
	__u16	dseg_len;
};

/* The so-called "zeropage" */
struct boot_params {
    struct screen_info screen_info;			/* 0x000 */
    struct apm_bios_info apm_bios_info;		/* 0x040 */
    __u8  _pad2[4];					/* 0x054 */
    __u64  tboot_addr;				/* 0x058 */
    struct ist_info ist_info;			/* 0x060 */
    __u8  _pad3[16];				/* 0x070 */
    __u8  hd0_info[16];	/* obsolete! */		/* 0x080 */
    __u8  hd1_info[16];	/* obsolete! */		/* 0x090 */
    struct sys_desc_table sys_desc_table; /* obsolete! */	/* 0x0a0 */
    struct olpc_ofw_header olpc_ofw_header;		/* 0x0b0 */
    __u32 ext_ramdisk_image;			/* 0x0c0 */
    __u32 ext_ramdisk_size;				/* 0x0c4 */
    __u32 ext_cmd_line_ptr;				/* 0x0c8 */
    __u8  _pad4[116];				/* 0x0cc */
    struct edid_info edid_info;			/* 0x140 */
    struct efi_info efi_info;			/* 0x1c0 */
    __u32 alt_mem_k;				/* 0x1e0 */
    __u32 scratch;		/* Scratch field! */	/* 0x1e4 */
    __u8  e820_entries;				/* 0x1e8 */
    __u8  eddbuf_entries;				/* 0x1e9 */
    __u8  edd_mbr_sig_buf_entries;			/* 0x1ea */
    __u8  kbd_status;				/* 0x1eb */
    __u8  _pad5[3];					/* 0x1ec */
    /*
     * The sentinel is set to a nonzero value (0xff) in header.S.
     *
     * A bootloader is supposed to only take setup_header and put
     * it into a clean boot_params buffer. If it turns out that
     * it is clumsy or too generous with the buffer, it most
     * probably will pick up the sentinel variable too. The fact
     * that this variable then is still 0xff will let kernel
     * know that some variables in boot_params are invalid and
     * kernel should zero out certain portions of boot_params.
     */
    __u8  sentinel;					/* 0x1ef */
    __u8  _pad6[1];					/* 0x1f0 */
    struct setup_header hdr;    /* setup header */	/* 0x1f1 */
    __u8  _pad7[0x290-0x1f1-sizeof(struct setup_header)];
    __u32 edd_mbr_sig_buffer[EDD_MBR_SIG_MAX];	/* 0x290 */
    struct e820entry e820_map[E820MAX];		/* 0x2d0 */
    __u8  _pad8[48];				/* 0xcd0 */
    struct edd_info eddbuf[EDDMAXNR];		/* 0xd00 */
    __u8  _pad9[276];				/* 0xeec */
} __attribute__((packed));

struct boot_params boot_params;
extern struct screen_info *screen_info;


static inline void io_delay(void)
{
	const u16 DELAY_PORT = 0x80;
	asm volatile("outb %%al,%0" : : "dN" (DELAY_PORT));
}

/* These functions are used to reference data in other segments. */
static inline u16 ds(void)
{
	u16 seg;
	asm("movw %%ds,%0" : "=rm" (seg));
	return seg;
}
 
static inline void set_fs(u16 seg)
{
	asm volatile("movw %0,%%fs" : : "rm" (seg));
}
static inline u16 fs(void)
{
	u16 seg;
	asm volatile("movw %%fs,%0" : "=rm" (seg));
	return seg;
}

static inline void set_gs(u16 seg)
{
	asm volatile("movw %0,%%gs" : : "rm" (seg));
}
static inline u16 gs(void)
{
	u16 seg;
	asm volatile("movw %%gs,%0" : "=rm" (seg));
	return seg;
}

typedef unsigned int addr_t;
 
static inline u8 rdfs8(addr_t addr)
{
	u8 v;
	asm volatile("movb %%fs:%1,%0" : "=q" (v) : "m" (*(u8 *)addr));
	return v;
}
static inline u16 rdfs16(addr_t addr)
{
	u16 v;
	asm volatile("movw %%fs:%1,%0" : "=r" (v) : "m" (*(u16 *)addr));
	return v;
}
static inline u32 rdfs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%fs:%1,%0" : "=r" (v) : "m" (*(u32 *)addr));
	return v;
}
 
static inline void wrfs8(u8 v, addr_t addr)
{
	asm volatile("movb %1,%%fs:%0" : "+m" (*(u8 *)addr) : "qi" (v));
}
static inline void wrfs16(u16 v, addr_t addr)
{
	asm volatile("movw %1,%%fs:%0" : "+m" (*(u16 *)addr) : "ri" (v));
}
static inline void wrfs32(u32 v, addr_t addr)
{
	asm volatile("movl %1,%%fs:%0" : "+m" (*(u32 *)addr) : "ri" (v));
}
 
static inline u8 rdgs8(addr_t addr)
{
	u8 v;
	asm volatile("movb %%gs:%1,%0" : "=q" (v) : "m" (*(u8 *)addr));
	return v;
}
static inline u16 rdgs16(addr_t addr)
{
	u16 v;
	asm volatile("movw %%gs:%1,%0" : "=r" (v) : "m" (*(u16 *)addr));
	return v;
}
static inline u32 rdgs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%gs:%1,%0" : "=r" (v) : "m" (*(u32 *)addr));
	return v;
}
 
static inline void wrgs8(u8 v, addr_t addr)
{
	asm volatile("movb %1,%%gs:%0" : "+m" (*(u8 *)addr) : "qi" (v));
}
static inline void wrgs16(u16 v, addr_t addr)
{
	asm volatile("movw %1,%%gs:%0" : "+m" (*(u16 *)addr) : "ri" (v));
}
static inline void wrgs32(u32 v, addr_t addr)
{
	asm volatile("movl %1,%%gs:%0" : "+m" (*(u32 *)addr) : "ri" (v));
}

/* Note: these only return true/false, not a signed return value! */
static inline bool memcmp_fs(const void *s1, addr_t s2, size_t len)
{
	bool diff;
	asm volatile("fs; repe; cmpsb" CC_SET(nz)
		     : CC_OUT(nz) (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}
static inline bool memcmp_gs(const void *s1, addr_t s2, size_t len)
{
	bool diff;
	asm volatile("gs; repe; cmpsb" CC_SET(nz)
		     : CC_OUT(nz) (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

/* Heap -- available for dynamic lists. */
extern char _end[];
extern char *HEAP;
extern char *heap_end;
#define RESET_HEAP() ((void *)( HEAP = _end ))
static inline char *__get_heap(size_t s, size_t a, size_t n)
{
 	char *tmp;

 	HEAP = (char *)(((size_t)HEAP+(a-1)) & ~(a-1));
 	tmp = HEAP;
 	HEAP += s*n;
	return tmp;
}
#define GET_HEAP(type, n) \
	((type *)__get_heap(sizeof(type),__alignof__(type),(n)))

static inline bool heap_free(size_t n)
{
 	return (int)(heap_end-HEAP) >= (int)n;
}

struct biosregs {
 	union {
 		struct {
 			u32 edi;
 			u32 esi;
 			u32 ebp;
 			u32 _esp;
 			u32 ebx;
 			u32 edx;
 			u32 ecx;
 			u32 eax;
 			u32 _fsgs;
 			u32 _dses;
 			u32 eflags;
 		};
 		struct {
 			u16 di, hdi;
 			u16 si, hsi;
 			u16 bp, hbp;
 			u16 _sp, _hsp;
 			u16 bx, hbx;
 			u16 dx, hdx;
 			u16 cx, hcx;
 			u16 ax, hax;
 			u16 gs, fs;
 			u16 es, ds;
 			u16 flags, hflags;
 		};
 		struct {
 			u8 dil, dih, edi2, edi3;
 			u8 sil, sih, esi2, esi3;
 			u8 bpl, bph, ebp2, ebp3;
 			u8 _spl, _sph, _esp2, _esp3;
 			u8 bl, bh, ebx2, ebx3;
 			u8 dl, dh, edx2, edx3;
 			u8 cl, ch, ecx2, ecx3;
 			u8 al, ah, eax2, eax3;
		};
	};
};
void intcall(u8 int_no, const struct biosregs *ireg, struct biosregs *oregs);

extern void clear_screen(); //clears the screen, duh
extern void print(char *msg, unsigned int line);//prints a message to the screen at line specified
extern void puts(const char *str);

extern void *__memset(void *, int, __kernel_size_t);
extern void *__memcpy(void *, const void *, __kernel_size_t);
extern void *__memmove(void *, const void *, __kernel_size_t);
extern void *memset(void *, int, int);
extern void *memcpy(void *, const void *, __kernel_size_t);
extern void *memmove(void *, const void *, __kernel_size_t);




#endif // stdio.h
