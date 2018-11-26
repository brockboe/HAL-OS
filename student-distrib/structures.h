#ifndef _STRUCTURES_H
#define _STRUCTURES_H

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

typedef struct flags{
      uint8_t in_use;
      uint8_t reserved1;
      uint8_t reserved2;
      uint8_t reserved3;
} flags_t;

/*operations jump table used for syscalls*/
typedef struct op_jmp_table {
      int32_t (*dev_open)(const uint8_t * filename);
      int32_t (*dev_read)(uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);
      int32_t (*dev_write)(int32_t fd, const void * buf, int32_t n_bytes);
      int32_t (*dev_close)(int32_t fd);
} op_jmp_table_t;

/*file descriptor pointer*/
typedef struct file_descriptor {
      union{
            uint32_t val[4];
            struct{
                  op_jmp_table_t * actions;
                  uint32_t inode;
                  uint32_t file_pos;
                  flags_t flags;
            };
      } __attribute__ ((packed));
} file_descriptor_t;

/*Data block (found in the filesystem)*/
typedef struct data_block {
      uint8_t data[4096];
} data_block_t;

/*Structure containing all PCB information*/
typedef struct PCB {
      file_descriptor_t fd[8];
      uint8_t argbuf[128];
      uint8_t is_active;
      int32_t PID;
      uint32_t ss0;
      uint32_t esp0;
      uint32_t stack_pointer;
      uint32_t EBP;
      struct PCB * parent_pcb;
} PCB_t;

#endif
