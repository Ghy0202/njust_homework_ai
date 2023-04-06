#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

//���� MIO���ŵ�ַ
#define MIO_PIN_07		(*(volatile unsigned int *)0xF800071C)
//#define MIO_PIN_50		(*(volatile unsigned int *)0xF80007C8)
//#define MIO_PIN_51		(*(volatile unsigned int *)0xF80007CC)

//���� GPIO�˿ڷ���Ĵ�����ַ
#define DIRM_0			(*(volatile unsigned int *)0xE000A204)
//#define DIRM_1			(*(volatile unsigned int *)0xE000A244)
#define DIRM_2			(*(volatile unsigned int *)0xE000A284)
#define DIRM_3			(*(volatile unsigned int *)0xE000A2C4)
//���� GPIO�˿����ʹ�ܼĴ�����ַ
#define OEN_0			(*(volatile unsigned int *)0xE000A208)
#define OEN_1			(*(volatile unsigned int *)0xE000A248)
#define OEN_2			(*(volatile unsigned int *)0xE000A288)
#define OEN_3			(*(volatile unsigned int *)0xE000A2C8)
//���� GPIO�˿�����Ĵ�����ַ
#define DATA_0			(*(volatile unsigned int *)0xE000A040)
#define DATA_1			(*(volatile unsigned int *)0xE000A044)
#define DATA_2			(*(volatile unsigned int *)0xE000A048)
#define DATA_3			(*(volatile unsigned int *)0xE000A04C)
//���� GPIO�˿�����Ĵ�����ַ
#define DATA_0_RO		(*(volatile unsigned int *)0xE000A060)
#define DATA_1_RO		(*(volatile unsigned int *)0xE000A064)
#define DATA_2_RO		(*(volatile unsigned int *)0xE000A068)
#define DATA_3_RO		(*(volatile unsigned int *)0xE000A06C)

//���� UART1���ŵ�ַ�ĺ궨��
#define rMIO_PIN_48		(*(volatile unsigned long*)0xF80007C0)
#define rMIO_PIN_49 	(*(volatile unsigned long*)0xF80007C4)
#define rUART_CLK_CTRL 	(*(volatile unsigned long*)0xF8000154)
#define rControl_reg0 	(*(volatile unsigned long*)0xE0001000)
#define rMode_reg0 		(*(volatile unsigned long*)0xE0001004)
//���� UART1�˿ڲ����ʵȲ�����ַ�Ĵ����ĺ궨��
#define rBaud_rate_gen_reg0 (*(volatile unsigned long*)0xE0001018)
#define rBaud_rate_divider_reg0 (*(volatile unsigned long*)0xE0001034)
#define rTx_Rx_FIFO0 (*(volatile unsigned long*)0xE0001030)
#define rChannel_sts_reg0 (*(volatile unsigned long*)0xE000102C)

void arm(int Arm_id,int Arm_dir);
void box(void);
void plate(int Plate_ID,int Plate_dir);
void trans(int Trans_dir);
void car(int id,int dir);
void send_Char_9(unsigned char modbus[]);				//9�ֽڴ��ڷ��ͺ���
unsigned rec_Char(void);
void send_Char(unsigned char data);						//���ڷ��ͺ�����һ��һ���ֽ�
void RS232_Init();										//���ڳ�ʼ������

void delay(int i,int n,int m);							//��ʱ����
int move[10];
int des[10]={30,7,-23,0,45};
int turn=60;

int main()
{
	u32 flag;		//����flag���ڼ�¼SW0~SW7����������Ϣ��

	//ע������MIO���ź�EMIO���ŵ������ͳһ��ŵģ�MIO���Ϊ0~31��32~53��EMIO���Ϊ54~85��86~117
	//���ü���ʼ��MIO07���ŵ���ؼĴ�����MIO07��ΪLED�ƿ��Ƶ��������
	//��Ĵ���MIO_PIN_N��д����Ӧ�Ĳ�������MIO����ΪGPIO
	MIO_PIN_07 = 0x00003600;
	//����MIO���ŷ���Ϊ���
	DIRM_0 = DIRM_0|0x00000080;
	//OENʹ�����
	OEN_0 = OEN_0|0x00000080;
	//���ü���ʼ��MIO50��MIO51���ŵ���ؼĴ�����MIO50��MIO51��Ϊ������������
	//MIO_PIN_50 = 0x00003600;
	//MIO_PIN_51 = 0x00003600;
	//DIRM_1 = DIRM_1 & 0xFFF3FFFF;
	//��ʼ��EMIO54~EMIO58�����ţ�����BTNU��BTND��BTNL��BTNR��BTNC����Ϊ���뷽��
	DIRM_2 = DIRM_2 & 0xFFFFFFE0;
	//��ʼ��EMIO59~EMIO66�����ţ�����SW7~SW0��������Ϊ���뷽��
	DIRM_2 = DIRM_2 & 0xFFFFE01F;
	//��ʼ��EMIO67~EMIO74�����ţ�����LED7~LED0Ϊ�������
	DIRM_2 = DIRM_2|0x001FE000;
	OEN_2 = OEN_2|0x001FE000;

	//��ʼ��UART1
	RS232_Init();


	//��¼�ָ�Ҫ���еĲ�����ԭ�����ķ�����
	//��ʼ��MODBUS����

	int i = 0;

    while(1){
    	//��ģʽ��Ϣ������SW7��SW6��������Ϣ
    	flag = DATA_2_RO&0x00000060;
    	u32 flag_button = DATA_2_RO & 0x0000001F;       //ȡBTNU,BTND,BTNL,BTNR,BTNC����
        switch(flag){
        	case 0x00:					//��λģʽ
        		DATA_2 = DATA_2&0xFFE01FFF;		//ģʽָʾ��LED7~LED0��

        		break;

        	case 0x20:					//�ֶ�����ģʽ

				flag = DATA_2_RO & 0x00000780;                  //ȡSW2,3,4,5����������һ��ģʽ����е��/���/����/����/���ʹ�

				switch(flag){
                    case 0x000:
                        //��һ����
                        //���
						DATA_2 = (DATA_2|0x00002000) & 0x00002FFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[0]++;
           	                arm(1,0);



           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[0]--;
           	                arm(1,1);


           	            }
                        break;
                        //
                    case 0x080:
                        //�ڶ�����:����
                        //���
						DATA_2 = (DATA_2|0x0000A000)& 0x0000AFFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[1]++;
           	                arm(2,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[1]--;
           	                arm(2,1);


           	            }
                        break;
                        //
                    case 0x100:
                        //�������᣺̧�첲
                        //���
						DATA_2 = (DATA_2|0x00012000)& 0x00012FFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[2]++;
           	                arm(3,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[2]--;
           	                arm(3,1);


           	            }
                        break;
                        //
                    case 0x180:
                        //���ĸ��᣺��ת��С��ת��
                        //���
						DATA_2 = (DATA_2|0x0001A000)& 0x0001AFFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[3]++;
           	                arm(4,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = (DATA_0 | 0x00000080)& 0x00008FFF;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[3]--;
           	                arm(4,1);


           	            }
                        break;
                        //
                     case 0x200:
                        //������᣺
                        //���
						DATA_2 = (DATA_2|0x00022000) & 0x00022FFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[4]++;
           	                arm(5,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[4]--;
           	                arm(5,1);


           	            }
                        break;
                        //
                    case 0x280:
                        //�������᣺ץ����
                        //���
						DATA_2 = (DATA_2|0x0002A000) & 0x0002AFFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                move[5]++;
           	                arm(6,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                move[5]--;
           	                arm(6,1);


           	            }
                        break;
                        //
                     case 0x300:
                        //����ƶ�����е�۹��
                        //���
						DATA_2 = (DATA_2|0x00032000)& 0x00032FFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //���ƣ�2��
           	                move[6]++;
           	                arm(7,0);


           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //���ƣ�2��
           	                move[6]--;
           	                arm(7,1);


           	            }
                        break;
                        //
                     case 0x380:
                        //����
                        //���
						DATA_2 = (DATA_2|0x0003A000) & 0x0003AFFF;
						if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
						    DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
						    delay(200,500,50);					//��ʱԼ1�룬��������������
						    flag_button = DATA_2_RO & 0x0000001F;
						    while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
						          flag_button = DATA_2_RO & 0x0000001F;
						    }
						    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
						    //������
						    plate(2,0);
						}
						else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
						     DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
						     delay(200,500,50);					//��ʱԼ1�룬��������������
						     flag_button = DATA_2_RO & 0x0000001F;
						     while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
						          flag_button = DATA_2_RO & 0x0000001F;
						     }
						     DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
						     //������
						     plate(2,1);
						}
						break;
                        //
                    case 0x400:
                        //����
                        //���
						DATA_2 = (DATA_2|0x00042000) & 0x00042FFF;
                        if (flag_button == 0x00000010){				//�ж�BTNC�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000010){			//�ж�BTNC�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //������
           	                box();
           	            }
                        break;
                        //
                     case 0x480:
                        //���ʹ�
                        //���
						DATA_2 = (DATA_2|0x0004A000) & 0x0004AFFF;
                        if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //��˳ʱ�룬ת�ٽǶ�Ϊ3
           	                trans(1);
           	            }
           	            else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
           	                DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
           	                delay(200,500,50);					//��ʱԼ1�룬��������������
           	                flag_button = DATA_2_RO & 0x0000001F;
           	                while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
           	    	            flag_button = DATA_2_RO & 0x0000001F;
           	                }
           	                DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
           	                //����ʱ�룬ת�ٽǶ�Ϊ3
           	                trans(2);
           	            }
                        break;


                     case 0x500:
						//���ʹ�

						 if (flag_button == 0x00000004){				//�ж�BTNL�����Ƿ���
								DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
								delay(200,500,50);					//��ʱԼ1�룬��������������
								flag_button = DATA_2_RO & 0x0000001F;
								while(flag_button == 0x00000004){			//�ж�BTNL�����Ƿ�̧��
									flag_button = DATA_2_RO & 0x0000001F;
								}
								DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
								//��˳ʱ�룬ת�ٽǶ�Ϊ3
								for(int i=0;i<18;i++){
									car(3,1);
								}
								car(4,1);

							}
							else if (flag_button == 0x00000008){			//�ж�BTNR�����Ƿ���
								DATA_0 = DATA_0 | 0x00000080;		//LED9ָʾ����
								delay(200,500,50);					//��ʱԼ1�룬��������������
								flag_button = DATA_2_RO & 0x0000001F;
								while(flag_button == 0x00000008){			//�ж�BTNR�����Ƿ�̧��
									flag_button = DATA_2_RO & 0x0000001F;
								}
								DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9ָʾ����
								//����ʱ�룬ת�ٽǶ�Ϊ3
								car(3,2);
							}
						 break;
                        //
               }
				break;
			case 0x40:					//�Զ�����ģʽ
        		DATA_2 = (DATA_2|0x00004000)&0xFFFF7FFF;	//LED7��LED6��
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
							//С�����ƶ�����
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



							//С�����ƶ�����
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
							//����
							delay(1000,500,50);
							for(int i=0;i<13;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//��50
							for(int i=0;i<50;i++){
								car(2,2);

							}
							car(2,0);
							//��10
							for(int i=0;i<10;i++){
								car(1,2);

							}
							car(1,0);
							//��10
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

							//ǰ50
//							for(int i=0;i<50;i++){
//								car(1,1);
//								delay(100,500,50);
//							}
//							car(1,0);
//							//��10
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
							//����
							delay(1000,500,50);
							for(int i=0;i<12;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//��26
							for(int i=0;i<26;i++){
								car(2,2);

							}
							car(2,0);
							//��35
							for(int i=0;i<35;i++){
								car(1,2);

							}
							car(1,0);
							//��10
							for(int i=0;i<10;i++){
								car(2,2);

							}
							car(2,0);
							delay(1000,100,10);
						}
						else if(data=='B'){
							//����
							delay(1000,500,50);
							for(int i=0;i<14;i++){
								car(1,2);
								delay(100,500,50);
							}
							car(1,0);
							//��7
							for(int i=0;i<7;i++){
								car(2,2);

							}
							car(2,0);
							//��50
							for(int i=0;i<50;i++){
								car(1,2);

							}
							car(1,0);
							//��10
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
						//С����ȥ
						if(data=='R'){
							//��10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//ǰ10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);
							//��33
							for(int i=0;i<33;i++){
								car(2,1);

							}
							car(2,0);
							//ǰ10
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
//							//��10
//							for(int i=0;i<10;i++){
//								car(2,1);
//
//							}
//							car(2,0);
//
//							//��33
//							for(int i=0;i<33;i++){
//								car(1,2);
//							}
//							car(1,0);
//							//ǰ10
//							for(int i=0;i<10;i++){
//								car(1,1);
//							}
//							car(1,0);




						}
						else if(data=='G'){
							//��10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//ǰ35
							for(int i=0;i<35;i++){
								car(1,1);

							}
							car(1,0);
							//��33
							for(int i=0;i<33;i++){
								car(2,1);

							}
							car(2,0);
							//ǰ10
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

							//��10
							for(int i=0;i<10;i++){
								car(2,1);

							}
							car(2,0);

							//ǰ50
							for(int i=0;i<50;i++){
								car(1,1);

							}
							car(1,0);
							//��33
							for(int i=0;i<31;i++){
								car(2,1);

							}
							car(2,0);
							//ǰ10
							for(int i=0;i<10;i++){
								car(1,1);

							}
							car(1,0);




						}



        			}


        	}
        		break;
			case 0x60:					//��е��ʾ��ģʽ����ģʽ�ݲ�ʵ�֣�
        		DATA_2 = DATA_2|0x00006000;					//LED7����LED6��
        		break;
        	}
    }
    return 0;
}

//��е����ظ�������������
void arm(int Arm_ID,int Arm_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//��ʼ�����̶�Ϊ#
	modbus_com[1]='2';				//��е�����
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	switch(Arm_ID){
	case 1:						//��һ����
	     if (Arm_dir==0){
		    modbus_com[2]='3';
	     }
	     else if(Arm_dir==1){
	    	modbus_com[2]='7';
	     }else{
	    	 modbus_com[2]='0';
	     }
	     break;
	case 2:						//�ڶ�����
		 if (Arm_dir==0){
		    modbus_com[3]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[3]='7';
		 }else{
			 modbus_com[3]='0';
		 }

	   	 break;
	case 3:						//��������
		 if (Arm_dir==0){
		    modbus_com[4]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[4]='7';
		 }else{
			 modbus_com[4]='0';
		 }
		 break;
	case 4:						//���ĸ���
		 if (Arm_dir==0){
		    modbus_com[5]='3';
		 }
		 else if(Arm_dir==1){
		   	modbus_com[5]='7';
		 }else{
			modbus_com[5]='0';
		 }
	   	 break;
	case 5:						//�������
		 if (Arm_dir==0){
		    modbus_com[6]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[6]='7';
		 }else{
			 modbus_com[6]='0';
		 }
         break;
	case 6:						//��������
		 if (Arm_dir==0){
		    modbus_com[7]='3';
		 }
		 else if(Arm_dir==1){
		    modbus_com[7]='7';
		 }else{
			 modbus_com[7]='0';
		 }
		 break;
	case 7:						//����ϵ��ƶ�
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

//������غ���
void box(void)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//��ʼ�����̶�Ϊ#
	modbus_com[1]='4';				//����
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';
    //2��վ
    modbus_com[3] = '1';
	send_Char_9(modbus_com);
}

//������غ���
void plate(int Plate_ID,int Plate_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//��ʼ�����̶�Ϊ#
	modbus_com[1]='5';				//����
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	switch(Plate_ID){
	case 1:						//1��վ
         if(Plate_dir == 0){
             modbus_com[2]='1';
         }else if(Plate_dir == 1){
             modbus_com[2]='2';
         }
	     break;
	case 2:						//2��վ
         if(Plate_dir == 0){
             modbus_com[3]='1';
         }else if(Plate_dir == 1){
             modbus_com[3]='2';
         }
	   	 break;
	case 3:						//3��վ
          if(Plate_dir == 0){
             modbus_com[4]='1';
         }else if(Plate_dir == 1){
             modbus_com[4]='2';
         }
	}
	send_Char_9(modbus_com);
}

//���ʹ���غ���
void trans(int Trans_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//��ʼ�����̶�Ϊ#
	modbus_com[1]='6';				//���ʹ�
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	//����3�Ŵ��ʹ�
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
	modbus_com[0]='#';				//��ʼ�����̶�Ϊ#
	modbus_com[1]='3';				//���ʹ�
	modbus_com[2]='0';
	modbus_com[3]='0';
	modbus_com[4]='0';
	modbus_com[5]='0';
	modbus_com[6]='0';
	modbus_com[7]='0';
	modbus_com[8]='0';
	switch(id){
		case 1:						//ǰ��
		     if (dir==1){
			    modbus_com[2]='1';
		     }
		     else if(dir==2){
		    	modbus_com[2]='2';
		     }else{
		    	 modbus_com[2]='0';
		     }
		     break;
		case 2:						//����
			 if (dir==1){
			    modbus_com[3]='1';
			 }
			 else if(dir==2){
			    modbus_com[3]='2';
			 }else{
				 modbus_com[3]='0';
			 }

		   	 break;
		case 3:						//��ת
			 if (dir==1){
			    modbus_com[4]='1';
			 }
			 else if(dir==2){
			    modbus_com[4]='2';
			 }else{
				 modbus_com[4]='0';
			 }
			 break;
		case 4:						//����
			 if (dir==1){
			    modbus_com[5]='1';//����
			 }
			 else if(dir==2){
			   	modbus_com[5]='2';//����
			 }else{
				modbus_com[5]='0';
			 }
		   	 break;
		case 5:						//��ɫ����
			 if (dir==1){
			    modbus_com[6]='1';
			 }
			 else if(dir==2){
			    modbus_com[6]='2';
			 }else{
				 modbus_com[6]='0';
			 }
	         break;
		case 6:						//��ɫ����
			 if (dir==1){
			    modbus_com[7]='1';
			 }
			 else if(dir==2){
			    modbus_com[7]='2';
			 }else{
				 modbus_com[7]='0';
			 }
			 break;
		case 7:						//δʹ��

			 break;
		}
	send_Char_9(modbus_com);
}
//9���ֽ����ݵķ��ͺ���
void send_Char_9(unsigned char modbus[])
{
	int i;
	char data;
	for(i=0;i<9;i++){
		data=modbus[i];
		send_Char(data);
		delay(100,10,10);		//��ʱ
	}
}

//�����ֽ����ݵķ��ͺ���
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

//UART1�ĳ�ʼ������
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

//��ʱ����
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
