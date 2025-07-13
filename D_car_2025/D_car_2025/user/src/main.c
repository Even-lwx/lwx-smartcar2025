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
#include "main.h"
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// *************************** 例程硬件连接说明 ***************************
// 核心板正常供电即可 无需额外连接
// 如果使用主板测试 主板必须要用电池供电

// **************************** 代码区域 ****************************

#define CONTROL_PIT (TIM6_PIT)
#define CONTROL_IRQn (TIM6_IRQn)

uint32 system_count = 0;
int threshold;
int image_proess=0;//图像处理完成标志位

void all_init(void)
{
	clock_init(SYSTEM_CLOCK_120M); // 初始化芯片时钟 工作频率为 120MHz
	debug_init();				   // 初始化默认 Debug UART

	menu_init(); // 菜单初始化
	system_delay_ms(300);

	buzzer_init();

	mt9v03x_init(); // 摄像头初始化
	imu_init();		// 姿态传感器初始化
	motor_init();	// 电机初始化
	encoder_init(); // 编码器初始化

	pit_ms_init(CONTROL_PIT, 1); // 开启控制函数定时器
	interrupt_set_priority(CONTROL_IRQn, 0);

	ips200_clear();
	buzzer_on();
	system_delay_ms(100);
	buzzer_off();
	system_delay_ms(100);
	pit_ms_init(TIM2_PIT, 10);
	interrupt_set_priority(TIM2_IRQn, 1); // 菜单的中断
}

int main(void)
{
	all_init();

	while (1)
	{

			

		/*图像处理*/
		if (mt9v03x_finish_flag)
		{

			memcpy(image_copy, mt9v03x_image, MT9V03X_H * MT9V03X_W); // 拷贝图像数据
			if (system_count % 4 == 0)								  // 间隔4次算一次阈值
			{
				threshold = otsu_get_threshold(image_copy, MT9V03X_W, MT9V03X_H); // 图像获取阈值
			}
			applyThreshold(image_copy, binaryImage, threshold); // 应用阈值生成二值化图像

			Longest_White_Column();
			Cross_Detect();
			turn_offset = err_sum_average(TURN_STANDARD_START, TURN_STANDARD_END); // 转向偏差（左正右负）
			system_count++;
			image_proess =1;
			mt9v03x_finish_flag = 0;
		}

		/*debug区域*/
		//  printf("ENCODER_Left counter \t%d .\r\n", Encoder_Left);                 // 输出编码器计数信息
		// printf("ENCODER_Right counter \t%d .\r\n", Encoder_Right);                 // 输出编码器计数信息

		// printf("%d,%d,%d,%d,%d,%d\r\n", gx, gy, gz, ax, ay, az);
		;
		//	printf("%d,%d,%.2f\r\n",gx,ay,filtering_angle );
		//printf("%2f\r\n",filtering_angle);
		//		system_delay_ms(20);
	}
}

// **************************** 代码区域 ****************************
