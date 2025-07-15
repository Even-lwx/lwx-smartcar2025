#include "zf_common_headfile.h"
#include "flash.h"

extern int ADC_Speed;      //电磁速度
extern int Base_Speed;     //基准速度
extern int Island_Speed;   //环岛速度
extern int Straight_Speed; //直道高速
extern int Ramp_Speed;     //稳定200
extern int Barricade_Speed;//横断速度

extern float Err_Diff;     //差速系数
extern float Shift_Ratio;  //变速系数
extern float Island_Diff;  //环岛差速
extern float Island_Ratio; //环岛变速
extern float Island_P;     //环岛p
extern float Island_D;     //环岛d
extern float Global_D;     //全局d
extern float P_L;          //左轮p
extern float I_L;          //左轮i
extern float P_R;          //右轮p
extern float I_R;          //右轮i

extern uint8 Go_Direction; //0右,1左
extern uint8 Obstacle_Dir; //0右拐，1左拐

/*-------------------------------------------------------------------------------------------------------------------
  @brief     写入flash
  @param     null
  @return    null
  Sample     Write_Flash();
  @note      将各项参数写入flash
-------------------------------------------------------------------------------------------------------------------*/
void  Write_Flash(void)
{
    flash_union_buffer[0].int32_type  =ADC_Speed;
    flash_union_buffer[1].int32_type  =Base_Speed;
    flash_union_buffer[2].int32_type  =Island_Speed;
    flash_union_buffer[3].int32_type  =Straight_Speed;
    flash_union_buffer[4].int32_type  =Ramp_Speed;
    flash_union_buffer[5].int32_type  =Barricade_Speed;

    flash_union_buffer[6].float_type  =Island_Diff;
    flash_union_buffer[7].float_type  =Island_Ratio;
    flash_union_buffer[8].float_type  =Err_Diff;
    flash_union_buffer[9].float_type  =Shift_Ratio;

    flash_union_buffer[10].float_type  =Island_P;
    flash_union_buffer[11].float_type  =Island_D;
    flash_union_buffer[12].float_type  =Global_D;
    flash_union_buffer[13].float_type  =P_L;
    flash_union_buffer[14].float_type  =I_L;
    flash_union_buffer[15].float_type  =P_R;
    flash_union_buffer[16].float_type  =I_R;
    flash_union_buffer[17].uint8_type  =Go_Direction;
    flash_union_buffer[18].uint8_type  =Obstacle_Dir;

    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);        // 向指定 Flash 扇区的页码写入缓冲区数据
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     读取flash
  @param     null
  @return    null
  Sample     Read_Flash();
  @note      将各项参数从flash中读取
-------------------------------------------------------------------------------------------------------------------*/
void  Read_Flash(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);           // 将数据从 flash 读取到缓冲区
    ADC_Speed       =flash_union_buffer[0].int32_type;
    Base_Speed      =flash_union_buffer[1].int32_type;
    Island_Speed    =flash_union_buffer[2].int32_type;
    Straight_Speed  =flash_union_buffer[3].int32_type;
    Ramp_Speed      =flash_union_buffer[4].int32_type;
    Barricade_Speed =flash_union_buffer[5].int32_type;
    Island_Diff     =flash_union_buffer[6].float_type;
    Island_Ratio    =flash_union_buffer[7].float_type;
    Err_Diff        =flash_union_buffer[8].float_type;
    Shift_Ratio     =flash_union_buffer[9].float_type;
    Island_P =flash_union_buffer[10].float_type;
    Island_D =flash_union_buffer[11].float_type;
    Global_D =flash_union_buffer[12].float_type;
    P_L      =flash_union_buffer[13].float_type;
    I_L      =flash_union_buffer[14].float_type;
    P_R      =flash_union_buffer[15].float_type;
    I_R      =flash_union_buffer[16].float_type;
    Go_Direction=flash_union_buffer[17].uint8_type;
    Obstacle_Dir=flash_union_buffer[18].uint8_type;

}

