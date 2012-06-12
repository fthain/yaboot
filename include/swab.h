#ifndef _REISERFS_SWAB_H_
#define _REISERFS_SWAB_H_
/* Stolen from linux/include/linux/byteorder/swab.h */
static inline __u16 swab16(__u16 x)
{
	return (((x & (__u16)0x00ffU) << 8) |
		((x & (__u16)0xff00U) >> 8) );
}

static inline __u32 swab32(__u32 x)
{
	return (((x & (__u32)0x000000ffUL) << 24) |
		((x & (__u32)0x0000ff00UL) <<  8) |
		((x & (__u32)0x00ff0000UL) >>  8) |
		((x & (__u32)0xff000000UL) >> 24) );
}

static inline __u64 swab64(__u64 x)
{
	return ((__u64)((x & (__u64)0x00000000000000ffULL) << 56) |
		(__u64)((x & (__u64)0x000000000000ff00ULL) << 40) |
		(__u64)((x & (__u64)0x0000000000ff0000ULL) << 24) |
		(__u64)((x & (__u64)0x00000000ff000000ULL) <<  8) |
	        (__u64)((x & (__u64)0x000000ff00000000ULL) >>  8) |
		(__u64)((x & (__u64)0x0000ff0000000000ULL) >> 24) |
		(__u64)((x & (__u64)0x00ff000000000000ULL) >> 40) |
		(__u64)((x & (__u64)0xff00000000000000ULL) >> 56) );
}
#endif /* _REISERFS_SWAB_H_ */
