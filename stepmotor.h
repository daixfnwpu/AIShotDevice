
#ifndef SETPMOTOR_H
#define SETPMOTOR_H

// 步进电机-初始设置, whatSpeed:每分钟转数；number_of_steps：每转的步数。
void StepperInit(long whatSpeed, int number_of_steps);

/*
* 转动电机steps_to_move步数。如果该数字为负，
* 电机沿相反方向转动。
*/
void movestep(int steps_to_move);

//步进电机- 转动测试
void StepperLoop();

#endif // SETPMOTOR_H