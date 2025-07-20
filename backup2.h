
#include <stdint.h>

#define FXLEN          80       /* length of file index */
#define DUMNAME   4     /* dummy name length for FS_NAME */
#define SIZSTR   16     /* size of strings in the volume record */

#define BPW     8

/*
 * header types.  the hdrlen[] array (dump and restor) assumes that
 * the numbers begin at 0, and that they are in this order.
 * the OINODE and ONAME records are not produced by dump, but were
 * produced by older versions, and restore knows how to interpret
 * them.
 */
#define FS_VOLUME        0
#define FS_FINDEX        1
#define FS_CLRI          2
#define FS_BITS          3
#define FS_OINODE        4
#define FS_ONAME         5
#define FS_VOLEND        6
#define FS_END           7
#define FS_DINODE         8
#define FS_NAME          9
#define FS_DS           10

struct hdr {                    /* common part of every header */
	uint8_t   len;              /* hdr length in dwords */
	uint8_t   type;             /* FS_* */
	uint16_t  magic;            /* magic number (MAGIC above) */
	uint16_t  checksum;
};

union fs_rec {

	/* common fields */
	struct hdr h;

	/* FS_VOLUME -- begins each volume */
	struct {
		struct  hdr h;
		uint16_t  volnum;         /* volume number */
		int32_t  date;           /* current date */
		int32_t  budate;          /* starting date */
		int32_t numwds;         /* number of wds this volume */
		char    disk[SIZSTR];   /* name of disk */
		char    fsname[SIZSTR]; /* name of file system */
		char    user[SIZSTR];   /* name of user */
		int16_t   incno;          /* dump level (or BYNAME) */
	} v;

	/* FS_FINDEX -- indexes files on this volume */
	struct {
		struct  hdr h;
		uint16_t  dummy;          /* get the alignment right */
		uint16_t   ino[FXLEN];     /* inumbers */
		int32_t addr[FXLEN];    /* addresses */
		int32_t link;           /* next volume record */
	} x;

	/* FS_BITS or FS_CLRI */
	struct {
		struct hdr h;
		uint16_t  nwds;           /* number of words of bits */
	} b;

	/* FS_OINODE */
	struct {
		struct hdr h;
		uint16_t  ino;            /* inumber */
		uint16_t  mode;           /* info from inode */
		uint16_t  nlink;
		uint16_t  uid;
		uint16_t  gid;
		int32_t   size;
		int32_t  atime;
		int32_t  mtime;
		int32_t  ctime;
		uint16_t   dev;            /* device file is on */
		uint16_t   rdev;           /* maj/min devno */
		int32_t   dsize;          /* dump size if packed */
	} oi;

	/* FS_INODE */
	struct {
		struct hdr h;
		uint16_t  ino;            /* inumber */
		uint16_t  mode;           /* info from inode */
		uint16_t  nlink;
		uint16_t  uid;
		uint16_t  gid;
		int32_t   size;
		int32_t  atime;
		int32_t  mtime;
		int32_t  ctime;
		uint16_t  devmaj;         /* device file is on */
		uint16_t  devmin;
		uint16_t  rdevmaj;        /* maj/min devno */
		uint16_t  rdevmin;
		int32_t   dsize;          /* dump size if packed */
		int32_t    pad;
	} i;

	/* FS_ONAME */
	/* must be exactly like FS_INODE except name at end */
	struct {
		struct hdr h;
		uint16_t  ino;
		uint16_t  mode;
		uint16_t  nlink;
		uint16_t  uid;
		uint16_t  gid;
		int32_t   size;
		int32_t  atime;
		int32_t  mtime;
		int32_t  ctime;
		uint16_t   dev;
		uint16_t   rdev;
		int32_t   dsize;
		char    name[DUMNAME];  /* file name given by user */
	} on;

	/* FS_NAME */
	/* must be exactly like FS_INODE except name at end */
	struct {
		struct hdr h;
		uint16_t  ino;
		uint16_t  mode;
		uint16_t  nlink;
		uint16_t  uid;
		uint16_t  gid;
		int32_t   size;
		int32_t  atime;
		int32_t  mtime;
		int32_t  ctime;
		uint16_t  devmaj;         /* device file is on */
		uint16_t  devmin;
		uint16_t  rdevmaj;        /* maj/min devno */
		uint16_t  rdevmin;
		int32_t   dsize;
		int32_t    pad;
		char    name[DUMNAME];  /* file name given by user */
	} n;

	/* FS_END or FS_VOLEND */
	struct {
		struct hdr h;
	} e;

	/* FS_DS */
	struct {
		struct hdr h;
		char	nid[8];
		char	qdir[2];	/* makes it 2 dwords */
	} ds;

};