#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

//设置 MIO引脚地址
#define MIO_PIN_07		(*(volatile unsigned int *)0xF800071C)
//#define MIO_PIN_50		(*(volatile unsigned int *)0xF80007C8)
//#define MIO_PIN_51		(*(volatile unsigned int *)0xF80007CC)

//设置 GPIO端口方向寄存器地址
#define DIRM_0			(*(volatile unsigned int *)0xE000A204)
//#define DIRM_1			(*(volatile unsigned int *)0xE000A244)
#define DIRM_2			(*(volatile unsigned int *)0xE000A284)
#define DIRM_3			(*(volatile unsigned int *)0xE000A2C4)
//设置 GPIO端口输出使能寄存器地址
#define OEN_0			(*(volatile unsigned int *)0xE000A208)
#define OEN_1			(*(volatile unsigned int *)0xE000A248)
#define OEN_2			(*(volatile unsigned int *)0xE000A288)
#define OEN_3			(*(volatile unsigned int *)0xE000A2C8)
//设置 GPIO端口输出寄存器地址
#define DATA_0			(*(volatile unsigned int *)0xE000A040)
#define DATA_1			(*(volatile unsigned int *)0xE000A044)
#define DATA_2			(*(volatile unsigned int *)0xE000A048)
#define DATA_3			(*(volatile unsigned int *)0xE000A04C)
//设置 GPIO端口输入寄存器地址
#define DATA_0_RO		(*(volatile unsigned int *)0xE000A060)
#define DATA_1_RO		(*(volatile unsigned int *)0xE000A064)
#define DATA_2_RO		(*(volatile unsigned int *)0xE000A068)
#define DATA_3_RO		(*(volatile unsigned int *)0xE000A06C)

//设置 UART1引脚地址的宏定义
#define rMIO_PIN_48		(*(volatile unsigned long*)0xF80007C0)
#define rMIO_PIN_49 	(*(volatile unsigned long*)0xF80007C4)
#define rUART_CLK_CTRL 	(*(volatile unsigned long*)0xF8000154)
#define rControl_reg0 	(*(volatile unsigned long*)0xE0001000)
#define rMode_reg0 		(*(volatile unsigned long*)0xE0001004)
//设置 UART1端口波特率等参数地址寄存器的宏定义
#define rBaud_rate_gen_reg0 (*(volatile unsigned long*)0xE0001018)
#define rBaud_rate_divider_reg0 (*(volatile unsigned long*)0xE0001034)
#define rTx_Rx_FIFO0 (*(volatile unsigned long*)0xE0001030)
#define rChannel_sts_reg0 (*(volatile unsigned long*)0xE000102C)

void arm(int Arm_id,int Arm_dir);
void box(void);
void plate(int Plate_ID,int Plate_dir);
void trans(int Trans_dir);
void car(int id,int dir);
void send_Char_9(unsigned char modbus[]);				//9字节串口发送函数
unsigned rec_Char(void);
void send_Char(unsigned char data);						//串口发送函数，一次一个字节
void RS232_Init();										//串口初始化函数

void delay(int i,int n,int m);							//延时函数
int move[10];
int des[10]={30,7,-23,0,45};
int turn=60;

int main()
{
	u32 flag;		//变量flag用于记录SW0~SW7按键按下信息；

	//注：下面MIO引脚和EMIO引脚的序号是统一编号的，MIO序号为0~31及32~53，EMIO序号为54~85及86~117
	//配置及初始化MIO07引脚的相关寄存器，MIO07作为LED灯控制的输出引脚
	//向寄存器MIO_PIN_N中写入相应的参数设置MIO引脚为GPIO
	MIO_PIN_07 = 0x00003600;
	//设置MIO引脚方向为输出
	DIRM_0 = DIRM_0|0x00000080;
	//OEN使能输出
	OEN_0 = OEN_0|0x00000080;
	//配置及初始化MIO50、MIO51引脚的相关寄存器，MIO50、MIO51作为按键输入引脚
	//MIO_PIN_50 = 0x00003600;
	//MIO_PIN_51 = 0x00003600;
	//DIRM_1 = DIRM_1 & 0xFFF3FFFF;
	//初始化EMIO54~EMIO58的引脚，设置BTNU、BTND、BTNL、BTNR、BTNC按键为输入方向
	DIRM_2 = DIRM_2 & 0xFFFFFFE0;
	//初始化EMIO59~EMIO66的引脚，设置SW7~SW0拨动开关为输入方向
	DIRM_2 = DIRM_2 & 0xFFFFE01F;
	//初始化EMIO67~EMIO74的引脚，设置LED7~LED0为输出方向
	DIRM_2 = DIRM_2|0x001FE000;
	OEN_2 = OEN_2|0x001FE000;

	//初始化UART1
	RS232_Init();


	//记录恢复要进行的操作，原操作的反操作
	//初始化MODBUS编码

	int i = 0;

    while(1){
    	//读模式信息，即读SW7、SW6的输入信息
    	flag = DATA_2_RO&0x00000060;
    	u32 flag_button = DATA_2_RO & 0x0000001F;       //取BTNU,BTND,BTNL,BTNR,BTNC按键
        switch(flag){
        	case 0x00:					//复位模式
        		DATA_2 = DATA_2&0xFFE01FFF;		//模式指示灯LED7~LED0灭

        		break;

        	case 0x20:					//手动控制模式

				flag = DATA_2_RO & 0x00000780;                  //取SW2,3,4,5来决定是哪一种模式，机械臂/轨道/吸盘/箱子/传送带

				switch(flag){
                    case 0x000:
                        //第一个轴
                        //点灯
						DATA_2 = (DATA_2|0x00002000) & 0x00002FFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[0]++;
           	                arm(1,0);



           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[0]--;
           	                arm(1,1);


           	            }
                        break;
                        //
                    case 0x080:
                        //第二个轴:弯腰
                        //点灯
						DATA_2 = (DATA_2|0x0000A000)& 0x0000AFFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[1]++;
           	                arm(2,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[1]--;
           	                arm(2,1);


           	            }
                        break;
                        //
                    case 0x100:
                        //第三个轴：抬胳膊
                        //点灯
						DATA_2 = (DATA_2|0x00012000)& 0x00012FFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[2]++;
           	                arm(3,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[2]--;
           	                arm(3,1);


           	            }
                        break;
                        //
                    case 0x180:
                        //第四个轴：旋转，小臂转身
                        //点灯
						DATA_2 = (DATA_2|0x0001A000)& 0x0001AFFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[3]++;
           	                arm(4,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = (DATA_0 | 0x00000080)& 0x00008FFF;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[3]--;
           	                arm(4,1);


           	            }
                        break;
                        //
                     case 0x200:
                        //第五个轴：
                        //点灯
						DATA_2 = (DATA_2|0x00022000) & 0x00022FFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[4]++;
           	                arm(5,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[4]--;
           	                arm(5,1);


           	            }
                        break;
                        //
                    case 0x280:
                        //第六个轴：抓箱子
                        //点灯
						DATA_2 = (DATA_2|0x0002A000) & 0x0002AFFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                move[5]++;
           	                arm(6,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                move[5]--;
           	                arm(6,1);


           	            }
                        break;
                        //
                     case 0x300:
                        //轨道移动：机械臂轨道
                        //点灯
						DATA_2 = (DATA_2|0x00032000)& 0x00032FFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //左移，2档
           	                move[6]++;
           	                arm(7,0);


           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //右移，2档
           	                move[6]--;
           	                arm(7,1);


           	            }
                        break;
                        //
                     case 0x380:
                        //吸盘
                        //点灯
						DATA_2 = (DATA_2|0x0003A000) & 0x0003AFFF;
						if (flag_button == 0x00000004){				//判断BTNL按键是否按下
						    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
						    delay(200,500,50);					//延时约1秒，进行消抖动处理
						    flag_button = DATA_2_RO & 0x0000001F;
						    while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
						          flag_button = DATA_2_RO & 0x0000001F;
						    }
						    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
						    //动作吸
						    plate(2,0);
						}
						else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
						     DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
						     delay(200,500,50);					//延时约1秒，进行消抖动处理
						     flag_button = DATA_2_RO & 0x0000001F;
						     while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
						          flag_button = DATA_2_RO & 0x0000001F;
						     }
						     DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
						     //动作放
						     plate(2,1);
						}
						break;
                        //
                    case 0x400:
                        //箱子
                        //点灯
						DATA_2 = (DATA_2|0x00042000) & 0x00042FFF;
                        if (flag_button == 0x00000010){				//判断BTNC按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000010){			//判断BTNC按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //放箱子
           	                box();
           	            }
                        break;
                        //
                     case 0x480:
                        //传送带
                        //点灯
						DATA_2 = (DATA_2|0x0004A000) & 0x0004AFFF;
                        if (flag_button == 0x00000004){				//判断BTNL按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴顺时针，转速角度为3
           	                trans(1);
           	            }
           	            else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	                delay(200,500,50);					//延时约1秒，进行消抖动处理
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	                //轴逆时针，转速角度为3
           	                trans(2);
           	            }
                        break;


                     case 0x500:
						//传送带

						 if (flag_button == 0x00000004){				//判断BTNL按键是否按下
								DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
								delay(200,500,50);					//延时约1秒，进行消抖动处理
								flag_button = DATA_2_RO & 0x0000001F;
								while(flag_button == 0x00000004){			//判断BTNL按键是否抬起
									flag_button = DATA_2_RO & 0x0000001F;
								}
								DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
								//轴顺时针，转速角度为3
								for(int i=0;i<18;i++){
									car(3,1);
								}
								car(4,1);

							}
							else if (flag_button == 0x00000008){			//判断BTNR按键是否按下
								DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
								delay(200,500,50);					//延时约1秒，进行消抖动处理
								flag_button = DATA_2_RO & 0x0000001F;
								while(flag_button == 0x00000008){			//判断BTNR按键是否抬起
									flag_button = DATA_2_RO & 0x0000001F;
								}
								DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
								//轴逆时针，转速角度为3
								car(3,2);
							}
						 break;
                        //
               }
				break;
			case 0x40:					//自动控制模式
        		DATA_2 = (DATA_2|0x00004000)&0xFFFF7FFF;	//LED7灭、LED6亮
        		if (flag_button == 0x00000004){
        			int n=10;
        			while(n--){
						box();
						trans(1);


						//1
						for(int i=0;i<90/3;i++){
							arm(1,0);
							delay(100,500,50);
						}
						arm(1,2);
						//2
						for(int i=0;i<26/3;i++){
							arm(2,0);
							delay(100,500,50);
						}
						arm(2,2);
						delay(200,500,50);


						i=0;
						while(i<400000000){
								i++;
						}


						//3
						for(int i=0;i<74/3;i++){
							arm(3,1);
							delay(100,500,50);
						}
						arm(3,2);
						//4
						for(int i=0;i<181/3;i++){
							arm(4,1);
							delay(100,500,50);
						}
						arm(4,2);

						delay(1000,500,50);

						//5
						for(int i=0;i<41/3;i++){
							arm(5,0);
							delay(100,500,50);
						}
						arm(5,2);
						delay(100,500,50);



						plate(2,0);





						for(int i=0;i<26/3;i++){
							arm(2,1);
							arm(5,1);
							delay(100,500,50);
						}

						//color
						char data = rec_Char();
						int r=5;
						int r25=10/3;
						int left=1;

						if(data=='R'){
							r=20;
							left=0;
							//小车的移动测试
							for(int i=0;i<10;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							for(int i=0;i<17;i++){
								car(2,1);
								delay(100,500,50);
							}
							car(2,0);
							for(int i=0;i<13;i++){
								car(1,1);
								delay(100,500,50);
							}
							car(1,0);
							delay(1000,100,10);




						}
						else if(data=='G'){
							r=5;
							left=1;


							for(int i=0;i<7;i++){
								car(2,2);

							}
							car(2,0);
							for(int i=0;i<2;i++){
								car(1,1);
							}
							car(1,0);
							delay(1000,100,10);

						}
						else if(data=='B'){
							r=20;
							left=1;



							//小车的移动测试
							for(int i=0;i<10;i++){
								car(1,2);
							}
							car(1,0);
							for(int i=0;i<24;i++){
								car(2,2);

							}
							car(2,0);
							for(int i=0;i<14;i++){
								car(1,1);
							}
							car(1,0);
							delay(1000,100,10);




						}

						//put down 20 arm(7,0) blue
						for(int i=0;i<r;i++){
							if(left) arm(7,0);
							else arm(7,1);
							delay(100,500,50);
						}
						arm(7,2);

						for(int i=0;i<180/3;i++){
							arm(1,0);
							delay(100,500,50);
						}
						arm(1,2);

						for(int i=0;i<r25;i++){
							arm(2,0);
							arm(5,0);
							delay(100,500,50);
						}

						delay(1000,500,50);
						plate(2,1);

						i=0;
						while(i<40000000){
								i++;
						}
						delay(1000,500,50);
						plate(3,0);
						if(data=='R'){
							//后退
							delay(1000,500,50);
							for(int i=0;i<13;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//右50
							for(int i=0;i<50;i++){
								car(2,2);

							}
							car(2,0);
							//后10
							for(int i=0;i<10;i++){
								car(1,2);

							}
							car(1,0);
							//右10
							for(int i=0;i<10;i++){
								car(2,2);

							}
							car(2,0);

							//90
							//for(int i=0;i<30;i++){
							//	car(3,2);
							//	delay(100,500,50);
							//}
							//car(3,0);

							//前50
//							for(int i=0;i<50;i++){
//								car(1,1);
//								delay(100,500,50);
//							}
//							car(1,0);
//							//右10
//							for(int i=0;i<10;i++){
//								car(2,2);
//								delay(100,500,50);
//
//							}
//
//
//							car(2,0);
							delay(1000,100,10);
						}
						else if(data=='G'){
							//后退
							delay(1000,500,50);
							for(int i=0;i<12;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//右26
							for(int i=0;i<26;i++){
								car(2,2);

							}
							car(2,0);
							//后35
							for(int i=0;i<35;i++){
								car(1,2);

							}
							car(1,0);
							//右10
							for(int i=0;i<10;i++){
								car(2,2);

							}
							car(2,0);
							delay(1000,100,10);
						}
						else if(data=='B'){
							//后退
							delay(1000,500,50);
							for(int i=0;i<14;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//右7
							for(int i=0;i<7;i++){
								car(2,2);

							}
							car(2,0);
							//后50
							for(int i=0;i<50;i++){
								car(1,2);

							}
							car(1,0);
							//右10
							for(int i=0;i<10;i++){
								car(2,2);

							}
							car(2,0);
							delay(1000,100,10);
						}
						for(int i=0;i<18;i++){
							car(3,2);
						}
						plate(3,1);
						//car
						delay(1000,500,50);
						for(int i=0;i<r25;i++){
							arm(2,1);
							arm(5,1);
							delay(100,500,50);
						}

						//reset

						for(int i=0;i<90/3;i++){
							arm(1,0);
							delay(100,500,50);
						}
						arm(1,2);

						//4
						for(int i=0;i<181/3;i++){
							arm(4,1);
							delay(100,500,50);
						}
						arm(4,2);

						//3
						for(int i=0;i<72/3;i++){
							arm(3,0);
							delay(100,500,50);
						}
						arm(3,2);

						//5
						for(int i=0;i<15/3;i++){
							arm(5,1);
							delay(100,500,50);
						}
						arm(5,2);






						for(int i=0;i<r;i++){
							if(left) arm(7,1);
							else arm(7,0);
							delay(100,500,50);
						}
						arm(7,2);

						trans(0);


						RS232_Init();


						for(int i=0;i<18;i++){
							car(3,1);
						}
						//小车回去
						if(data=='R'){
							//左10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//前10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);
							//左33
							for(int i=0;i<33;i++){
								car(2,1);

							}
							car(2,0);
							//前10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);


//							//90
//							for(int i=0;i<30;i++){
//								car(3,1);
//							}
//							car(3,0);
//
//							//左10
//							for(int i=0;i<10;i++){
//								car(2,1);
//
//							}
//							car(2,0);
//
//							//后33
//							for(int i=0;i<33;i++){
//								car(1,2);
//							}
//							car(1,0);
//							//前10
//							for(int i=0;i<10;i++){
//								car(1,1);
//							}
//							car(1,0);




						}
						else if(data=='G'){
							//左10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//前35
							for(int i=0;i<35;i++){
								car(1,1);

							}
							car(1,0);
							//左33
							for(int i=0;i<33;i++){
								car(2,1);

							}
							car(2,0);
							//前10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);
//							for(int i=0;i<2;i++){
//								car(1,2);
//							}
//							car(1,0);
//							for(int i=0;i<7;i++){
//								car(2,1);
//
//							}
//							car(2,0);
//
//							delay(1000,100,10);

						}
						else if(data=='B'){

							//左10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//前50
							for(int i=0;i<50;i++){
								car(1,1);

							}
							car(1,0);
							//左33
							for(int i=0;i<31;i++){
								car(2,1);

							}
							car(2,0);
							//前10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);




						}



        			}


        	}
        		break;
			case 0x60:					//机械臂示教模式（该模式暂不实现）
        		DATA_2 = DATA_2|0x00006000;					//LED7亮、LED6亮
        		break;
        	}
    }
    return 0;
}

//机械臂相关各部件动作函数
void arm(int Arm_ID,int Arm_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='2';				//机械臂相关
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	switch(Arm_ID){
	case 1:						//第一个轴
	     if (Arm_dir==0){
		    modbus_com[2]='3';
	     }
	     else if(Arm_dir==1){
	    	modbus_com[2]='7';
	     }else{
	    	 modbus_com[2]='0';
	     }
	     break;
	case 2:						//第二个轴
		 if (Arm_dir==0){
		    modbus_com[3]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[3]='7';
		 }else{
			 modbus_com[3]='0';
		 }

	   	 break;
	case 3:						//第三个轴
		 if (Arm_dir==0){
		    modbus_com[4]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[4]='7';
		 }else{
			 modbus_com[4]='0';
		 }
		 break;
	case 4:						//第四个轴
		 if (Arm_dir==0){
		    modbus_com[5]='3';
		 }
		 else if(Arm_dir==1){
		   	modbus_com[5]='7';
		 }else{
			modbus_com[5]='0';
		 }
	   	 break;
	case 5:						//第五个轴
		 if (Arm_dir==0){
		    modbus_com[6]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[6]='7';
		 }else{
			 modbus_com[6]='0';
		 }
         break;
	case 6:						//第六个轴
		 if (Arm_dir==0){
		    modbus_com[7]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[7]='7';
		 }else{
			 modbus_com[7]='0';
		 }
		 break;
	case 7:						//轨道上的移动
		 if (Arm_dir==0){
		    modbus_com[8]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[8]='6';
		 }else{
			 modbus_com[8]='0';
		 }
		 break;
	}
	send_Char_9(modbus_com);
}

//箱子相关函数
void box(void)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='4';				//箱子
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';
    //2号站
    modbus_com[3] = '1';
	send_Char_9(modbus_com);
}

//吸盘相关函数
void plate(int Plate_ID,int Plate_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='5';				//吸盘
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	switch(Plate_ID){
	case 1:						//1号站
         if(Plate_dir == 0){
             modbus_com[2]='1';
         }else if(Plate_dir == 1){
             modbus_com[2]='2';
         }
	     break;
	case 2:						//2号站
         if(Plate_dir == 0){
             modbus_com[3]='1';
         }else if(Plate_dir == 1){
             modbus_com[3]='2';
         }
	   	 break;
	case 3:						//3号站
          if(Plate_dir == 0){
             modbus_com[4]='1';
         }else if(Plate_dir == 1){
             modbus_com[4]='2';
         }
	}
	send_Char_9(modbus_com);
}

//传送带相关函数
void trans(int Trans_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='6';				//传送带
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	//控制3号传送带
    if (Trans_dir==0){
    	modbus_com[3]='0';
    }
    else if(Trans_dir == 1){
        modbus_com[3]='1';
    }else if(Trans_dir == 2){
        modbus_com[3]='2';
    }
	send_Char_9(modbus_com);
}
void car(int id,int dir){
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='3';				//传送带
	modbus_com[2]='0';
	modbus_com[3]='0';
	modbus_com[4]='0';
	modbus_com[5]='0';
	modbus_com[6]='0';
	modbus_com[7]='0';
	modbus_com[8]='0';
	switch(id){
		case 1:						//前后
		     if (dir==1){
			    modbus_com[2]='1';
		     }
		     else if(dir==2){
		    	modbus_com[2]='2';
		     }else{
		    	 modbus_com[2]='0';
		     }
		     break;
		case 2:						//左右
			 if (dir==1){
			    modbus_com[3]='1';
			 }
			 else if(dir==2){
			    modbus_com[3]='2';
			 }else{
				 modbus_com[3]='0';
			 }

		   	 break;
		case 3:						//旋转
			 if (dir==1){
			    modbus_com[4]='1';
			 }
			 else if(dir==2){
			    modbus_com[4]='2';
			 }else{
				 modbus_com[4]='0';
			 }
			 break;
		case 4:						//上下
			 if (dir==1){
			    modbus_com[5]='1';//吸盘
			 }
			 else if(dir==2){
			   	modbus_com[5]='2';//放置
			 }else{
				modbus_com[5]='0';
			 }
		   	 break;
		case 5:						//红色的门
			 if (dir==1){
			    modbus_com[6]='1';
			 }
			 else if(dir==2){
			    modbus_com[6]='2';
			 }else{
				 modbus_com[6]='0';
			 }
	         break;
		case 6:						//绿色的门
			 if (dir==1){
			    modbus_com[7]='1';
			 }
			 else if(dir==2){
			    modbus_com[7]='2';
			 }else{
				 modbus_com[7]='0';
			 }
			 break;
		case 7:						//未使用

			 break;
		}
	send_Char_9(modbus_com);
}
//9个字节数据的发送函数
void send_Char_9(unsigned char modbus[])
{
	int i;
	char data;
	for(i=0;i<9;i++){
		data=modbus[i];
		send_Char(data);
		delay(100,10,10);		//延时
	}
}

//单个字节数据的发送函数
void send_Char(unsigned char data)
{
     while((rChannel_sts_reg0&0x10)==0x10);
     rTx_Rx_FIFO0=data;
}

unsigned rec_Char(void){
	char data;
	while((rChannel_sts_reg0 & 0x2) == 0x2);
	data = rTx_Rx_FIFO0;
	return data;
}

//UART1的初始化函数
void RS232_Init()
{
     rMIO_PIN_48=0x000026E0;
     rMIO_PIN_49=0x000026E0;
     rUART_CLK_CTRL=0x00001402;
     rControl_reg0=0x00000017;
     rMode_reg0=0x00000020;
     rBaud_rate_gen_reg0=62;
     rBaud_rate_divider_reg0=6;
}

//延时函数
void delay(int n,int m,int p)
{
	 int i,j,k;
	 for(i=1;i<=n;i++){
		 for(j=1;j<=m;j++){
			 for(k=1;k<=p;k++){

			 }
		 }
	 }
}
