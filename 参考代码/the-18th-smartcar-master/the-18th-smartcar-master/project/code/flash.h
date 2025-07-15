#ifndef __FLASH_H__
#define __FLASH_H__

#include "zf_common_headfile.h"

// 这段宏定义是为了演示如何向 FLASH 使用缓冲区写入和读取数据而定义
#define FLASH_SECTION_INDEX       (63)                                          // 存储数据用的扇区 倒数第一个扇区
#define FLASH_PAGE_INDEX          (3)                                           // 存储数据用的页码 倒数第一个页码

void  Read_Flash(void);
void  Write_Flash(void); 

#endif
