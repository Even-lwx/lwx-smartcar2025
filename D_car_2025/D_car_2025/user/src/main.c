/*********************************************************************************************************************
 * MM32F327X-G8P Opensourec Library 即（MM32F327X-G8P 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 MM32F327X-G8P 开源库的一部分
 *
 * MM32F327X-G8P 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          main
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          IAR 8.32.4 or MDK 5.37
 * 适用平台          MM32F327X_G8P
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2022-08-10        Teternal            first version
 ********************************************************************************************************************/

#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// *************************** 例程硬件连接说明 ***************************
// 核心板正常供电即可 无需额外连接
// 如果使用主板测试 主板必须要用电池供电

// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程 完成上电
//
// 2.可以看到核心板上两个 LED 呈流水灯状闪烁
//
// 3.将 SWITCH1 / SWITCH2 两个宏定义对应的引脚分别按照 00 01 10 11 的组合接到 1-VCC 0-GND 或者波动对应主板的拨码开关
//
// 3.不同的组合下 两个 LED 流水灯状闪烁的频率会发生变化
//
// 4.将 KEY1 / KEY2 / KEY3 / KEY4 两个宏定义对应的引脚接到 1-VCC 0-GND 或者 按对应按键
//
// 5.任意引脚接 GND 或者 按键按下会使得两个 LED 一起闪烁 松开后恢复流水灯
//
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查

// **************************** 代码区域 ****************************

int32 encoder1;
int32 encoder2;
int8 offset = 0;


#define CONTROL_PIT                         (TIM6_PIT )                                 // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调de
#define CONTROL_IRQn                         (TIM6_IRQn )   
void all_init(void)
{
	clock_init(SYSTEM_CLOCK_120M); // 初始化芯片时钟 工作频率为 120MHz
	debug_init();				   // 初始化默认 Debug UART

	system_delay_ms(300);
	gpio_init(D7, GPO, GPIO_LOW, GPO_PUSH_PULL);//
	menu_init(); // 菜单初始化
	mt9v03x_init();//摄像头初始化
	imu660rb_init();//姿态传感器初始化
	motor_init();	// 电机初始化
	encoder_init(); // 编码器初始化
	pit_ms_init(CONTROL_PIT , 5);//开启控制函数定时器
	interrupt_set_priority(CONTROL_IRQn ,0);
	ips200_clear();

}



int main(void)
{
	all_init();
	
	while (1)
	{
		
		// 运行菜单
		show_process(NULL);
		if (mt9v03x_finish_flag)
    {
        // 拷贝图像数据
        memcpy(image_copy, mt9v03x_image, MT9V03X_H * MT9V03X_W);
				image_output();
				mt9v03x_finish_flag=0;
    }
		system_delay_ms(20);
		
		//camera_send_image(DEBUG_UART_INDEX, (const uint8 *)mt9v03x_image, MT9V03X_IMAGE_SIZE);
		// printf("ENCODER_Left counter \t%d .\r\n", Encoder_Left);                 // 输出编码器计数信息
		 //printf("ENCODER_Right counter \t%d .\r\n", Encoder_Right);                 // 输出编码器计数信息

		//		printf("IMU963RA acc data: x=%5d, y=%5d, z=%5d\n", imu963ra_acc_x, imu963ra_acc_y, imu963ra_acc_z);
		//    printf("IMU963RA gyro data:  x=%5d, y=%5d, z=%5d\n", imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z);
		//		printf("%2f, %2f, %2f\n", roll, pitch, yaw);


	}
}

// **************************** 代码区域 ****************************

// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
//
// 问题1：LED 不闪烁
//      如果使用主板测试，主板必须要用电池供电
//      查看程序是否正常烧录，是否下载报错，确认正常按下复位按键
//      万用表测量对应 LED 引脚电压是否变化，如果不变化证明程序未运行，如果变化证明 LED 灯珠损坏
//
// 问题2：SWITCH1 / SWITCH2 更改组合流水灯频率无变化
//      如果使用主板测试，主板必须要用电池供电
//      查看程序是否正常烧录，是否下载报错，确认正常按下复位按键
//      万用表测量对应 LED 引脚电压是否变化，如果不变化证明程序未运行，如果变化证明 LED 灯珠损坏
//      万用表检查对应 SWITCH1 / SWITCH2 引脚电压是否正常变化，是否跟接入信号不符，引脚是否接错
//
// 问题3：KEY1 / KEY2 / KEY3 / KEY4 接GND或者按键按下无变化
//      如果使用主板测试，主板必须要用电池供电
//      查看程序是否正常烧录，是否下载报错，确认正常按下复位按键
//      万用表测量对应 LED 引脚电压是否变化，如果不变化证明程序未运行，如果变化证明 LED 灯珠损坏
//      万用表检查对应 KEY1 / KEY2 / KEY3 / KEY4 引脚电压是否正常变化，是否跟接入信号不符，引脚是否接错
