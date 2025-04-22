
// 激光测距仪相关操作功能
#include <HardwareSerial.h>
#include "sk60.h"

HardwareSerial SK60Serial(0);

// 初始化
void SK60Init(void)
{
  // K60数据读取串口设置， SK60Serial0 on pins TX=D6 and RX=D7 (-1, -1 means use the default)
  SK60Serial.begin(19200, SERIAL_8N1, -1, -1);
  // Serial.print("SK60Serial");
}

// 向激光测距仪发送数据
void SK60Serial_Send(uint8_t numbyte, uint8_t *k60_cmd_buf)
{
  for (uint8_t i=0; i<numbyte; i++)
  {
    SK60Serial.write(*(k60_cmd_buf+i));
  }
}

// 从激光测距仪读取数据
void SK60Serial_Recev(uint8_t numbyte, uint8_t *k60_recev_buf)
{

  *k60_recev_buf = SK60Serial.read();
  if (*k60_recev_buf == 0xaa) // 正常通信
  {
    for (uint8_t i=1; i<numbyte; i++)
    {
      while (SK60Serial.available()){ // 其检查是否存在可从串行端口读取的任何数据
        *(k60_recev_buf+i) = SK60Serial.read();
        break;
      }       
    }   
  }
  else if (*k60_recev_buf == 0xee)// 应答错误，第一个字节是0xee
  { 
    Serial.printf("K60 recv err: %2x", *k60_recev_buf);
    for (uint8_t i=1; i<9; i++)
    {
      while (SK60Serial.available()){ // 其检查是否存在可从串行端口读取的任何数据
        *(k60_recev_buf+i) = SK60Serial.read();
         Serial.printf(" %2x", *(k60_recev_buf+i));
      }
      
    }
    Serial.printf("\n");
  } 
  else
  {
    return;
  }
  if (*k60_recev_buf == 0xaa) // 正常通信
  {
    Serial.printf("K60 distance: %dmm quality: %d \n", swap_32b_bytes(*(uint32_t *)(k60_recev_buf+6)), swap_16b_bytes(*(uint16_t *)(k60_recev_buf+10)));
  }
}


// sk60 发送命令, cmd: 命令；parm：参数
// SK60命令缓存, 0xaa开始，1byte:地址;2and3byte: 寄存器地址；4and5byte: 有效计算；6and7byte:是有效字节；8byte:校验和(1~7byte之和)
void sk60_sendcmd(uint16_t cmd, uint8_t parm) {
  switch (cmd) {
    case REG_ERR_CODE: // 读取系统状态代码
    {
      uint8_t buf[5] = {0xaa, 0x80, 0x00, 0x00,0x80};
      // memcpy(k60_cmd_buf, buf, sizeof(buf));
      SK60Serial_Send(5, buf);
      // delay(100);
      // SK60Serial_Recev(9);
      break;
    }
    case REG_BAT_VLTG: // 读取工作电压
    {
      uint8_t buf[5] = {0xaa, 0x80, 0x00, 0x06,0x86};
      // memcpy(k60_cmd_buf, buf, sizeof(buf));
      SK60Serial_Send(5, buf);
      // delay(100);
      // SK60Serial_Recev(9);
      break;
    }
    
    case REG_MEA_START: // 开始测量
    {
      if (parm == 6) // 结束连续测量
      {
        uint8_t buf[1]  = {0x58}; // X
        SK60Serial_Send(1, buf);
      }
      else{
        if (parm == 0) // 单次自动测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x21};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
           SK60Serial_Send(9, buf);
        }
        if (parm == 1) // 单次慢速测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x01, 0x22};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
          SK60Serial_Send(9, buf);
        }
        else if (parm == 2) // 单次快速测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x02, 0x23};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
          SK60Serial_Send(9, buf);
        }
        else if (parm == 3) // 连续自动测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x04, 0x25};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
          SK60Serial_Send(9, buf);
        }
        else if (parm == 4) // 连续速慢测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x05, 0x26};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
          SK60Serial_Send(9, buf);
        }
        else if (parm == 5) // 连续速快测量
        {
          uint8_t buf[9] = {0xaa, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x06, 0x27};
          // memcpy(k60_cmd_buf, buf, sizeof(buf));
          SK60Serial_Send(9, buf);
        }
        
        // SK60Serial_Send(9);
        // delay(500);
        // SK60Serial_Recev(13);
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

// 32为高低字节互换
uint32_t swap_32b_bytes(uint32_t value) {
    return ((value >> 24) & 0xFF) | // 取出4字节的8位并放到第1字节低8位
           ((value >> 8) & 0xFF00) | // 取出3字节8位并放到2字节8位
           ((value << 8) & 0xFF0000) | // 取出2字节8位并放到3字节8位
           ((value << 24) & 0xFF000000); // 取出低8位并放到高8位
}

// 16为高低字节互换
uint16_t swap_16b_bytes(uint16_t value) {
    return ((value >> 8) & 0xFF) | // 取出高8位并放到低8位
           ((value << 8) & 0xFF00); // 取出低8位并放到高8位
}

// 开启连续慢速自动测量
void Sk60Start(void)
{
  uint16_t cmd = 0;
   // 读取距离数据
  cmd = REG_MEA_START;
  sk60_sendcmd(cmd, 4); 
}

// 关闭连续测量
void Sk60Stop(void)
{
  uint16_t cmd = 0;
   // 读取距离数据
  cmd = REG_MEA_START;
  sk60_sendcmd(cmd, 6); 
}

// 读取数据
void Sk60Read(uint8_t *buf)
{
  SK60Serial_Recev(13, buf); 
}



// 激光测距仪测试
void Sk60Loop(void)
{
  uint16_t cmd = 0;
   // 读取距离数据
  cmd = REG_MEA_START;
  sk60_sendcmd(cmd, 4); // 开启连续慢速自动测量
  uint8_t recv_buf[13] = {0};
  SK60Serial_Recev(13, recv_buf); // 连续读5次， 间隔是1秒
  delay (1000);
  SK60Serial_Recev(13, recv_buf);
  delay (1000);
  SK60Serial_Recev(13, recv_buf);
  delay (1000);
  SK60Serial_Recev(13, recv_buf);
  delay (1000);
  SK60Serial_Recev(13, recv_buf);
  delay (1000); //Wait 5 sec
  sk60_sendcmd(cmd, 6); // 关闭连续测量
}