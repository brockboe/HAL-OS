#ifndef _FILESYS_H
#define _FILESYS_H

#define FNAME_MAX_LEN 32
#define MAX_DENTRY 63
#define FOURKB 4096
#define FILENAME_MAXLEN 32

#include "types.h"

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

typedef struct file_descriptor {
      union{
            uint32_t val[4];
            struct{
                  uint32_t file_op_table_ptr;
                  inode_t * inode;
                  uint32_t file_pos;
                  uint32_t flags;
            };
      } __attribute__ ((packed));
} file_descriptor_t;

typedef struct data_block {
      uint8_t data[4096];
} data_block_t;

void init_filesys(boot_block_t * bb);

int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);

int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);

int32_t stringcompare(const uint8_t * a, const uint8_t * b, int cmplen);

inode_t * file_open(uint8_t * fname);

uint32_t file_write();

uint32_t file_read(uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);

uint32_t file_close();

inode_t * dir_open(uint8_t * fname);

uint32_t dir_write();

uint32_t dir_read(uint32_t offset, uint8_t * buf, uint32_t nbytes);

uint32_t dir_close();

#endif
