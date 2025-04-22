#include "Arduino.h"
#include "stepmotor.h"


// 定义电机控制器管脚
#define  ENA_PIN  D0 // 允许控制
#define  IN1_PIN  D1 // IN1 脉冲1
#define  IN2_PIN  D2 // IN2 脉冲1

// 设置电机一转的总步数
int one_turn_number_steps = 0;

/*
 * 设置速度（每分钟转数）, 通过设置每步间隔时间来控制速度。
 */
long step_delay = 0;

unsigned long last_step_time = 0; // 最后一步的时间戳

int step_number = 0; // 转动步数计数

// 驱动马达, thisStep: 控制时序
void driveMotor(int thisStep);

/*
 * 控制电机的初始设置，, whatSpeed:每分钟转数；number_of_steps：每转的步数。
 */
void StepperInit(long whatSpeed, int number_of_steps) {
  // 设置控制管脚为输出模式
  pinMode(ENA_PIN, OUTPUT);
  // 设置脉冲管脚为输出模式
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  one_turn_number_steps = number_of_steps;

  // 将电机速度设置为whatSpeed RPM; 1分钟/一转的总步数/转数 = 一步所需时间
  step_delay = 60L * 1000L * 1000L / one_turn_number_steps / whatSpeed;
}

/*
* 转动电机steps_to_move步数。如果该数字为负，
* 电机沿相反方向转动。
*/
void movestep(int steps_to_move)
{
  int steps_left = abs(steps_to_move);  // 要走多少步
  int direction = 0;
  step_number = 0; // 记录马达转到一圈中的哪一步了

  // 根据steps_to_move是正还是负来确定转动方向
  if (steps_to_move > 0) { direction = 1; }
  if (steps_to_move < 0) { direction = 0; }


  // 每转动一步，步数减一。
  while (steps_left > 0)
  {
    unsigned long now = micros();
    // 转动时间
    if (now - last_step_time >= step_delay) // 到了一步所需时间，走下一步。
    {
      // 设置最后一转的时间为当前时间
      last_step_time = now;

      if (direction == 1) // 正转
      {
        step_number++;  // 转动步数加1
        if (step_number == one_turn_number_steps) { //转完一圈
          step_number = 0;
        }
      }
      else
      {
        if (step_number == 0) {  //转完一圈
          step_number = one_turn_number_steps;
        }
        step_number--;
      }

      // 要走的步数减一
      steps_left--;
      driveMotor(step_number % 4);
    }
  }
}

// 驱动马达, thisStep: 控制时序
void driveMotor(int thisStep)
{
  switch (thisStep) {
      case 0:  // 00
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
      break;
      case 1:  // 01
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH);
      break;
      case 2:  // 11
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, HIGH);
      break;
      case 3:  // 10
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
      break;
    }
}

void StepperLoop() {
  // 允许控制电机
  digitalWrite(ENA_PIN, HIGH);

  // 正转10转
  movestep(10 * one_turn_number_steps);

  delay (1000); //Wait 1 sec
  
  // 反转10转
  movestep(-10 * one_turn_number_steps);

  // 禁止控制电机
  digitalWrite(ENA_PIN, LOW);
}