#ifndef _FILESYS_H
#define _FILESYS_H

#define FNAME_MAX_LEN 32
#define MAX_DENTRY 63
#define FOURKB 4096
#define FILENAME_MAXLEN 32

#include "types.h"

/*Directory entry structure*/
typedef struct dentry {
      union{
            uint32_t val[2];
            struct{
                  uint8_t file_name[32];
                  uint32_t file_type;
                  uint32_t inode_num;
                  uint8_t reserved[24];
            };
      }__attribute__ ((packed));
} dentry_t;

/*Boot block structure*/
typedef struct boot_block {
      uint32_t num_dir_entries;
      uint32_t num_inodes;
      uint32_t num_data_blocks;
      uint8_t reserved[52];
      dentry_t directory_entries[63];
} boot_block_t;

typedef struct inode {
      uint32_t length;
      uint32_t block_location[1023];
} inode_t;

/*file descriptor pointer*/
typedef struct file_descriptor {
      union{
            uint32_t val[4];
            struct{
                  int32_t (*operations_pointer)(uint32_t action, uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);
                  uint32_t inode;
                  uint32_t file_pos;
                  uint32_t flags;
            };
      } __attribute__ ((packed));
} file_descriptor_t;

typedef struct data_block {
      uint8_t data[4096];
} data_block_t;

/*Points to the very beginning of the filesystem*/
boot_block_t * filesys_begin;
/*Points to the beginning of the inodes in the filesystem*/
inode_t * inodes_begin;
/*Points to the beginning of the data blocks in the filesystem*/
data_block_t * data_block_begin;
/*The number of inodes within the file system*/
uint32_t num_inodes;

//initializes the file system
void init_filesys(boot_block_t * bb);

//fills in a dentry when given an index by name
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);

//reads data from a file
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);

//finds and records a dentry when given a numerical index
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);

//compares to strings to check if they are equal
int32_t stringcompare(const uint8_t * a, const uint8_t * b, int cmplen);

//opens a file
inode_t * file_open(uint8_t * fname);

//writes to a file
int32_t file_write();

//reads data from a file into a buffer
int32_t file_read(uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);

//close a file descriptor
int32_t file_close();

//opens a directory and returns the associated inode
inode_t * dir_open(uint8_t * fname);

//writes to a directory
int32_t dir_write();

//reads all the file names from a directory
int32_t dir_read(uint32_t offset, uint8_t * buf, uint32_t nbytes);

//close a directory file descriptor
int32_t dir_close();

//consolidates the directory read and write functionality
int32_t dir_io(uint32_t action, uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);

//consolidates the file read and write functionality.
int32_t file_io(uint32_t action, uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);

#endif
