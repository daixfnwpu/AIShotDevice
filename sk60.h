// 激光测距仪相关操作功能
// sk60.h
// 激光测距仪相关操作功能
// sk60.h
#ifndef SK60_H
#define SK60_H

// 激光测距仪初始化
void SK60Init(void);

// 向激光测距仪发送数据
void SK60Serial_Send(uint8_t numbyte, uint8_t *k60_cmd_buf);

// 从激光测距仪读取数据
void SK60Serial_Recev(uint8_t numbyte, uint8_t *k60_recedv_buf);

// sk60 发送命令, cmd: 命令；parm：参数
void sk60_sendcmd(uint16_t cmd, uint8_t parm);

// 定义寄存器地址
#define REG_ERR_CODE  0x0000 // 系统状态代码
#define REG_BAT_VLTG  0x0006 // 工作电压
#define REG_ADDRESS  0x0010 // 模块地址
#define REG_OFFSET  0x0012 // 模块测量结果偏移
#define REG_MEA_START  0x0020 // 开始测量
#define REG_MEA_RESULT  0x0022 // 测量结果
#define REG_CTRL_LD  0x01be // 激光二极管的控制

// 32为高低字节互换
uint32_t swap_32b_bytes(uint32_t value);

// 16为高低字节互换
uint16_t swap_16b_bytes(uint16_t value);

// 激光测距仪测试
void Sk60Loop(void);

// 开启连续慢速自动测量
void Sk60Start(void);

// 关闭连续测量
void Sk60Stop(void);

// 读取数据
void Sk60Read(uint8_t *buf);



#endif // SK60_H