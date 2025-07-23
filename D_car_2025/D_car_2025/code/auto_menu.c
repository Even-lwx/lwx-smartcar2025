

#include "zf_common_headfile.h"
#include "auto_menu.h"
#include "key.h"
#include "zf_driver_flash.h" // �������ṩ��Flash����ͷ�ļ�

// �����ź��������������ź���
#ifdef MENU_USE_RTT
extern rt_sem_t key1_sem;
extern rt_sem_t key2_sem;
extern rt_sem_t key3_sem;
extern rt_sem_t key4_sem;
extern rt_sem_t button_feedback_sem;
#endif
/*-------------------����--------------------
                    button1����
                    button2ȷ��
                    button3�·�
                    button4�Ϸ�
---------------------------------------------*/
uint8 button1 = 0, button2 = 0, button3 = 0, button4 = 0;
uint8 first_in_page_flag = 0;
uint8 is_clear_flag = 0;

uint8 *p_index_xy_dad, *p_index_xy_son;

static menu_unit *p_unit = NULL;      // ��Ԫָ��
static menu_unit *p_unit_last = NULL; // ��һ�εĵ�Ԫָ��

static menu_unit *P_dad_head = NULL;

uint8 DAD_NUM = 1;

#ifdef USE_STATIC_MENU
menu_unit my_menu_unit[MEM_SIZE];
param_set my_param_set[MEM_SIZE];
uint8 my_index[MEM_SIZE * 2];
static int static_cnt = 0;
#endif

// ��������ָ��
void (*current_operation_menu)(void);

// ���ȫ�ֱ���������ʾģʽ
uint8_t display_mode = 0; // 0:�����˵�, 1:��ʾͼ��, 2:����������, 3:��������

// ���������ݽṹ��
typedef struct
{
    float temperature;
    float humidity;
    float pressure;
    uint16_t light_level;
    uint16_t distance;
} SensorData;

static SensorData sensor_data = {0};

// ��������״̬����
static uint8_t launch_state = 0;
static uint32_t last_time = 0;
static uint8_t countdown = 5;

// Flash�����洢λ��
#define PARAM_FLASH_SECTOR 127
#define PARAM_FLASH_PAGE 0

void dad_name_init()
{
    char *p = NULL;
    p = (char *)malloc(STR_LEN_MAX);
    memset(p, 0, STR_LEN_MAX);
    strcpy(p, "Page ");
    DAD_NUM = IND1 + 1;
    for (uint8 i = 0; i < DAD_NUM; i++)
    {
        if (i < 10)
        {
            p[5] = '0' + i;
            strcpy(P_dad_head->name, p);
        }
        else
        {
            p[5] = '0' + i / 10;
            p[6] = '0' + i % 10;
            strcpy(P_dad_head->name, p);
        }
        P_dad_head = P_dad_head->up;
    }
    free(p);
}

// �Ӳ˵���Ԫ����
void son_link(menu_unit *p_1, menu_unit *p_2, menu_unit *dad)
{
    p_1->up = p_2;
    p_2->down = p_1;
    p_1->enter = p_1;
    p_2->enter = p_2;
    p_1->back = dad;
    p_2->back = dad;
}

// ���˵���Ԫ����
void dad_link(menu_unit *p_1, menu_unit *p_2, menu_unit *son)
{
    p_1->up = p_2;
    p_2->down = p_1;
    p_1->back = p_1;
    p_2->back = p_2;
    if (son != NULL)
        p_1->enter = son;
}
void unit_default(menu_unit *p, uint8 ind)
{
    p->type_t = USE_FUN;
    p->current_operation = NULL_FUN;
    p->m_index[0] = ind;
    p->m_index[1] = 255;
}
void unit_index_init(menu_unit *_p1, uint8 ind_0, uint8 ind_1)
{
    _p1->m_index[0] = ind_0;
    _p1->m_index[1] = ind_1;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���޸Ĳ�������
//  @param      p_param     	        ���޸Ĳ���ָ��
//  @param      t     	        		���޸Ĳ�����������
//  @param      delta       	    	����ÿ����һ�Σ��������޸ĵĴ�С
//  @param      num     	  		 		������������ʾλ��
//  @param      point_num     	 		������С����ʾλ��
//  @param      t1     	 						��Ԫ����
//  @param      _name[STR_LEN_MAX]  ��������
//  @return     void
//  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
//-------------------------------------------------------------------------------------------------------------------
void unit_param_set(void *p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX])
{
    static menu_unit *p_middle = NULL;
    menu_unit *p1 = NULL, *p2 = NULL;
    param_set *p1_par = NULL, *p2_par = NULL;
    static menu_unit *dad;

#ifdef USE_STATIC_MENU
    p1 = my_menu_unit + static_cnt;
    p1_par = my_param_set + static_cnt;
    static_cnt++;
    p2 = my_menu_unit + static_cnt;
    p2_par = my_param_set + static_cnt;
    static_cnt++;
#else
    p1 = malloc(sizeof(menu_unit));
    p1_par = malloc(sizeof(param_set));

    p2 = malloc(sizeof(menu_unit));
    p2_par = malloc(sizeof(param_set));
#endif

    if (P_dad_head == NULL)
    {
#ifdef USE_STATIC_MENU
        dad = my_menu_unit + static_cnt;
        static_cnt++;
#else
        dad = malloc(sizeof(menu_unit));
#endif
        P_dad_head = dad;
        unit_default(dad, 0);
        p_unit = dad;
        p_unit_last = NULL;
        unit_index_init(p1, 0, 0);
        unit_index_init(p2, 0, 1);
        dad_link(dad, dad, p1);
        son_link(p1, p2, dad);
        son_link(p2, p1, dad);
    }
    else
    {
        if (IND2 >= SON_NUM - 2)
        {
#ifdef USE_STATIC_MENU
            dad = my_menu_unit + static_cnt;
            static_cnt++;
#else
            dad = malloc(sizeof(menu_unit));
#endif
            unit_default(dad, IND1 + 1);
            unit_index_init(p1, IND1 + 1, 0);
            unit_index_init(p2, IND1 + 1, 1);
            dad_link(P_dad_head->down, dad, NULL);
            dad_link(dad, P_dad_head, p1);
            son_link(p1, p2, dad);
            son_link(p2, p1, dad);
        }
        else
        {
            unit_index_init(p1, IND1, IND2 + 1);
            unit_index_init(p2, IND1, IND2 + 2);
            son_link(p_middle, p1, dad);
            son_link(p1, p2, dad);
            son_link(p2, SON_BEGIN_UNIT, dad);
        }
    }
    p_middle = p2;

    p1->par_set = p1_par;
    p1->par_set->p_par = p_param;
    p1->par_set->par_type = t;
    p1->par_set->delta = delta;
    p1->par_set->num = num;
    p1->par_set->point_num = point_num;

    p1->type_t = t1;
    memset(p1->name, 0, STR_LEN_MAX);
    strcpy(p1->name, _name);
    p1->name[strlen(_name)] = '+';

    p2->par_set = p2_par;
    p2->par_set->p_par = p_param; // ��p1ָ��ͬһ������
    p2->par_set->par_type = t;
    p2->par_set->delta = -delta;
    p2->par_set->num = num;
    p2->par_set->point_num = point_num;
    p2->type_t = t1;
    memset(p2->name, 0, STR_LEN_MAX);
    strcpy(p2->name, _name);
    p2->name[strlen(_name)] = '-';
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ʼ�������˵���Ԫ
//  @param      fun     	   				�败���ĺ���ָ��
//  @param      _name       	  		��ʾ���ַ���
//  @return     void
//  Sample usage:               		fun_init(Flash_Read,"flash_read");
//-------------------------------------------------------------------------------------------------------------------
void fun_init(void *fun, const char *_name)
{
    menu_unit *p2 = NULL;
    menu_unit *p1 = NULL;

#ifdef USE_STATIC_MENU
    p1 = my_menu_unit + static_cnt;
    static_cnt++;
#else
    p1 = malloc(sizeof(menu_unit));
#endif
    p1->par_set = NULL;

    if (IND2 >= SON_NUM - 1)
    {
#ifdef USE_STATIC_MENU
        p2 = my_menu_unit + static_cnt;
        static_cnt++;
#else
        p2 = malloc(sizeof(menu_unit));
#endif

        unit_default(p2, IND1 + 1);
        unit_index_init(p1, IND1 + 1, 0);
        dad_link(P_dad_head->down, p2, NULL);
        dad_link(p2, P_dad_head, p1);
        son_link(p1, p1, p2);
    }
    else
    {
        unit_index_init(p1, IND1, IND2 + 1);
        son_link(SON_END_UNIT, p1, P_dad_head->down);
        son_link(p1, SON_BEGIN_UNIT, P_dad_head->down);
    }

    p1->current_operation = fun;

    for (uint8 i = 0; i < STR_LEN_MAX; i++)
        p1->name[i] = 0;
    strcpy(p1->name, _name);
    p1->type_t = USE_FUN;
}

// �ַ���������ʼ��
void index_xy_init()
{
#ifdef USE_STATIC_MENU
    p_index_xy_dad = my_index;
    p_index_xy_son = my_index + DAD_NUM * 2;
#else
    p_index_xy_dad = (uint8 *)malloc(sizeof(uint8) * DAD_NUM * 2);
    p_index_xy_son = (uint8 *)malloc(sizeof(uint8) * SON_NUM * 2);
#endif
    uint8 half;
    half = (DAD_NUM + 1) / 2;
    for (uint8 i = 0; i < half; i++)
    {
        DAD_INDEX(i, 0) = DIS_X * 0;
        DAD_INDEX(i, 1) = DIS_Y * i;
    }
    for (uint8 i = half; i < DAD_NUM; i++)
    {
        DAD_INDEX(i, 0) = DIS_X * 1;
        DAD_INDEX(i, 1) = DIS_Y * (i - half);
    }
    for (uint8 i = 0; i < SON_NUM; i++)
    {
        SON_INDEX(i, 0) = DIS_X * 0;
        SON_INDEX(i, 1) = DIS_Y * i;
    }
}

// �������в�����Flash
void flash_save_parameters()
{
#ifdef USE_FLASH
    menu_unit *p = P_dad_head->enter;
    uint32_t index = 0;

    // ��ջ�����
    flash_buffer_clear();

    while (1)
    {
        if (p->type_t == NORMAL_PAR || p->type_t == PID_PAR)
        {
            // ���ݲ������ͱ���
            switch (p->par_set->par_type)
            {
            case TYPE_FLOAT:
                flash_union_buffer[index].float_type = *(float *)p->par_set->p_par;
                break;
            case TYPE_INT:
                flash_union_buffer[index].int32_type = *(int *)p->par_set->p_par;
                break;
            case TYPE_UINT16:
                flash_union_buffer[index].uint16_type = *(uint16_t *)p->par_set->p_par;
                break;
            case TYPE_UINT32:
                flash_union_buffer[index].uint32_type = *(uint32_t *)p->par_set->p_par;
                break;
            default:
                // Ĭ�ϰ�uint32����
                flash_union_buffer[index].uint32_type = *(uint32_t *)p->par_set->p_par;
                break;
            }
            index++;

            // ��黺�����Ƿ�����
            if (index >= FLASH_DATA_BUFFER_SIZE)
            {
                break;
            }
        }

        if (p->m_index[1] < p->up->m_index[1])
        {
            p = p->up;
        }
        else
        {
            p = p->back->up->enter;
        }

        // �����������������
        if (p->type_t == USE_FUN)
            break;
    }

    // ������������д��Flash
    flash_write_page_from_buffer(PARAM_FLASH_SECTOR, PARAM_FLASH_PAGE);

    // ��ʾ����ɹ���ʾ
    showstr(0, (SON_NUM + 1) * DIS_Y, "Params Saved!");
#endif
}

//// ��Flash�������в���
void flash_load_parameters()
{
#ifdef USE_FLASH
    // ���Flash���Ƿ�������
    if (flash_check(PARAM_FLASH_SECTOR, PARAM_FLASH_PAGE))
    {
        // ��Flash��ȡ���ݵ�������
        flash_read_page_to_buffer(PARAM_FLASH_SECTOR, PARAM_FLASH_PAGE);

        menu_unit *p = P_dad_head->enter;
        uint32_t index = 0;

        while (1)
        {
            if (p->type_t == NORMAL_PAR || p->type_t == PID_PAR)
            {
                // ���ݲ������ͼ���
                switch (p->par_set->par_type)
                {
                case TYPE_FLOAT:
                    *(float *)p->par_set->p_par = flash_union_buffer[index].float_type;
                    break;
                case TYPE_INT:
                    *(int *)p->par_set->p_par = flash_union_buffer[index].int32_type;
                    break;
                case TYPE_UINT16:
                    *(uint16_t *)p->par_set->p_par = flash_union_buffer[index].uint16_type;
                    break;
                case TYPE_UINT32:
                    *(uint32_t *)p->par_set->p_par = flash_union_buffer[index].uint32_type;
                    break;
                default:
                    // Ĭ�ϰ�uint32����
                    *(uint32_t *)p->par_set->p_par = flash_union_buffer[index].uint32_type;
                    break;
                }
                index++;

                // ����Ƿ��Ѷ�ȡ���б���Ĳ���
                if (index >= FLASH_DATA_BUFFER_SIZE)
                {
                    break;
                }
            }

            if (p->m_index[1] < p->up->m_index[1])
            {
                p = p->up;
            }
            else
            {
                p = p->back->up->enter;
            }

            // �����������������
            if (p->type_t == USE_FUN)
                break;
        }

        // ��ʾ���سɹ���ʾ
        showstr(10, 100, "Params Loaded!");
    }
#endif
}

void flash_init_wz()
{
// ��ʼ����Ҫ����Ĳ���  ������˳���ʼ����ȥ��ע�⣺��data_flash.h��������������
#if AUTO_READ
    menu_unit *p;
    p = P_dad_head->enter;
    while (1)
    {
        if (p->m_index[1] < p->up->m_index[1])
        {
            flash_unit_init(p->par_set.p_par, p->par_set.par_type);
            p = p->up;
        }
        else
        {
            flash_unit_init(p->par_set.p_par, p->par_set.par_type);
            p = p->back->up->enter;
        }
        if (p->par_set.type_t == USE_FUN)
            break;
    }
#endif
// index��ʼ�������ù�
// �ϵ��ȡ����
#if AUTO_READ
    flash_index_init();
    flash_read();
#endif
}

// �Ƿ������Ļ
uint8 is_menu_clear()
{
    return (((p_unit->back) != (p_unit)) && button1) || (((p_unit->enter) != (p_unit)) && button2);
}
uint8 begin_menu_flag = 1;
// �����˵���ʾ
void center_menu()
{
    uint8 index = p_unit->m_index[0];

    if (first_in_page_flag)
        showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);

    if (button3 || button4)
    {
        if (index == 0)
        {
            showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), " ");
            showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), MOUSE_LOOK);
            showstr(DAD_INDEX(1, 0), DAD_INDEX(1, 1), " ");
        }
        else if (index == DAD_NUM - 1)
        {
            showstr(DAD_INDEX(DAD_NUM - 2, 0), DAD_INDEX(DAD_NUM - 2, 1), " ");
            showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), MOUSE_LOOK);
            showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), " ");
        }
        else
        {
            showstr(DAD_INDEX(index - 1, 0), DAD_INDEX(index - 1, 1), " ");
            showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);
            showstr(DAD_INDEX(index + 1, 0), DAD_INDEX(index + 1, 1), " ");
        }
    }
    else if ((is_clear_flag == 1 && (button1)) || (begin_menu_flag))
    {
        menu_unit *p = NULL;
        p = p_unit;
        for (uint8 i = 0; i < DAD_NUM; i++)
        {
            showstr(DAD_INDEX(p->m_index[0], 0) + MOUSE_DIS, DAD_INDEX(p->m_index[0], 1), p->name);
            p = p->up;
        }
        begin_menu_flag = 0;
    }
}

// �Ӽ��˵���ʾ
void assist_menu()
{
    uint8 index = p_unit->m_index[1];
    if (first_in_page_flag)
        showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);

    if (button3 || button4)
    {
        if (index == 0)
        {
            showstr(SON_INDEX(p_unit->down->m_index[1], 0), SON_INDEX(p_unit->down->m_index[1], 1), " ");
            showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), MOUSE_LOOK);
            showstr(SON_INDEX(1, 0), SON_INDEX(1, 1), " ");
        }
        else if (index == p_unit->back->enter->down->m_index[1])
        {
            showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
            showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
            showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), " ");
        }
        else
        {
            showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
            showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
            showstr(SON_INDEX(index + 1, 0), SON_INDEX(index + 1, 1), " ");
        }
    }
    else if (is_clear_flag == 1 && (button2))
    {
        menu_unit *p = NULL;
        p = p_unit;
        for (uint8 i = 0; i < SON_NUM; i++)
        {
            showstr(SON_INDEX(p->m_index[1], 0) + MOUSE_DIS, SON_INDEX(p->m_index[1], 1), p->name);
            p = p->up;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		�޸Ĳ�����С
// @param		param  	 	����
// @return	void
// Sample usage:				change_value(TYPE_FLOAT,0.01,&P_speed);button2����ʱ�޸�P_speed��ֵΪP_speed+0.01
// attention	 :				ע����޸Ĳ��������ͣ�TYPE_DOUBLE,TYPE_FLOAT,TYPE_INT...�������Ϳ�������ӣ�
//-------------------------------------------------------------------------------------------------------------------
void change_value(param_set *param)
{
    uint8 type = param->par_type;
    float delta_x = param->delta;
    void *value = param->p_par;
    uint8 num = param->num;
    uint8 point_num = param->point_num;
    static uint8 last_index = 0;
    uint8 is_show_num = (p_unit_last->par_set == NULL ? 1 : p_unit_last->par_set->p_par != p_unit->par_set->p_par);
    if (p_unit->par_set->p_par != NULL)
    {
        if (type == TYPE_FLOAT)
        {
            float *p_value;
            p_value = (float *)(value);
            if (IS_OK)
            {
                *p_value += delta_x;
                showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
            }
            if (is_show_num)
            {
                showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
                showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
            }
        }
        else if (type == TYPE_DOUBLE)
        {
            double *p_value;
            p_value = (double *)(value);
            if (IS_OK)
            {
                *p_value += (double)delta_x;
                showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
            }
            if (is_show_num)
            {
                showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
                showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
            }
        }
        else if (type == TYPE_INT)
        {
            int *p_value;
            p_value = (int *)(value);
            if (IS_OK)
            {
                *p_value += (int)delta_x;
                showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
            if (is_show_num)
            {
                showstr(0, (SON_NUM + 1) * DIS_Y, "        ");
                showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
        }
        else if (type == TYPE_UINT16)
        {
            uint16 *p_value;
            p_value = (uint16 *)(value);
            if (IS_OK)
            {
                *p_value += (int)delta_x;
                showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
            if (is_show_num)
            {
                showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
                showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
        }
        else if (type == TYPE_UINT32)
        {
            uint32 *p_value;
            p_value = (uint32 *)(value);
            if (IS_OK)
            {
                *p_value += (int)delta_x;
                showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
            if (is_show_num)
            {
                showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
                showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
            }
        }
    }
    last_index = p_unit->m_index[1];
}

// �Ƿ�Ϊ��һ�ν�����ҳ��
void is_first_in_page()
{
    first_in_page_flag = (p_unit_last != p_unit) && (button1 || button2);
}

void show_menu()
{
    if (p_unit->m_index[1] == 255)
        center_menu(); // ��ʾ���˵�
    else
        assist_menu(); // ��ʾ�Ӳ˵�
}

// Ч������
void fun_menu()
{
    if (p_unit->type_t == NORMAL_PAR || p_unit->type_t == PID_PAR)
    {
        change_value(p_unit->par_set);
    }
    else
    {
        current_operation_menu = p_unit->current_operation; // ����ָ��
        (*current_operation_menu)();
    }
}

// ͼ����ʾ����
void handle_image_display()
{
	int display_image=0;
    // ���Ʋο���λ��
    ips200_draw_line(MT9V03X_W / 2, 0, MT9V03X_W / 2, MT9V03X_H, RGB565_YELLOW);

    ips200_draw_line(0, TURN_STANDARD_START, MT9V03X_W, TURN_STANDARD_START, RGB565_YELLOW);
    ips200_draw_line(0, TURN_STANDARD_END, MT9V03X_W, TURN_STANDARD_END, RGB565_YELLOW);

    if (image_proess)
    {

               // ��ʾԭʼͼ��
       if(display_image ==1)
			 {				 
			ips200_show_gray_image(0, 0, (const uint8 *)image_copy,
                                      MT9V03X_W, MT9V03X_H,
                                      MT9V03X_W, MT9V03X_H, 0);
		}
        // ��ʾ�����Ķ�ֵ��ͼ��
			if(display_image ==0)
			{
        ips200_show_gray_image(0, 0, (const uint8 *)binaryImage,
                               MT9V03X_W, MT9V03X_H,
                               MT9V03X_W, MT9V03X_H, 0);
			}
        // ��ʾ�߽������ͼ��
        for (int i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--) // �����������ɨ��
        {
            // ���ұ߽�ʹ�û�ɫ
            ips200_draw_point(Left_Line[i] + 1, i, RGB565_RED);
            ips200_draw_point(Right_Line[i] - 1, i, RGB565_RED);
            // ����ʹ����ɫ
            ips200_draw_point((Left_Line[i] + Right_Line[i]) >> 1, i, RGB565_BLUE);
        }
        // ��ʾ�����
        ips200_draw_line(Longest_White_Column_Left[1], 0, Longest_White_Column_Left[1], MT9V03X_H, RGB565_GREEN);
        ips200_draw_line(Longest_White_Column_Right[1], 0, Longest_White_Column_Right[1], MT9V03X_H, RGB565_GREEN);
        ips200_show_string(30, 130, "Cross_Flag");
        ips200_show_int(140, 130, Cross_Flag, 2);
        ips200_show_string(30, 160, "right_circle");
        ips200_show_int(140, 160, right_circle_flag, 2);
        ips200_show_string(30, 190, "Ramp_Flag");
        ips200_show_int(140, 190, Ramp_Flag, 2);

        image_proess = 0;
    }
    // ��ʾ�˳���ʾ
    ips200_show_string(10, 240, "Press BACK to exit");

    // ��ⷵ�ؼ�
    if (button1)
    {
        display_mode = 0;  // �˳�ͼ��ģʽ
        is_clear_flag = 1; // ����������־
        assist_menu();     // ˢ�²˵���ʾ
    }
		if(button2)
		{
			
		display_image ++;
			display_image%=2;
		}
}

// ������������ʾ����
void handle_sensor_display()
{
    // ���´��������ݣ�����ʹ��ģ�����ݣ�

    // ��������ʾ����

    ips200_show_string(10, 10, "Sensor Parameters");
    ips200_draw_line(10, 30, 230, 30, RGB565_BLUE);
    ips200_show_string(0, 50, "roll:");
    // ips200_show_string(0, 70, "pitch:");
    // ips200_show_string(0, 90, "yaw:");
    ips200_show_string(0, 70, "Left:");
    ips200_show_string(0, 90, "Right:");
    ips200_show_float(90, 50, filtering_angle, 4, 4);
    // ips200_show_float(60, 70, pitch, 4, 4);
    // ips200_show_float(60, 90, yaw, 4, 4);
    ips200_show_float(90, 70, Encoder_Left, 4, 4);
    ips200_show_float(90, 90, Encoder_Right, 4, 4);

    ips200_show_float(90, 110, imu963ra_gyro_z, 4, 4);
    // ��ʾ�˳���ʾ
    ips200_show_string(10, 240, "Press BACK to exit");

    // ��ⷵ�ؼ�
    if (button1)
    {
        display_mode = 0;  // �˳���������ʾģʽ
        is_clear_flag = 1; // ����������־
        assist_menu();     // ˢ�²˵���ʾ
    }
}

// ����������
void handle_launch_program()
{
    static uint32_t counter = 0;
    static bool display_updated = false; // ��������ʾ���±�־

    switch (launch_state)
    {
    case 0: // ׼���׶�
        // ֻ�е���ʾδ���»�״̬�仯ʱ��ˢ����Ļ
        if (!display_updated)
        {
            ips200_clear();
            ips200_show_string(10, 10, "Launch Program");
            ips200_draw_line(10, 30, 230, 30, RGB565_RED);
            ips200_show_string(15, 50, "Ready to launch");
            ips200_show_string(15, 80, "Press OK to start");
            ips200_show_string(15, 110, "offset:");

            display_updated = true; // �����ʾ�Ѹ���
        }
        ips200_show_int(80, 110, turn_offset, 3);
        if (button2)
        {
            launch_state = 1;
            car_run = 1; // ������־λ
            right_circle_flag = 0;
            pid_count = 0;
            display_updated = false; // ������ʾ��־��������һ״̬����ʾ����
        }
        break;

    case 1: // �����׶�
        // ֻ�е���ʾδ���»�״̬�仯ʱ��ˢ����Ļ
        if (!display_updated)
        {
            ips200_clear();
            ips200_show_string(10, 10, "Launch Program");
            ips200_draw_line(10, 30, 230, 30, RGB565_GREEN);
            ips200_show_string(15, 50, "Launching!");
            display_updated = true; // �����ʾ�Ѹ���
        }

        // �������п��ƴ�����Է�������
        // ע�⣺���ﲻ�����κ���Ļˢ�²���

        break;
    }

    // ��ʾ�˳���ʾ - ֻ����׼���׶β���Ҫ��ʾ
    if (launch_state == 0 && !display_updated)
    {
        ips200_show_string(10, 240, "Press BACK to exit");
    }

    // ��ⷵ�ؼ�
    if (button1)
    {
        car_run = 0;
        zebra_count = 0;
        speed_over = 0;
        pid_count = 0;
			
			
        display_mode = 0;        // �˳�����ģʽ
        launch_state = 0;        // ����״̬
        is_clear_flag = 1;       // ����������־
        display_updated = false; // ������ʾ��־

        // �������ֹͣ�����Ŀ��ƴ���
        // ���磺motor_control(0); // ֹͣ���

        assist_menu(); // ˢ�²˵���ʾ
    }
}

// ��ʾ���̹��Ӻ���
void show_process(void *parameter)
{
#ifdef MENU_USE_RTT
    while (1)
    {
        // ��ȡ�����ź���
        button1 = (RT_EOK == rt_sem_take(key1_sem, RT_WAITING_NO));
        button2 = (RT_EOK == rt_sem_take(key2_sem, RT_WAITING_NO));
        button3 = (RT_EOK == rt_sem_take(key3_sem, RT_WAITING_NO));
        button4 = (RT_EOK == rt_sem_take(key4_sem, RT_WAITING_NO));

        // ���ݵ�ǰģʽִ�в�ͬ����
        switch (display_mode)
        {
        case 1: // ͼ����ʾģʽ
            handle_image_display();
            break;

        case 2: // ������������ʾģʽ
            handle_sensor_display();
            break;

        case 3: // ��������ģʽ
            handle_launch_program();
            break;

        default: // �����˵�ģʽ
            is_clear_flag = is_menu_clear();

            if (button1 || button2 || button3 || button4)
            {
                rt_sem_release(button_feedback_sem);
            }

            if (is_clear_flag)
                clear_hhh(0, 0, SCREEN_W, SON_NUM * 16, IPS200_BGCOLOR);

            if (button1 == 1)
                p_unit = p_unit->back;
            else if (button2 == 1)
                p_unit = p_unit->enter;
            else if (button3 == 1)
                p_unit = p_unit->up;
            else if (button4 == 1)
                p_unit = p_unit->down;

            is_first_in_page();

            // ��ʾ����
            show_menu();

            // Ч������
            fun_menu();

            p_unit_last = p_unit;
            break;
        }

        // �̹߳���
        rt_thread_mdelay(10);
    }
#else
    // ��������µİ�������
    button_entry(NULL);

    // ���ݵ�ǰģʽִ�в�ͬ����
    switch (display_mode)
    {
    case 1: // ͼ����ʾģʽ
        handle_image_display();
        break;

    case 2: // ������������ʾģʽ
        handle_sensor_display();
        break;

    case 3: // ��������ģʽ
        handle_launch_program();
        break;

    default: // �����˵�ģʽ
        is_clear_flag = is_menu_clear();

        if (is_clear_flag)
            clear();

        if (button1 == 1)
            p_unit = p_unit->back;
        else if (button2 == 1)
            p_unit = p_unit->enter;
        else if (button3 == 1)
            p_unit = p_unit->up;
        else if (button4 == 1)
            p_unit = p_unit->down;

        is_first_in_page();

        // ��ʾ����
        show_menu();

        // Ч������
        fun_menu();

        p_unit_last = p_unit;
        break;
    }
#endif
}

// �˵�ϵͳ��ʼ��(��˳��)
void menu_init()
{
    /*---------------��Ļ��ʼ��----------------*/
    screen_init(IPS200_TYPE_SPI);

    /*---------------������ʼ��----------------*/
    key_into();

    /*---------------���޸Ĳ���----------------*/
    UNIT_SET();

    /*---------------����Ļص�����----------------*/
    FUN_INIT();

    /*---------------�˵����ֳ�ʼ��----------------*/
    dad_name_init();

    /*---------------�ַ���������ʼ��----------------*/
    index_xy_init();

/*-----------------����flash---------------*/
#ifdef USE_FLASH
    flash_init_wz();
    flash_load_parameters(); // �ϵ���ز���
#endif

/*----------------�˵��̳߳�ʼ��----------------*/
#ifdef MENU_USE_RTT
    rt_thread_t tid;
    // ������ʾ�߳�
    tid = rt_thread_create("display", show_process, RT_NULL, 1024 * 2, 11, 5);
    // ������ʾ�߳�
    if (RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
#endif
}

/*
�������û��Լ����Զ��庯��
1.�˵�����ͼ����ʾ�ͳ��������ʾ����
*/

void photo_display()
{
    if (IS_OK)
    {
        display_mode = 1; // ����ͼ����ʾģʽ
        ips200_clear();   // ����
    }
}

// ���봫����������ʾ����
void enter_sensor_display()
{
    if (IS_OK)
    {
        display_mode = 2; // ���봫������ʾģʽ
        ips200_clear();   // ����
    }
}

// ���뷢���������
void enter_launch_program()
{
    if (IS_OK)
    {
        display_mode = 3; // ���뷢������ģʽ
        ips200_clear();   // ����
    }
}

void NULL_FUN()
{
}
extern float Gyro_Kp, Gyro_Ki ;
extern float Vertical_Kp , Vertical_Kd ; // ֱ����
extern float Turn_kp1, Turn_kp2, Turn_kd1, Turn_kd2;
extern int turn_start;
extern int turn_end;

extern float Velocity_Kp;
extern int Target_Speed;

extern int zebracount; // Ԫ�������˵�����

void UNIT_SET()
{
    // �˵���Ԫ���β�����ʼ��
		unit_param_set(& Gyro_Kp, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Gyro_Kp");
    unit_param_set(&Gyro_Ki, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Gyro_Ki");
    unit_param_set(&Vertical_Kp, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Vertical_Kp");
    unit_param_set(&Vertical_Kd, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Vertical_Kd");
		unit_param_set(&Velocity_Kp, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Speed_kp");
    unit_param_set(&Target_Speed, TYPE_INT, 1, 3, 3, NORMAL_PAR, "SPEED");
	
    unit_param_set(&Turn_kp1, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Turn_kp1");
    unit_param_set(&Turn_kp2, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Turn_kp2");
    unit_param_set(&Turn_kd1, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Turn_kd1");
    unit_param_set(&Turn_kd2, TYPE_FLOAT, 0.001, 3, 3, NORMAL_PAR, "Turn_kd2");
    unit_param_set(&turn_start, TYPE_INT, 1, 3, 3, NORMAL_PAR, "turn_start");
    unit_param_set(&turn_end, TYPE_INT, 1, 3, 3, NORMAL_PAR, "turn_end");
    unit_param_set(&zebracount, TYPE_INT, 1, 3, 3, NORMAL_PAR, "zebracount");
    
}

void FUN_INIT()
{
    // �˵���Ԫ����ָ���ʼ��
    fun_init(NULL_FUN, "NULL_FUN1");
    fun_init(NULL_FUN, "NULL_FUN2");
    fun_init(photo_display, "Image_dis");
    fun_init(enter_sensor_display, "Data");      // ������������ʾ���
    fun_init(enter_launch_program, "Launch");    // �����������
    fun_init(flash_save_parameters, "Save_Par"); // �����������
}

/*�����������汾�Ĳ˵�*/
/*
���ǳ�ʼ�ĵ�һ����򣬽����Զ���ĺ����ɿ��Բ���
*/

// #include "zf_common_headfile.h"
// #include "auto_menu.h"
// #include "key.h"

//// �����ź��������������ź���
// #ifdef MENU_USE_RTT
// extern rt_sem_t key1_sem;
// extern rt_sem_t key2_sem;
// extern rt_sem_t key3_sem;
// extern rt_sem_t key4_sem;
// extern rt_sem_t button_feedback_sem;
// #endif
///*-------------------����--------------------
//					button1����
//					button2ȷ��
//					button3�·�
//					button4�Ϸ�
//---------------------------------------------*/
// uint8 button1 = 0, button2 = 0, button3 = 0, button4 = 0;
// uint8 first_in_page_flag = 0;
// uint8 is_clear_flag = 0;

// uint8 *p_index_xy_dad, *p_index_xy_son;

// static menu_unit *p_unit = NULL;	  // ��Ԫָ��
// static menu_unit *p_unit_last = NULL; // ��һ�εĵ�Ԫָ��

// static menu_unit *P_dad_head = NULL;

// uint8 DAD_NUM = 1;

// #ifdef USE_STATIC_MENU
// menu_unit my_menu_unit[MEM_SIZE];
// param_set my_param_set[MEM_SIZE];
// uint8 my_index[MEM_SIZE * 2];
// static int static_cnt = 0;
// #endif

//// ��������ָ��
// void (*current_operation_menu)(void);

// void dad_name_init()
//{
//	char *p = NULL;
//	p = (char *)malloc(STR_LEN_MAX);
//	memset(p, 0, STR_LEN_MAX);
//	strcpy(p, "Page ");
//	DAD_NUM = IND1 + 1;
//	for (uint8 i = 0; i < DAD_NUM; i++)
//	{
//		if (i < 10)
//		{
//			p[5] = '0' + i;
//			strcpy(P_dad_head->name, p);
//		}
//		else
//		{
//			p[5] = '0' + i / 10;
//			p[6] = '0' + i % 10;
//			strcpy(P_dad_head->name, p);
//		}
//		P_dad_head = P_dad_head->up;
//	}
//	free(p);
// }

//// �Ӳ˵���Ԫ����
// void son_link(menu_unit *p_1, menu_unit *p_2, menu_unit *dad)
//{
//	p_1->up = p_2;
//	p_2->down = p_1;
//	p_1->enter = p_1;
//	p_2->enter = p_2;
//	p_1->back = dad;
//	p_2->back = dad;
// }

//// ���˵���Ԫ����
// void dad_link(menu_unit *p_1, menu_unit *p_2, menu_unit *son)
//{
//	p_1->up = p_2;
//	p_2->down = p_1;
//	p_1->back = p_1;
//	p_2->back = p_2;
//	if (son != NULL)
//		p_1->enter = son;
// }
// void unit_default(menu_unit *p, uint8 ind)
//{
//	p->type_t = USE_FUN;
//	p->current_operation = NULL_FUN;
//	p->m_index[0] = ind;
//	p->m_index[1] = 255;
// }
// void unit_index_init(menu_unit *_p1, uint8 ind_0, uint8 ind_1)
//{
//	_p1->m_index[0] = ind_0;
//	_p1->m_index[1] = ind_1;
// }
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ���޸Ĳ�������
////  @param      p_param     	   		���޸Ĳ���ָ��
////  @param      t     	        		���޸Ĳ�����������
////  @param      delta       	    	����ÿ����һ�Σ��������޸ĵĴ�С
////  @param      num     	  		 		������������ʾλ��
////  @param      point_num     	 		������С����ʾλ��
////  @param      t1     	 						��Ԫ����
////  @param      _name[STR_LEN_MAX]  ��������
////  @return     void
////  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
////-------------------------------------------------------------------------------------------------------------------
// void unit_param_set(void *p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX])
//{
//	static menu_unit *p_middle = NULL;
//	menu_unit *p1 = NULL, *p2 = NULL;
//	param_set *p1_par = NULL, *p2_par = NULL;
//	static menu_unit *dad;

// #ifdef USE_STATIC_MENU
//	p1 = my_menu_unit + static_cnt;
//	p1_par = my_param_set + static_cnt;
//	static_cnt++;
//	p2 = my_menu_unit + static_cnt;
//	p2_par = my_param_set + static_cnt;
//	static_cnt++;
// #else
//	p1 = malloc(sizeof(menu_unit));
//	p1_par = malloc(sizeof(param_set));

//	p2 = malloc(sizeof(menu_unit));
//	p2_par = malloc(sizeof(param_set));
// #endif

//	if (P_dad_head == NULL)
//	{
// #ifdef USE_STATIC_MENU
//		dad = my_menu_unit + static_cnt;
//		static_cnt++;
// #else
//		dad = malloc(sizeof(menu_unit));
// #endif
//		P_dad_head = dad;
//		unit_default(dad, 0);
//		p_unit = dad;
//		p_unit_last = NULL;
//		unit_index_init(p1, 0, 0);
//		unit_index_init(p2, 0, 1);
//		dad_link(dad, dad, p1);
//		son_link(p1, p2, dad);
//		son_link(p2, p1, dad);
//	}
//	else
//	{
//		if (IND2 >= SON_NUM - 2)
//		{
// #ifdef USE_STATIC_MENU
//			dad = my_menu_unit + static_cnt;
//			static_cnt++;
// #else
//			dad = malloc(sizeof(menu_unit));
// #endif
//			unit_default(dad, IND1 + 1);
//			unit_index_init(p1, IND1 + 1, 0);
//			unit_index_init(p2, IND1 + 1, 1);
//			dad_link(P_dad_head->down, dad, NULL);
//			dad_link(dad, P_dad_head, p1);
//			son_link(p1, p2, dad);
//			son_link(p2, p1, dad);
//		}
//		else
//		{
//			unit_index_init(p1, IND1, IND2 + 1);
//			unit_index_init(p2, IND1, IND2 + 2);
//			son_link(p_middle, p1, dad);
//			son_link(p1, p2, dad);
//			son_link(p2, SON_BEGIN_UNIT, dad);
//		}
//	}
//	p_middle = p2;
//	p1->par_set = p1_par;
//	p1->par_set->p_par = p_param;
//	p1->par_set->par_type = t;
//	p1->par_set->delta = delta;
//	p1->par_set->num = num;
//	p1->par_set->point_num = point_num;
//	p1->type_t = t1;
//	memset(p1->name, 0, STR_LEN_MAX);
//	strcpy(p1->name, _name);
//	p1->name[strlen(_name)] = '+';

//	p2->par_set = p2_par;
//	p2->par_set->p_par = p_param;
//	p2->par_set->par_type = t;
//	p2->par_set->delta = -delta;
//	p2->par_set->num = num;
//	p2->par_set->point_num = point_num;
//	p2->type_t = t1;
//	memset(p2->name, 0, STR_LEN_MAX);
//	strcpy(p2->name, _name);
//	p2->name[strlen(_name)] = '-';
//}

////-------------------------------------------------------------------------------------------------------------------
////  @brief      ��ʼ�������˵���Ԫ
////  @param      fun     	   				�败���ĺ���ָ��
////  @param      _name       	  		��ʾ���ַ���
////  @return     void
////  Sample usage:               		fun_init(Flash_Read,"flash_read");
////-------------------------------------------------------------------------------------------------------------------
// void fun_init(void *fun, const char *_name)
//{
//	menu_unit *p2 = NULL;
//	menu_unit *p1 = NULL;

// #ifdef USE_STATIC_MENU
//	p1 = my_menu_unit + static_cnt;
//	static_cnt++;
// #else
//	p1 = malloc(sizeof(menu_unit));
// #endif
//	p1->par_set = NULL;

//	if (IND2 >= SON_NUM - 1)
//	{
// #ifdef USE_STATIC_MENU
//		p2 = my_menu_unit + static_cnt;
//		static_cnt++;
// #else
//		p2 = malloc(sizeof(menu_unit));
// #endif

//		unit_default(p2, IND1 + 1);
//		unit_index_init(p1, IND1 + 1, 0);
//		dad_link(P_dad_head->down, p2, NULL);
//		dad_link(p2, P_dad_head, p1);
//		son_link(p1, p1, p2);
//	}
//	else
//	{
//		unit_index_init(p1, IND1, IND2 + 1);
//		son_link(SON_END_UNIT, p1, P_dad_head->down);
//		son_link(p1, SON_BEGIN_UNIT, P_dad_head->down);
//	}

//	p1->current_operation = fun;

//	for (uint8 i = 0; i < STR_LEN_MAX; i++)
//		p1->name[i] = 0;
//	strcpy(p1->name, _name);
//	p1->type_t = USE_FUN;
//}

//// �ַ���������ʼ��
// void index_xy_init()
//{
// #ifdef USE_STATIC_MENU
//	p_index_xy_dad = my_index;
//	p_index_xy_son = my_index + DAD_NUM * 2;
// #else
//	p_index_xy_dad = (uint8 *)malloc(sizeof(uint8) * DAD_NUM * 2);
//	p_index_xy_son = (uint8 *)malloc(sizeof(uint8) * SON_NUM * 2);
// #endif
//	uint8 half;
//	half = (DAD_NUM + 1) / 2;
//	for (uint8 i = 0; i < half; i++)
//	{
//		DAD_INDEX(i, 0) = DIS_X * 0;
//		DAD_INDEX(i, 1) = DIS_Y * i;
//	}
//	for (uint8 i = half; i < DAD_NUM; i++)
//	{
//		DAD_INDEX(i, 0) = DIS_X * 1;
//		DAD_INDEX(i, 1) = DIS_Y * (i - half);
//	}
//	for (uint8 i = 0; i < SON_NUM; i++)
//	{
//		SON_INDEX(i, 0) = DIS_X * 0;
//		SON_INDEX(i, 1) = DIS_Y * i;
//	}
// }

// void flash_init_wz()
//{
//// ��ʼ����Ҫ����Ĳ���  ������˳���ʼ����ȥ��ע�⣺��data_flash.h��������������
// #if AUTO_READ
//	menu_unit *p;
//	p = P_dad_head->enter;
//	while (1)
//	{
//		if (p->m_index[1] < p->up->m_index[1])
//		{
//			flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//			p = p->up;
//		}
//		else
//		{
//			flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//			p = p->back->up->enter;
//		}
//		if (p->par_set.type_t == USE_FUN)
//			break;
//	}
// #endif
//// index��ʼ�������ù�
//// �ϵ��ȡ����
// #if AUTO_READ
//	flash_index_init();
//	flash_read();
// #endif
// }

//// �Ƿ������Ļ
// uint8 is_menu_clear()
//{
//	return (((p_unit->back) != (p_unit)) && button1) || (((p_unit->enter) != (p_unit)) && button2);
// }
// uint8 begin_menu_flag = 1;
//// �����˵���ʾ
// void center_menu()
//{
//	uint8 index = p_unit->m_index[0];

//	if (first_in_page_flag)
//		showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);

//	if (button3 || button4)
//	{
//		if (index == 0)
//		{
//			showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), " ");
//			showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(1, 0), DAD_INDEX(1, 1), " ");
//		}
//		else if (index == DAD_NUM - 1)
//		{
//			showstr(DAD_INDEX(DAD_NUM - 2, 0), DAD_INDEX(DAD_NUM - 2, 1), " ");
//			showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), " ");
//		}
//		else
//		{
//			showstr(DAD_INDEX(index - 1, 0), DAD_INDEX(index - 1, 1), " ");
//			showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(index + 1, 0), DAD_INDEX(index + 1, 1), " ");
//		}
//	}
//	else if ((is_clear_flag == 1 && (button1)) || (begin_menu_flag))
//	{
//		menu_unit *p = NULL;
//		p = p_unit;
//		for (uint8 i = 0; i < DAD_NUM; i++)
//		{
//			showstr(DAD_INDEX(p->m_index[0], 0) + MOUSE_DIS, DAD_INDEX(p->m_index[0], 1), p->name);
//			p = p->up;
//		}
//		begin_menu_flag = 0;
//	}
//}

//// �Ӽ��˵���ʾ
// void assist_menu()
//{
//	uint8 index = p_unit->m_index[1];
//	if (first_in_page_flag)
//		showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);

//	if (button3 || button4)
//	{
//		if (index == 0)
//		{
//			showstr(SON_INDEX(p_unit->down->m_index[1], 0), SON_INDEX(p_unit->down->m_index[1], 1), " ");
//			showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(1, 0), SON_INDEX(1, 1), " ");
//		}
//		else if (index == p_unit->back->enter->down->m_index[1])
//		{
//			showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//			showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), " ");
//		}
//		else
//		{
//			showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//			showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(index + 1, 0), SON_INDEX(index + 1, 1), " ");
//		}
//	}
//	else if (is_clear_flag == 1 && (button2))
//	{
//		menu_unit *p = NULL;
//		p = p_unit;
//		for (uint8 i = 0; i < SON_NUM; i++)
//		{
//			showstr(SON_INDEX(p->m_index[1], 0) + MOUSE_DIS, SON_INDEX(p->m_index[1], 1), p->name);
//			p = p->up;
//		}
//	}
//}

////-------------------------------------------------------------------------------------------------------------------
//// @brief		�޸Ĳ�����С
//// @param		param  	 	����
//// @return	void
//// Sample usage:				change_value(TYPE_FLOAT,0.01,&P_speed);button2����ʱ�޸�P_speed��ֵΪP_speed+0.01
//// attention	 :				ע����޸Ĳ��������ͣ�TYPE_DOUBLE,TYPE_FLOAT,TYPE_INT...�������Ϳ�������ӣ�
////-------------------------------------------------------------------------------------------------------------------
// void change_value(param_set *param)
//{
//	uint8 type = param->par_type;
//	float delta_x = param->delta;
//	void *value = param->p_par;
//	uint8 num = param->num;
//	uint8 point_num = param->point_num;
//	static uint8 last_index = 0;
//	uint8 is_show_num = (p_unit_last->par_set == NULL ? 1 : p_unit_last->par_set->p_par != p_unit->par_set->p_par);
//	if (p_unit->par_set->p_par != NULL)
//	{
//		if (type == TYPE_FLOAT)
//		{
//			float *p_value;
//			p_value = (float *)(value);
//			if (IS_OK)
//			{
//				*p_value += delta_x;
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//		}
//		else if (type == TYPE_DOUBLE)
//		{
//			double *p_value;
//			p_value = (double *)(value);
//			if (IS_OK)
//			{
//				*p_value += (double)delta_x;
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//		}
//		else if (type == TYPE_INT)
//		{
//			int *p_value;
//			p_value = (int *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "        ");
//				showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//		else if (type == TYPE_UINT16)
//		{
//			uint16 *p_value;
//			p_value = (uint16 *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//				showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//		else if (type == TYPE_UINT32)
//		{
//			uint32 *p_value;
//			p_value = (uint32 *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//				showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//	}
//	last_index = p_unit->m_index[1];
// }

//// �Ƿ�Ϊ��һ�ν�����ҳ��
// void is_first_in_page()
//{
//	first_in_page_flag = (p_unit_last != p_unit) && (button1 || button2);
// }

// void show_menu()
//{
//	if (p_unit->m_index[1] == 255)
//		center_menu(); // ��ʾ���˵�
//	else
//		assist_menu(); // ��ʾ�Ӳ˵�
// }

//// Ч������
// void fun_menu()
//{
//	if (p_unit->type_t == NORMAL_PAR || p_unit->type_t == PID_PAR)
//	{
//		change_value(p_unit->par_set);
//	}
//	else
//	{
//		current_operation_menu = p_unit->current_operation; // ����ָ��
//		(*current_operation_menu)();
//	}
// }
//// ��ʾ���̹��Ӻ���
// void show_process(void *parameter)
//{
// #ifdef MENU_USE_RTT
//	while (1)
//	{
//		// ����
//		button1 = (RT_EOK == rt_sem_take(key1_sem, RT_WAITING_NO));
//		button2 = (RT_EOK == rt_sem_take(key2_sem, RT_WAITING_NO));
//		button3 = (RT_EOK == rt_sem_take(key3_sem, RT_WAITING_NO));
//		button4 = (RT_EOK == rt_sem_take(key4_sem, RT_WAITING_NO));

//		is_clear_flag = is_menu_clear();

//		if (button1 || button2 || button3 || button4)
//		{
//			rt_sem_release(button_feedback_sem);
//		}

//		if (is_clear_flag)
//			clear_hhh(0, 0, SCREEN_W, SON_NUM * 16, IPS200_BGCOLOR);

//		if (button1 == 1)
//			p_unit = p_unit->back;
//		else if (button2 == 1)
//			p_unit = p_unit->enter;
//		else if (button3 == 1)
//			p_unit = p_unit->up;
//		else if (button4 == 1)
//			p_unit = p_unit->down;

//		is_first_in_page();

//		// ��ʾ����
//		show_menu();

//		// Ч������
//		fun_menu();

//		p_unit_last = p_unit;
//		// �̹߳���
//		rt_thread_mdelay(10);
//	}
// #else
//	button_entry(NULL);

//	is_clear_flag = is_menu_clear();

//	if (is_clear_flag)
//		clear();

//	if (button1 == 1)
//		p_unit = p_unit->back;
//	else if (button2 == 1)
//		p_unit = p_unit->enter;
//	else if (button3 == 1)
//		p_unit = p_unit->up;
//	else if (button4 == 1)
//		p_unit = p_unit->down;

//	is_first_in_page();

//	// ��ʾ����
//	show_menu();

//	// Ч������
//	fun_menu();

//	p_unit_last = p_unit;
// #endif
//}

//// �˵�ϵͳ��ʼ��(��˳��)
// void menu_init()
//{
//	/*---------------��Ļ��ʼ��----------------*/
//	screen_init(IPS200_TYPE_SPI);

//	/*---------------������ʼ��----------------*/
//	key_into();

//	/*---------------���޸Ĳ���----------------*/
//	UNIT_SET();

//	/*---------------����Ļص�����----------------*/
//	FUN_INIT();

//	/*---------------�˵����ֳ�ʼ��----------------*/
//	dad_name_init();

//	/*---------------�ַ���������ʼ��----------------*/
//	index_xy_init();

///*-----------------����flash---------------*/
// #ifdef USE_FLASH
//	flash_init_wz();
// #endif

///*----------------�˵��̳߳�ʼ��----------------*/
// #ifdef MENU_USE_RTT
//	rt_thread_t tid;
//	// ������ʾ�߳�
//	tid = rt_thread_create("display", show_process, RT_NULL, 1024 * 2, 11, 5);
//	// ������ʾ�߳�
//	if (RT_NULL != tid)
//	{
//		rt_thread_startup(tid);
//	}
// #endif
// }

//// ����ҹ������ģʽ
// static uint16 IPS200_BGCOLOR = RGB565_WHITE;
// void day_night()
//{
//	if (IS_OK)
//	{
//		if (IPS200_BGCOLOR == RGB565_WHITE)
//		{
//			IPS200_BGCOLOR = RGB565_BLACK;
//			ips200_set_color(RGB565_WHITE, RGB565_BLACK);
//			showstr(0, (SON_NUM + 1) * 16, "BLACK");
//		}
//		else if (IPS200_BGCOLOR == RGB565_BLACK)
//		{
//			IPS200_BGCOLOR = RGB565_WHITE;
//			ips200_set_color(RGB565_BLACK, RGB565_WHITE);
//			showstr(0, (SON_NUM + 1) * 16, "WHITE");
//		}
//	}
// }
//// ���ı�����ɫ
// void rand_color()
//{
//	if (IS_OK)
//	{
//		uint16 color;
//		color = rand() % (32768 * 2);
//		ips200_set_color(color, ~color);
//		showstr(0, (SON_NUM + 1) * 16, "rand");
//	}
// }

// void NULL_FUN()
//{
// }

// float test_a = 1.1f;
// int test_b = 100;
// double test_c = 100;
// uint16 test_d = 20;
// uint32 test_e = 32;

// void UNIT_SET()
//{
//	// �˵���Ԫ���β�����ʼ��
//	unit_param_set(&test_a, TYPE_DOUBLE, 0.5, 3, 3, NORMAL_PAR, "test_a");
//	unit_param_set(&test_b, TYPE_DOUBLE, 2, 6, 3, NORMAL_PAR, "test_b");
//	unit_param_set(&test_c, TYPE_DOUBLE, 11.11, 4, 4, NORMAL_PAR, "test_c");
//	unit_param_set(&test_d, TYPE_UINT16, 1, 6, 0, NORMAL_PAR, "test_d");
//	unit_param_set(&test_e, TYPE_UINT32, 1, 6, 0, NORMAL_PAR, "test_e");
// }

// void FUN_INIT()
//{
//	// �˵���Ԫ����ָ���ʼ��
//	fun_init(NULL_FUN, "NULL_FUN1");
//	fun_init(NULL_FUN, "NULL_FUN2");
//	fun_init(day_night, "day_night");
//	fun_init(rand_color, "rand_color");
//	fun_init(photo_display, "photo_dis");

//	// fun_init(NULL_FUN, "NULL_FUN3");
//}

/*
���ǵڶ��溯������ȡ״̬����ʾ����ͷͼ��
*/

// #include "zf_common_headfile.h"
// #include "auto_menu.h"
// #include "key.h"

//// �����ź��������������ź���
// #ifdef MENU_USE_RTT
// extern rt_sem_t key1_sem;
// extern rt_sem_t key2_sem;
// extern rt_sem_t key3_sem;
// extern rt_sem_t key4_sem;
// extern rt_sem_t button_feedback_sem;
// #endif
///*-------------------����--------------------
//					button1����
//					button2ȷ��
//					button3�·�
//					button4�Ϸ�
//---------------------------------------------*/
// uint8 button1 = 0, button2 = 0, button3 = 0, button4 = 0;
// uint8 first_in_page_flag = 0;
// uint8 is_clear_flag = 0;

// uint8 *p_index_xy_dad, *p_index_xy_son;

// static menu_unit *p_unit = NULL;	  // ��Ԫָ��
// static menu_unit *p_unit_last = NULL; // ��һ�εĵ�Ԫָ��

// static menu_unit *P_dad_head = NULL;

// uint8 DAD_NUM = 1;

// #ifdef USE_STATIC_MENU
// menu_unit my_menu_unit[MEM_SIZE];
// param_set my_param_set[MEM_SIZE];
// uint8 my_index[MEM_SIZE * 2];
// static int static_cnt = 0;
// #endif

//// ��������ָ��
// void (*current_operation_menu)(void);

//// ���ȫ�ֱ�������ͼ����ʾģʽ
// static uint8_t display_mode = 0; // 0:�����˵�, 1:��ʾͼ��

// void dad_name_init()
//{
//	char *p = NULL;
//	p = (char *)malloc(STR_LEN_MAX);
//	memset(p, 0, STR_LEN_MAX);
//	strcpy(p, "Page ");
//	DAD_NUM = IND1 + 1;
//	for (uint8 i = 0; i < DAD_NUM; i++)
//	{
//		if (i < 10)
//		{
//			p[5] = '0' + i;
//			strcpy(P_dad_head->name, p);
//		}
//		else
//		{
//			p[5] = '0' + i / 10;
//			p[6] = '0' + i % 10;
//			strcpy(P_dad_head->name, p);
//		}
//		P_dad_head = P_dad_head->up;
//	}
//	free(p);
// }

//// �Ӳ˵���Ԫ����
// void son_link(menu_unit *p_1, menu_unit *p_2, menu_unit *dad)
//{
//	p_1->up = p_2;
//	p_2->down = p_1;
//	p_1->enter = p_1;
//	p_2->enter = p_2;
//	p_1->back = dad;
//	p_2->back = dad;
// }

//// ���˵���Ԫ����
// void dad_link(menu_unit *p_1, menu_unit *p_2, menu_unit *son)
//{
//	p_1->up = p_2;
//	p_2->down = p_1;
//	p_1->back = p_1;
//	p_2->back = p_2;
//	if (son != NULL)
//		p_1->enter = son;
// }
// void unit_default(menu_unit *p, uint8 ind)
//{
//	p->type_t = USE_FUN;
//	p->current_operation = NULL_FUN;
//	p->m_index[0] = ind;
//	p->m_index[1] = 255;
// }
// void unit_index_init(menu_unit *_p1, uint8 ind_0, uint8 ind_1)
//{
//	_p1->m_index[0] = ind_0;
//	_p1->m_index[1] = ind_1;
// }
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ���޸Ĳ�������
////  @param      p_param     	   		���޸Ĳ���ָ��
////  @param      t     	        		���޸Ĳ�����������
////  @param      delta       	    	����ÿ����һ�Σ��������޸ĵĴ�С
////  @param      num     	  		 		������������ʾλ��
////  @param      point_num     	 		������С����ʾλ��
////  @param      t1     	 						��Ԫ����
////  @param      _name[STR_LEN_MAX]  ��������
////  @return     void
////  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
////-------------------------------------------------------------------------------------------------------------------
// void unit_param_set(void *p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX])
//{
//	static menu_unit *p_middle = NULL;
//	menu_unit *p1 = NULL, *p2 = NULL;
//	param_set *p1_par = NULL, *p2_par = NULL;
//	static menu_unit *dad;

// #ifdef USE_STATIC_MENU
//	p1 = my_menu_unit + static_cnt;
//	p1_par = my_param_set + static_cnt;
//	static_cnt++;
//	p2 = my_menu_unit + static_cnt;
//	p2_par = my_param_set + static_cnt;
//	static_cnt++;
// #else
//	p1 = malloc(sizeof(menu_unit));
//	p1_par = malloc(sizeof(param_set));

//	p2 = malloc(sizeof(menu_unit));
//	p2_par = malloc(sizeof(param_set));
// #endif

//	if (P_dad_head == NULL)
//	{
// #ifdef USE_STATIC_MENU
//		dad = my_menu_unit + static_cnt;
//		static_cnt++;
// #else
//		dad = malloc(sizeof(menu_unit));
// #endif
//		P_dad_head = dad;
//		unit_default(dad, 0);
//		p_unit = dad;
//		p_unit_last = NULL;
//		unit_index_init(p1, 0, 0);
//		unit_index_init(p2, 0, 1);
//		dad_link(dad, dad, p1);
//		son_link(p1, p2, dad);
//		son_link(p2, p1, dad);
//	}
//	else
//	{
//		if (IND2 >= SON_NUM - 2)
//		{
// #ifdef USE_STATIC_MENU
//			dad = my_menu_unit + static_cnt;
//			static_cnt++;
// #else
//			dad = malloc(sizeof(menu_unit));
// #endif
//			unit_default(dad, IND1 + 1);
//			unit_index_init(p1, IND1 + 1, 0);
//			unit_index_init(p2, IND1 + 1, 1);
//			dad_link(P_dad_head->down, dad, NULL);
//			dad_link(dad, P_dad_head, p1);
//			son_link(p1, p2, dad);
//			son_link(p2, p1, dad);
//		}
//		else
//		{
//			unit_index_init(p1, IND1, IND2 + 1);
//			unit_index_init(p2, IND1, IND2 + 2);
//			son_link(p_middle, p1, dad);
//			son_link(p1, p2, dad);
//			son_link(p2, SON_BEGIN_UNIT, dad);
//		}
//	}
//	p_middle = p2;
//	p1->par_set = p1_par;
//	p1->par_set->p_par = p_param;
//	p1->par_set->par_type = t;
//	p1->par_set->delta = delta;
//	p1->par_set->num = num;
//	p1->par_set->point_num = point_num;
//	p1->type_t = t1;
//	memset(p1->name, 0, STR_LEN_MAX);
//	strcpy(p1->name, _name);
//	p1->name[strlen(_name)] = '+';

//	p2->par_set = p2_par;
//	p2->par_set->p_par = p_param;
//	p2->par_set->par_type = t;
//	p2->par_set->delta = -delta;
//	p2->par_set->num = num;
//	p2->par_set->point_num = point_num;
//	p2->type_t = t1;
//	memset(p2->name, 0, STR_LEN_MAX);
//	strcpy(p2->name, _name);
//	p2->name[strlen(_name)] = '-';
//}

////-------------------------------------------------------------------------------------------------------------------
////  @brief      ��ʼ�������˵���Ԫ
////  @param      fun     	   				�败���ĺ���ָ��
////  @param      _name       	  		��ʾ���ַ���
////  @return     void
////  Sample usage:               		fun_init(Flash_Read,"flash_read");
////-------------------------------------------------------------------------------------------------------------------
// void fun_init(void *fun, const char *_name)
//{
//	menu_unit *p2 = NULL;
//	menu_unit *p1 = NULL;

// #ifdef USE_STATIC_MENU
//	p1 = my_menu_unit + static_cnt;
//	static_cnt++;
// #else
//	p1 = malloc(sizeof(menu_unit));
// #endif
//	p1->par_set = NULL;

//	if (IND2 >= SON_NUM - 1)
//	{
// #ifdef USE_STATIC_MENU
//		p2 = my_menu_unit + static_cnt;
//		static_cnt++;
// #else
//		p2 = malloc(sizeof(menu_unit));
// #endif

//		unit_default(p2, IND1 + 1);
//		unit_index_init(p1, IND1 + 1, 0);
//		dad_link(P_dad_head->down, p2, NULL);
//		dad_link(p2, P_dad_head, p1);
//		son_link(p1, p1, p2);
//	}
//	else
//	{
//		unit_index_init(p1, IND1, IND2 + 1);
//		son_link(SON_END_UNIT, p1, P_dad_head->down);
//		son_link(p1, SON_BEGIN_UNIT, P_dad_head->down);
//	}

//	p1->current_operation = fun;

//	for (uint8 i = 0; i < STR_LEN_MAX; i++)
//		p1->name[i] = 0;
//	strcpy(p1->name, _name);
//	p1->type_t = USE_FUN;
//}

//// �ַ���������ʼ��
// void index_xy_init()
//{
// #ifdef USE_STATIC_MENU
//	p_index_xy_dad = my_index;
//	p_index_xy_son = my_index + DAD_NUM * 2;
// #else
//	p_index_xy_dad = (uint8 *)malloc(sizeof(uint8) * DAD_NUM * 2);
//	p_index_xy_son = (uint8 *)malloc(sizeof(uint8) * SON_NUM * 2);
// #endif
//	uint8 half;
//	half = (DAD_NUM + 1) / 2;
//	for (uint8 i = 0; i < half; i++)
//	{
//		DAD_INDEX(i, 0) = DIS_X * 0;
//		DAD_INDEX(i, 1) = DIS_Y * i;
//	}
//	for (uint8 i = half; i < DAD_NUM; i++)
//	{
//		DAD_INDEX(i, 0) = DIS_X * 1;
//		DAD_INDEX(i, 1) = DIS_Y * (i - half);
//	}
//	for (uint8 i = 0; i < SON_NUM; i++)
//	{
//		SON_INDEX(i, 0) = DIS_X * 0;
//		SON_INDEX(i, 1) = DIS_Y * i;
//	}
// }

// void flash_init_wz()
//{
//// ��ʼ����Ҫ����Ĳ���  ������˳���ʼ����ȥ��ע�⣺��data_flash.h��������������
// #if AUTO_READ
//	menu_unit *p;
//	p = P_dad_head->enter;
//	while (1)
//	{
//		if (p->m_index[1] < p->up->m_index[1])
//		{
//			flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//			p = p->up;
//		}
//		else
//		{
//			flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//			p = p->back->up->enter;
//		}
//		if (p->par_set.type_t == USE_FUN)
//			break;
//	}
// #endif
//// index��ʼ�������ù�
//// �ϵ��ȡ����
// #if AUTO_READ
//	flash_index_init();
//	flash_read();
// #endif
// }

//// �Ƿ������Ļ
// uint8 is_menu_clear()
//{
//	return (((p_unit->back) != (p_unit)) && button1) || (((p_unit->enter) != (p_unit)) && button2);
// }
// uint8 begin_menu_flag = 1;
//// �����˵���ʾ
// void center_menu()
//{
//	uint8 index = p_unit->m_index[0];

//	if (first_in_page_flag)
//		showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);

//	if (button3 || button4)
//	{
//		if (index == 0)
//		{
//			showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), " ");
//			showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(1, 0), DAD_INDEX(1, 1), " ");
//		}
//		else if (index == DAD_NUM - 1)
//		{
//			showstr(DAD_INDEX(DAD_NUM - 2, 0), DAD_INDEX(DAD_NUM - 2, 1), " ");
//			showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), " ");
//		}
//		else
//		{
//			showstr(DAD_INDEX(index - 1, 0), DAD_INDEX(index - 1, 1), " ");
//			showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);
//			showstr(DAD_INDEX(index + 1, 0), DAD_INDEX(index + 1, 1), " ");
//		}
//	}
//	else if ((is_clear_flag == 1 && (button1)) || (begin_menu_flag))
//	{
//		menu_unit *p = NULL;
//		p = p_unit;
//		for (uint8 i = 0; i < DAD_NUM; i++)
//		{
//			showstr(DAD_INDEX(p->m_index[0], 0) + MOUSE_DIS, DAD_INDEX(p->m_index[0], 1), p->name);
//			p = p->up;
//		}
//		begin_menu_flag = 0;
//	}
//}

//// �Ӽ��˵���ʾ
// void assist_menu()
//{
//	uint8 index = p_unit->m_index[1];
//	if (first_in_page_flag)
//		showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);

//	if (button3 || button4)
//	{
//		if (index == 0)
//		{
//			showstr(SON_INDEX(p_unit->down->m_index[1], 0), SON_INDEX(p_unit->down->m_index[1], 1), " ");
//			showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(1, 0), SON_INDEX(1, 1), " ");
//		}
//		else if (index == p_unit->back->enter->down->m_index[1])
//		{
//			showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//			showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), " ");
//		}
//		else
//		{
//			showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//			showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//			showstr(SON_INDEX(index + 1, 0), SON_INDEX(index + 1, 1), " ");
//		}
//	}
//	else if (is_clear_flag == 1 && (button2))
//	{
//		menu_unit *p = NULL;
//		p = p_unit;
//		for (uint8 i = 0; i < SON_NUM; i++)
//		{
//			showstr(SON_INDEX(p->m_index[1], 0) + MOUSE_DIS, SON_INDEX(p->m_index[1], 1), p->name);
//			p = p->up;
//		}
//	}
//}

////-------------------------------------------------------------------------------------------------------------------
//// @brief		�޸Ĳ�����С
//// @param		param  	 	����
//// @return	void
//// Sample usage:				change_value(TYPE_FLOAT,0.01,&P_speed);button2����ʱ�޸�P_speed��ֵΪP_speed+0.01
//// attention	 :				ע����޸Ĳ��������ͣ�TYPE_DOUBLE,TYPE_FLOAT,TYPE_INT...�������Ϳ�������ӣ�
////-------------------------------------------------------------------------------------------------------------------
// void change_value(param_set *param)
//{
//	uint8 type = param->par_type;
//	float delta_x = param->delta;
//	void *value = param->p_par;
//	uint8 num = param->num;
//	uint8 point_num = param->point_num;
//	static uint8 last_index = 0;
//	uint8 is_show_num = (p_unit_last->par_set == NULL ? 1 : p_unit_last->par_set->p_par != p_unit->par_set->p_par);
//	if (p_unit->par_set->p_par != NULL)
//	{
//		if (type == TYPE_FLOAT)
//		{
//			float *p_value;
//			p_value = (float *)(value);
//			if (IS_OK)
//			{
//				*p_value += delta_x;
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//		}
//		else if (type == TYPE_DOUBLE)
//		{
//			double *p_value;
//			p_value = (double *)(value);
//			if (IS_OK)
//			{
//				*p_value += (double)delta_x;
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//				showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//			}
//		}
//		else if (type == TYPE_INT)
//		{
//			int *p_value;
//			p_value = (int *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "        ");
//				showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//		else if (type == TYPE_UINT16)
//		{
//			uint16 *p_value;
//			p_value = (uint16 *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//				showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//		else if (type == TYPE_UINT32)
//		{
//			uint32 *p_value;
//			p_value = (uint32 *)(value);
//			if (IS_OK)
//			{
//				*p_value += (int)delta_x;
//				showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//			if (is_show_num)
//			{
//				showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//				showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//			}
//		}
//	}
//	last_index = p_unit->m_index[1];
// }

//// �Ƿ�Ϊ��һ�ν�����ҳ��
// void is_first_in_page()
//{
//	first_in_page_flag = (p_unit_last != p_unit) && (button1 || button2);
// }

// void show_menu()
//{
//	if (p_unit->m_index[1] == 255)
//		center_menu(); // ��ʾ���˵�
//	else
//		assist_menu(); // ��ʾ�Ӳ˵�
// }

//// Ч������
// void fun_menu()
//{
//	if (p_unit->type_t == NORMAL_PAR || p_unit->type_t == PID_PAR)
//	{
//		change_value(p_unit->par_set);
//	}
//	else
//	{
//		current_operation_menu = p_unit->current_operation; // ����ָ��
//		(*current_operation_menu)();
//	}
// }

//// ͼ����ʾ����
// void handle_image_display()
//{
//     if (mt9v03x_finish_flag)
//     {
//         // ����ͼ������
//         memcpy(image_copy, mt9v03x_image, MT9V03X_H * MT9V03X_W);
//
//         // ��ʾԭʼͼ��
//         ips200_show_gray_image(0, 0, (const uint8 *)image_copy,
//                               MT9V03X_W, MT9V03X_H,
//                               MT9V03X_W, MT9V03X_H, 0);
//
//         // ��ʾ�����Ķ�ֵ��ͼ��
//         ips200_show_gray_image(0, 120, (const uint8 *)binaryImage,
//                               MT9V03X_W, MT9V03X_H,
//                               MT9V03X_W, MT9V03X_H, 0);
//
//         // ��ʾ�˳���ʾ
//         ips200_show_string(10, 230, "Press BACK to exit");
//
//         mt9v03x_finish_flag = 0;
//     }
//
//     // ��ⷵ�ؼ�
//     if (button1) {
//         display_mode = 0;  // �˳�ͼ��ģʽ
//         is_clear_flag = 1; // ����������־
//         assist_menu();     // ˢ�²˵���ʾ
//     }
// }

//// ��ʾ���̹��Ӻ���
// void show_process(void *parameter)
//{
// #ifdef MENU_USE_RTT
//	while (1)
//	{
//		// ��ȡ�����ź���
//		button1 = (RT_EOK == rt_sem_take(key1_sem, RT_WAITING_NO));
//		button2 = (RT_EOK == rt_sem_take(key2_sem, RT_WAITING_NO));
//		button3 = (RT_EOK == rt_sem_take(key3_sem, RT_WAITING_NO));
//		button4 = (RT_EOK == rt_sem_take(key4_sem, RT_WAITING_NO));

//		// ���ݵ�ǰģʽִ�в�ͬ����
//		if (display_mode == 1) {
//			// ͼ����ʾģʽ
//			handle_image_display();
//		} else {
//			// �����˵�ģʽ
//			is_clear_flag = is_menu_clear();

//			if (button1 || button2 || button3 || button4)
//			{
//				rt_sem_release(button_feedback_sem);
//			}

//			if (is_clear_flag)
//				clear_hhh(0, 0, SCREEN_W, SON_NUM * 16, IPS200_BGCOLOR);

//			if (button1 == 1)
//				p_unit = p_unit->back;
//			else if (button2 == 1)
//				p_unit = p_unit->enter;
//			else if (button3 == 1)
//				p_unit = p_unit->up;
//			else if (button4 == 1)
//				p_unit = p_unit->down;

//			is_first_in_page();

//			// ��ʾ����
//			show_menu();

//			// Ч������
//			fun_menu();

//			p_unit_last = p_unit;
//		}
//
//		// �̹߳���
//		rt_thread_mdelay(10);
//	}
// #else
//	// ��������µİ�������
//	button_entry(NULL);
//
//	// ���ݵ�ǰģʽִ�в�ͬ����
//	if (display_mode == 1) {
//		// ͼ����ʾģʽ
//		handle_image_display();
//	} else {
//		// �����˵�ģʽ
//		is_clear_flag = is_menu_clear();

//		if (is_clear_flag)
//			clear();

//		if (button1 == 1)
//			p_unit = p_unit->back;
//		else if (button2 == 1)
//			p_unit = p_unit->enter;
//		else if (button3 == 1)
//			p_unit = p_unit->up;
//		else if (button4 == 1)
//			p_unit = p_unit->down;

//		is_first_in_page();

//		// ��ʾ����
//		show_menu();

//		// Ч������
//		fun_menu();

//		p_unit_last = p_unit;
//	}
// #endif
//}

//// �˵�ϵͳ��ʼ��(��˳��)
// void menu_init()
//{
//	/*---------------��Ļ��ʼ��----------------*/
//	screen_init(IPS200_TYPE_SPI);

//	/*---------------������ʼ��----------------*/
//	key_into();

//	/*---------------���޸Ĳ���----------------*/
//	UNIT_SET();

//	/*---------------����Ļص�����----------------*/
//	FUN_INIT();

//	/*---------------�˵����ֳ�ʼ��----------------*/
//	dad_name_init();

//	/*---------------�ַ���������ʼ��----------------*/
//	index_xy_init();

///*-----------------����flash---------------*/
// #ifdef USE_FLASH
//	flash_init_wz();
// #endif

///*----------------�˵��̳߳�ʼ��----------------*/
// #ifdef MENU_USE_RTT
//	rt_thread_t tid;
//	// ������ʾ�߳�
//	tid = rt_thread_create("display", show_process, RT_NULL, 1024 * 2, 11, 5);
//	// ������ʾ�߳�
//	if (RT_NULL != tid)
//	{
//		rt_thread_startup(tid);
//	}
// #endif
// }

//// ����ҹ������ģʽ
// static uint16 IPS200_BGCOLOR = RGB565_WHITE;
// void day_night()
//{
//	if (IS_OK)
//	{
//		if (IPS200_BGCOLOR == RGB565_WHITE)
//		{
//			IPS200_BGCOLOR = RGB565_BLACK;
//			ips200_set_color(RGB565_WHITE, RGB565_BLACK);
//			showstr(0, (SON_NUM + 1) * 16, "BLACK");
//		}
//		else if (IPS200_BGCOLOR == RGB565_BLACK)
//		{
//			IPS200_BGCOLOR = RGB565_WHITE;
//			ips200_set_color(RGB565_BLACK, RGB565_WHITE);
//			showstr(0, (SON_NUM + 1) * 16, "WHITE");
//		}
//	}
// }
//// ���ı�����ɫ
// void rand_color()
//{
//	if (IS_OK)
//	{
//		uint16 color;
//		color = rand() % (32768 * 2);
//		ips200_set_color(color, ~color);
//		showstr(0, (SON_NUM + 1) * 16, "rand");
//	}
// }

///*
// �������û��Լ����Զ��庯��
// 1.�˵�����ͼ����ʾ�ͳ��������ʾ����
//*/

// void photo_display()
//{
//	if (IS_OK)
//	{
//		display_mode = 1;  // ����ͼ����ʾģʽ
//		ips200_clear();    // ����
//	}
// }

// void NULL_FUN()
//{
// }

// float test_a = 1.1f;
// int test_b = 100;
// double test_c = 100;
// uint16 test_d = 20;
// uint32 test_e = 32;

// void UNIT_SET()
//{
//	// �˵���Ԫ���β�����ʼ��
//	unit_param_set(&test_a, TYPE_DOUBLE, 0.5, 3, 3, NORMAL_PAR, "test_a");
//	unit_param_set(&test_b, TYPE_DOUBLE, 2, 6, 3, NORMAL_PAR, "test_b");
//	unit_param_set(&test_c, TYPE_DOUBLE, 11.11, 4, 4, NORMAL_PAR, "test_c");
//	unit_param_set(&test_d, TYPE_UINT16, 1, 6, 0, NORMAL_PAR, "test_d");
//	unit_param_set(&test_e, TYPE_UINT32, 1, 6, 0, NORMAL_PAR, "test_e");
// }

// void FUN_INIT()
//{
//	// �˵���Ԫ����ָ���ʼ��
//	fun_init(NULL_FUN, "NULL_FUN1");
//	fun_init(NULL_FUN, "NULL_FUN2");
//	fun_init(day_night, "day_night");
//	fun_init(rand_color, "rand_color");
//	fun_init(photo_display, "photo_dis");

//	// fun_init(NULL_FUN, "NULL_FUN3");
//}

/*
���ǵ�����˵�����Ӳ�����ʾ�ͷ�������Ĳ˵�
*/

// #include "zf_common_headfile.h"
// #include "auto_menu.h"
// #include "key.h"

//// �����ź��������������ź���
// #ifdef MENU_USE_RTT
// extern rt_sem_t key1_sem;
// extern rt_sem_t key2_sem;
// extern rt_sem_t key3_sem;
// extern rt_sem_t key4_sem;
// extern rt_sem_t button_feedback_sem;
// #endif
///*-------------------����--------------------
//                    button1����
//                    button2ȷ��
//                    button3�·�
//                    button4�Ϸ�
//---------------------------------------------*/
// uint8 button1 = 0, button2 = 0, button3 = 0, button4 = 0;
// uint8 first_in_page_flag = 0;
// uint8 is_clear_flag = 0;

// uint8 *p_index_xy_dad, *p_index_xy_son;

// static menu_unit *p_unit = NULL;      // ��Ԫָ��
// static menu_unit *p_unit_last = NULL; // ��һ�εĵ�Ԫָ��

// static menu_unit *P_dad_head = NULL;

// uint8 DAD_NUM = 1;

// #ifdef USE_STATIC_MENU
// menu_unit my_menu_unit[MEM_SIZE];
// param_set my_param_set[MEM_SIZE];
// uint8 my_index[MEM_SIZE * 2];
// static int static_cnt = 0;
// #endif

//// ��������ָ��
// void (*current_operation_menu)(void);

//// ���ȫ�ֱ���������ʾģʽ
// static uint8_t display_mode = 0; // 0:�����˵�, 1:��ʾͼ��, 2:����������, 3:��������

//// ���������ݽṹ��
// typedef struct {
//     float temperature;
//     float humidity;
//     float pressure;
//     uint16_t light_level;
//     uint16_t distance;
// } SensorData;

// static SensorData sensor_data = {0};

//// ��������״̬����
// static uint8_t launch_state = 0;
// static uint32_t last_time = 0;
// static uint8_t countdown = 5;

// void dad_name_init()
//{
//     char *p = NULL;
//     p = (char *)malloc(STR_LEN_MAX);
//     memset(p, 0, STR_LEN_MAX);
//     strcpy(p, "Page ");
//     DAD_NUM = IND1 + 1;
//     for (uint8 i = 0; i < DAD_NUM; i++)
//     {
//         if (i < 10)
//         {
//             p[5] = '0' + i;
//             strcpy(P_dad_head->name, p);
//         }
//         else
//         {
//             p[5] = '0' + i / 10;
//             p[6] = '0' + i % 10;
//             strcpy(P_dad_head->name, p);
//         }
//         P_dad_head = P_dad_head->up;
//     }
//     free(p);
// }

//// �Ӳ˵���Ԫ����
// void son_link(menu_unit *p_1, menu_unit *p_2, menu_unit *dad)
//{
//     p_1->up = p_2;
//     p_2->down = p_1;
//     p_1->enter = p_1;
//     p_2->enter = p_2;
//     p_1->back = dad;
//     p_2->back = dad;
// }

//// ���˵���Ԫ����
// void dad_link(menu_unit *p_1, menu_unit *p_2, menu_unit *son)
//{
//     p_1->up = p_2;
//     p_2->down = p_1;
//     p_1->back = p_1;
//     p_2->back = p_2;
//     if (son != NULL)
//         p_1->enter = son;
// }
// void unit_default(menu_unit *p, uint8 ind)
//{
//     p->type_t = USE_FUN;
//     p->current_operation = NULL_FUN;
//     p->m_index[0] = ind;
//     p->m_index[1] = 255;
// }
// void unit_index_init(menu_unit *_p1, uint8 ind_0, uint8 ind_1)
//{
//     _p1->m_index[0] = ind_0;
//     _p1->m_index[1] = ind_1;
// }
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ���޸Ĳ�������
////  @param      p_param     	        ���޸Ĳ���ָ��
////  @param      t     	        		���޸Ĳ�����������
////  @param      delta       	    	����ÿ����һ�Σ��������޸ĵĴ�С
////  @param      num     	  		 		������������ʾλ��
////  @param      point_num     	 		������С����ʾλ��
////  @param      t1     	 						��Ԫ����
////  @param      _name[STR_LEN_MAX]  ��������
////  @return     void
////  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
////-------------------------------------------------------------------------------------------------------------------
// void unit_param_set(void *p_param, type_value t, float delta, uint8 num, uint8 point_num, unit_type t1, const char _name[STR_LEN_MAX])
//{
//     static menu_unit *p_middle = NULL;
//     menu_unit *p1 = NULL, *p2 = NULL;
//     param_set *p1_par = NULL, *p2_par = NULL;
//     static menu_unit *dad;

// #ifdef USE_STATIC_MENU
//     p1 = my_menu_unit + static_cnt;
//     p1_par = my_param_set + static_cnt;
//     static_cnt++;
//     p2 = my_menu_unit + static_cnt;
//     p2_par = my_param_set + static_cnt;
//     static_cnt++;
// #else
//     p1 = malloc(sizeof(menu_unit));
//     p1_par = malloc(sizeof(param_set));

//    p2 = malloc(sizeof(menu_unit));
//    p2_par = malloc(sizeof(param_set));
// #endif

//    if (P_dad_head == NULL)
//    {
// #ifdef USE_STATIC_MENU
//        dad = my_menu_unit + static_cnt;
//        static_cnt++;
// #else
//        dad = malloc(sizeof(menu_unit));
// #endif
//        P_dad_head = dad;
//        unit_default(dad, 0);
//        p_unit = dad;
//        p_unit_last = NULL;
//        unit_index_init(p1, 0, 0);
//        unit_index_init(p2, 0, 1);
//        dad_link(dad, dad, p1);
//        son_link(p1, p2, dad);
//        son_link(p2, p1, dad);
//    }
//    else
//    {
//        if (IND2 >= SON_NUM - 2)
//        {
// #ifdef USE_STATIC_MENU
//            dad = my_menu_unit + static_cnt;
//            static_cnt++;
// #else
//            dad = malloc(sizeof(menu_unit));
// #endif
//            unit_default(dad, IND1 + 1);
//            unit_index_init(p1, IND1 + 1, 0);
//            unit_index_init(p2, IND1 + 1, 1);
//            dad_link(P_dad_head->down, dad, NULL);
//            dad_link(dad, P_dad_head, p1);
//            son_link(p1, p2, dad);
//            son_link(p2, p1, dad);
//        }
//        else
//        {
//            unit_index_init(p1, IND1, IND2 + 1);
//            unit_index_init(p2, IND1, IND2 + 2);
//            son_link(p_middle, p1, dad);
//            son_link(p1, p2, dad);
//            son_link(p2, SON_BEGIN_UNIT, dad);
//        }
//    }
//    p_middle = p2;
//    p1->par_set = p1_par;
//    p1->par_set->p_par = p_param;
//    p1->par_set->par_type = t;
//    p1->par_set->delta = delta;
//    p1->par_set->num = num;
//    p1->par_set->point_num = point_num;
//    p1->type_t = t1;
//    memset(p1->name, 0, STR_LEN_MAX);
//    strcpy(p1->name, _name);
//    p1->name[strlen(_name)] = '+';

//    p2->par_set = p2_par;
//    p2->par_set->p_par = p_param;
//    p2->par_set->par_type = t;
//    p2->par_set->delta = -delta;
//    p2->par_set->num = num;
//    p2->par_set->point_num = point_num;
//    p2->type_t = t1;
//    memset(p2->name, 0, STR_LEN_MAX);
//    strcpy(p2->name, _name);
//    p2->name[strlen(_name)] = '-';
//}

////-------------------------------------------------------------------------------------------------------------------
////  @brief      ��ʼ�������˵���Ԫ
////  @param      fun     	   				�败���ĺ���ָ��
////  @param      _name       	  		��ʾ���ַ���
////  @return     void
////  Sample usage:               		fun_init(Flash_Read,"flash_read");
////-------------------------------------------------------------------------------------------------------------------
// void fun_init(void *fun, const char *_name)
//{
//     menu_unit *p2 = NULL;
//     menu_unit *p1 = NULL;

// #ifdef USE_STATIC_MENU
//     p1 = my_menu_unit + static_cnt;
//     static_cnt++;
// #else
//     p1 = malloc(sizeof(menu_unit));
// #endif
//     p1->par_set = NULL;

//    if (IND2 >= SON_NUM - 1)
//    {
// #ifdef USE_STATIC_MENU
//        p2 = my_menu_unit + static_cnt;
//        static_cnt++;
// #else
//        p2 = malloc(sizeof(menu_unit));
// #endif

//        unit_default(p2, IND1 + 1);
//        unit_index_init(p1, IND1 + 1, 0);
//        dad_link(P_dad_head->down, p2, NULL);
//        dad_link(p2, P_dad_head, p1);
//        son_link(p1, p1, p2);
//    }
//    else
//    {
//        unit_index_init(p1, IND1, IND2 + 1);
//        son_link(SON_END_UNIT, p1, P_dad_head->down);
//        son_link(p1, SON_BEGIN_UNIT, P_dad_head->down);
//    }

//    p1->current_operation = fun;

//    for (uint8 i = 0; i < STR_LEN_MAX; i++)
//        p1->name[i] = 0;
//    strcpy(p1->name, _name);
//    p1->type_t = USE_FUN;
//}

//// �ַ���������ʼ��
// void index_xy_init()
//{
// #ifdef USE_STATIC_MENU
//     p_index_xy_dad = my_index;
//     p_index_xy_son = my_index + DAD_NUM * 2;
// #else
//     p_index_xy_dad = (uint8 *)malloc(sizeof(uint8) * DAD_NUM * 2);
//     p_index_xy_son = (uint8 *)malloc(sizeof(uint8) * SON_NUM * 2);
// #endif
//     uint8 half;
//     half = (DAD_NUM + 1) / 2;
//     for (uint8 i = 0; i < half; i++)
//     {
//         DAD_INDEX(i, 0) = DIS_X * 0;
//         DAD_INDEX(i, 1) = DIS_Y * i;
//     }
//     for (uint8 i = half; i < DAD_NUM; i++)
//     {
//         DAD_INDEX(i, 0) = DIS_X * 1;
//         DAD_INDEX(i, 1) = DIS_Y * (i - half);
//     }
//     for (uint8 i = 0; i < SON_NUM; i++)
//     {
//         SON_INDEX(i, 0) = DIS_X * 0;
//         SON_INDEX(i, 1) = DIS_Y * i;
//     }
// }

// void flash_init_wz()
//{
//// ��ʼ����Ҫ����Ĳ���  ������˳���ʼ����ȥ��ע�⣺��data_flash.h��������������
// #if AUTO_READ
//     menu_unit *p;
//     p = P_dad_head->enter;
//     while (1)
//     {
//         if (p->m_index[1] < p->up->m_index[1])
//         {
//             flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//             p = p->up;
//         }
//         else
//         {
//             flash_unit_init(p->par_set.p_par, p->par_set.par_type);
//             p = p->back->up->enter;
//         }
//         if (p->par_set.type_t == USE_FUN)
//             break;
//     }
// #endif
//// index��ʼ�������ù�
//// �ϵ��ȡ����
// #if AUTO_READ
//     flash_index_init();
//     flash_read();
// #endif
// }

//// �Ƿ������Ļ
// uint8 is_menu_clear()
//{
//     return (((p_unit->back) != (p_unit)) && button1) || (((p_unit->enter) != (p_unit)) && button2);
// }
// uint8 begin_menu_flag = 1;
//// �����˵���ʾ
// void center_menu()
//{
//     uint8 index = p_unit->m_index[0];

//    if (first_in_page_flag)
//        showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);

//    if (button3 || button4)
//    {
//        if (index == 0)
//        {
//            showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), " ");
//            showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), MOUSE_LOOK);
//            showstr(DAD_INDEX(1, 0), DAD_INDEX(1, 1), " ");
//        }
//        else if (index == DAD_NUM - 1)
//        {
//            showstr(DAD_INDEX(DAD_NUM - 2, 0), DAD_INDEX(DAD_NUM - 2, 1), " ");
//            showstr(DAD_INDEX(DAD_NUM - 1, 0), DAD_INDEX(DAD_NUM - 1, 1), MOUSE_LOOK);
//            showstr(DAD_INDEX(0, 0), DAD_INDEX(0, 1), " ");
//        }
//        else
//        {
//            showstr(DAD_INDEX(index - 1, 0), DAD_INDEX(index - 1, 1), " ");
//            showstr(DAD_INDEX(index, 0), DAD_INDEX(index, 1), MOUSE_LOOK);
//            showstr(DAD_INDEX(index + 1, 0), DAD_INDEX(index + 1, 1), " ");
//        }
//    }
//    else if ((is_clear_flag == 1 && (button1)) || (begin_menu_flag))
//    {
//        menu_unit *p = NULL;
//        p = p_unit;
//        for (uint8 i = 0; i < DAD_NUM; i++)
//        {
//            showstr(DAD_INDEX(p->m_index[0], 0) + MOUSE_DIS, DAD_INDEX(p->m_index[0], 1), p->name);
//            p = p->up;
//        }
//        begin_menu_flag = 0;
//    }
//}

//// �Ӽ��˵���ʾ
// void assist_menu()
//{
//     uint8 index = p_unit->m_index[1];
//     if (first_in_page_flag)
//         showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);

//    if (button3 || button4)
//    {
//        if (index == 0)
//        {
//            showstr(SON_INDEX(p_unit->down->m_index[1], 0), SON_INDEX(p_unit->down->m_index[1], 1), " ");
//            showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), MOUSE_LOOK);
//            showstr(SON_INDEX(1, 0), SON_INDEX(1, 1), " ");
//        }
//        else if (index == p_unit->back->enter->down->m_index[1])
//        {
//            showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//            showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//            showstr(SON_INDEX(0, 0), SON_INDEX(0, 1), " ");
//        }
//        else
//        {
//            showstr(SON_INDEX(index - 1, 0), SON_INDEX(index - 1, 1), " ");
//            showstr(SON_INDEX(index, 0), SON_INDEX(index, 1), MOUSE_LOOK);
//            showstr(SON_INDEX(index + 1, 0), SON_INDEX(index + 1, 1), " ");
//        }
//    }
//    else if (is_clear_flag == 1 && (button2))
//    {
//        menu_unit *p = NULL;
//        p = p_unit;
//        for (uint8 i = 0; i < SON_NUM; i++)
//        {
//            showstr(SON_INDEX(p->m_index[1], 0) + MOUSE_DIS, SON_INDEX(p->m_index[1], 1), p->name);
//            p = p->up;
//        }
//    }
//}

////-------------------------------------------------------------------------------------------------------------------
//// @brief		�޸Ĳ�����С
//// @param		param  	 	����
//// @return	void
//// Sample usage:				change_value(TYPE_FLOAT,0.01,&P_speed);button2����ʱ�޸�P_speed��ֵΪP_speed+0.01
//// attention	 :				ע����޸Ĳ��������ͣ�TYPE_DOUBLE,TYPE_FLOAT,TYPE_INT...�������Ϳ�������ӣ�
////-------------------------------------------------------------------------------------------------------------------
// void change_value(param_set *param)
//{
//     uint8 type = param->par_type;
//     float delta_x = param->delta;
//     void *value = param->p_par;
//     uint8 num = param->num;
//     uint8 point_num = param->point_num;
//     static uint8 last_index = 0;
//     uint8 is_show_num = (p_unit_last->par_set == NULL ? 1 : p_unit_last->par_set->p_par != p_unit->par_set->p_par);
//     if (p_unit->par_set->p_par != NULL)
//     {
//         if (type == TYPE_FLOAT)
//         {
//             float *p_value;
//             p_value = (float *)(value);
//             if (IS_OK)
//             {
//                 *p_value += delta_x;
//                 showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//             }
//             if (is_show_num)
//             {
//                 showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//                 showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//             }
//         }
//         else if (type == TYPE_DOUBLE)
//         {
//             double *p_value;
//             p_value = (double *)(value);
//             if (IS_OK)
//             {
//                 *p_value += (double)delta_x;
//                 showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//             }
//             if (is_show_num)
//             {
//                 showstr(0, (SON_NUM + 1) * DIS_Y, "            ");
//                 showfloat(0, (SON_NUM + 1) * DIS_Y, *p_value, num, point_num);
//             }
//         }
//         else if (type == TYPE_INT)
//         {
//             int *p_value;
//             p_value = (int *)(value);
//             if (IS_OK)
//             {
//                 *p_value += (int)delta_x;
//                 showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//             if (is_show_num)
//             {
//                 showstr(0, (SON_NUM + 1) * DIS_Y, "        ");
//                 showint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//         }
//         else if (type == TYPE_UINT16)
//         {
//             uint16 *p_value;
//             p_value = (uint16 *)(value);
//             if (IS_OK)
//             {
//                 *p_value += (int)delta_x;
//                 showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//             if (is_show_num)
//             {
//                 showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//                 showuint16(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//         }
//         else if (type == TYPE_UINT32)
//         {
//             uint32 *p_value;
//             p_value = (uint32 *)(value);
//             if (IS_OK)
//             {
//                 *p_value += (int)delta_x;
//                 showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//             if (is_show_num)
//             {
//                 showstr(0, (SON_NUM + 1) * DIS_Y, "         ");
//                 showuint32(0, (SON_NUM + 1) * DIS_Y, *p_value, num);
//             }
//         }
//     }
//     last_index = p_unit->m_index[1];
// }

//// �Ƿ�Ϊ��һ�ν�����ҳ��
// void is_first_in_page()
//{
//     first_in_page_flag = (p_unit_last != p_unit) && (button1 || button2);
// }

// void show_menu()
//{
//     if (p_unit->m_index[1] == 255)
//         center_menu(); // ��ʾ���˵�
//     else
//         assist_menu(); // ��ʾ�Ӳ˵�
// }

//// Ч������
// void fun_menu()
//{
//     if (p_unit->type_t == NORMAL_PAR || p_unit->type_t == PID_PAR)
//     {
//         change_value(p_unit->par_set);
//     }
//     else
//     {
//         current_operation_menu = p_unit->current_operation; // ����ָ��
//         (*current_operation_menu)();
//     }
// }

//// ͼ����ʾ����
// void handle_image_display()
//{
//     if (mt9v03x_finish_flag)
//     {
//         // ����ͼ������
//         memcpy(image_copy, mt9v03x_image, MT9V03X_H * MT9V03X_W);
//
//         // ��ʾԭʼͼ��
//         ips200_show_gray_image(0, 0, (const uint8 *)image_copy,
//                               MT9V03X_W, MT9V03X_H,
//                               MT9V03X_W, MT9V03X_H, 0);
//
//         // ��ʾ�����Ķ�ֵ��ͼ��
//         ips200_show_gray_image(0, 120, (const uint8 *)binaryImage,
//                               MT9V03X_W, MT9V03X_H,
//                               MT9V03X_W, MT9V03X_H, 0);
//
//         // ��ʾ�˳���ʾ
//         ips200_show_string(10, 240, "Press BACK to exit");
//
//         mt9v03x_finish_flag = 0;
//     }
//
//
//     // ��ⷵ�ؼ�
//     if (button1) {
//         display_mode = 0;  // �˳�ͼ��ģʽ
//         is_clear_flag = 1; // ����������־
//         assist_menu();     // ˢ�²˵���ʾ
//     }
// }

//// ������������ʾ����
// void handle_sensor_display()
//{
//     // ���´��������ݣ�����ʹ��ģ�����ݣ�
//
//
//     // ��������ʾ����
//
//     ips200_show_string(10, 10, "Sensor Parameters");
//     ips200_draw_line(10, 30, 230, 30, RGB565_BLUE);
//     ips200_show_string(0,50,"roll:");
//		ips200_show_string(0,70,"pitch:");
//		ips200_show_string(0,90,"yaw:");
//    	ips200_show_float(60,50,roll,4,4);
//		ips200_show_float(60,70,pitch,4,4);
//		ips200_show_float(60,90,yaw,4,4);
//
//
//     // ��ʾ�˳���ʾ
//     ips200_show_string(10, 240, "Press BACK to exit");
//
//     // ��ⷵ�ؼ�
//     if (button1) {
//         display_mode = 0;  // �˳���������ʾģʽ
//         is_clear_flag = 1; // ����������־
//         assist_menu();     // ˢ�²˵���ʾ
//     }
// }

//// ����������
// void handle_launch_program()
//{
//     static uint32_t counter = 0;
//
//     switch(launch_state) {
//         case 0: // ׼���׶�
//             ips200_clear();
//             ips200_show_string(10, 10, "Launch Program");
//             ips200_draw_line(10, 30, 230, 30, RGB565_RED);
//             ips200_show_string(15, 50, "Ready to launch");
//             ips200_show_string(15, 80, "Press OK to start");
//
//             if (button2) {
//                 launch_state = 1;
//                 countdown = 5;
//                 counter = 0; // ���ü�����
//             }
//             break;
//
//         case 1: // ����ʱ�׶�
//             ips200_clear();
//             ips200_show_string(10, 10, "Launch Program");
//             ips200_draw_line(10, 30, 230, 30, RGB565_RED);
//
//             char count_str[30];
//             sprintf(count_str, "Launch in: %d", countdown);
//             ips200_show_string(50, 80, count_str);
//
//             // �򵥵���ʱ����ʹ�ü�����ģ��ʱ�䣩
//             counter++;
//             if (counter >= 100) { // ��Լ1�루����ÿ10ms����һ�Σ�
//                 counter = 0;
//                 if (countdown > 0) {
//                     countdown--;
//                 } else {
//                     launch_state = 2; // ���뷢��״̬
//                 }
//             }
//             break;
//
//         case 2: // ����״̬
//             ips200_clear();
//             ips200_show_string(10, 10, "Launch Program");
//             ips200_draw_line(10, 30, 230, 30, RGB565_GREEN);
//             ips200_show_string(15, 50, "Launching!");
//
//             // �������ʵ�ʵķ������ƴ���
//             // ���磺motor_control(100); // ���Ƶ���ٶ�
//
//             // ��ʾ״̬��Ϣ
//             ips200_show_string(15, 80, "Running...");
//             ips200_show_string(15, 100, "Speed: 100%");
//             break;
//     }
//
//     // ��ʾ�˳���ʾ
//     ips200_show_string(10, 240, "Press BACK to exit");
//
//     // ��ⷵ�ؼ�
//     if (button1) {
//         display_mode = 0;  // �˳�����ģʽ
//         launch_state = 0;  // ����״̬
//         is_clear_flag = 1; // ����������־
//
//         // �������ֹͣ�����Ŀ��ƴ���
//         // ���磺motor_control(0); // ֹͣ���
//
//         assist_menu();     // ˢ�²˵���ʾ
//     }
// }

//// ��ʾ���̹��Ӻ���
// void show_process(void *parameter)
//{
// #ifdef MENU_USE_RTT
//     while (1)
//     {
//         // ��ȡ�����ź���
//         button1 = (RT_EOK == rt_sem_take(key1_sem, RT_WAITING_NO));
//         button2 = (RT_EOK == rt_sem_take(key2_sem, RT_WAITING_NO));
//         button3 = (RT_EOK == rt_sem_take(key3_sem, RT_WAITING_NO));
//         button4 = (RT_EOK == rt_sem_take(key4_sem, RT_WAITING_NO));

//        // ���ݵ�ǰģʽִ�в�ͬ����
//        switch (display_mode) {
//            case 1: // ͼ����ʾģʽ
//                handle_image_display();
//                break;
//
//            case 2: // ������������ʾģʽ
//                handle_sensor_display();
//                break;
//
//            case 3: // ��������ģʽ
//                handle_launch_program();
//                break;
//
//            default: // �����˵�ģʽ
//                is_clear_flag = is_menu_clear();

//                if (button1 || button2 || button3 || button4)
//                {
//                    rt_sem_release(button_feedback_sem);
//                }

//                if (is_clear_flag)
//                    clear_hhh(0, 0, SCREEN_W, SON_NUM * 16, IPS200_BGCOLOR);

//                if (button1 == 1)
//                    p_unit = p_unit->back;
//                else if (button2 == 1)
//                    p_unit = p_unit->enter;
//                else if (button3 == 1)
//                    p_unit = p_unit->up;
//                else if (button4 == 1)
//                    p_unit = p_unit->down;

//                is_first_in_page();

//                // ��ʾ����
//                show_menu();

//                // Ч������
//                fun_menu();

//                p_unit_last = p_unit;
//                break;
//        }
//
//        // �̹߳���
//        rt_thread_mdelay(10);
//    }
// #else
//    // ��������µİ�������
//    button_entry(NULL);
//
//    // ���ݵ�ǰģʽִ�в�ͬ����
//    switch (display_mode) {
//        case 1: // ͼ����ʾģʽ
//            handle_image_display();
//            break;
//
//        case 2: // ������������ʾģʽ
//
//            handle_sensor_display();
//            break;
//
//        case 3: // ��������ģʽ
//            handle_launch_program();
//            break;
//
//        default: // �����˵�ģʽ
//            is_clear_flag = is_menu_clear();

//            if (is_clear_flag)
//                clear();

//            if (button1 == 1)
//                p_unit = p_unit->back;
//            else if (button2 == 1)
//                p_unit = p_unit->enter;
//            else if (button3 == 1)
//                p_unit = p_unit->up;
//            else if (button4 == 1)
//                p_unit = p_unit->down;

//            is_first_in_page();

//            // ��ʾ����
//            show_menu();

//            // Ч������
//            fun_menu();

//            p_unit_last = p_unit;
//            break;
//    }
// #endif
//}

//// �˵�ϵͳ��ʼ��(��˳��)
// void menu_init()
//{
//     /*---------------��Ļ��ʼ��----------------*/
//     screen_init(IPS200_TYPE_SPI);

//    /*---------------������ʼ��----------------*/
//    key_into();

//    /*---------------���޸Ĳ���----------------*/
//    UNIT_SET();

//    /*---------------����Ļص�����----------------*/
//    FUN_INIT();

//    /*---------------�˵����ֳ�ʼ��----------------*/
//    dad_name_init();

//    /*---------------�ַ���������ʼ��----------------*/
//    index_xy_init();

///*-----------------����flash---------------*/
// #ifdef USE_FLASH
//     flash_init_wz();
// #endif

///*----------------�˵��̳߳�ʼ��----------------*/
// #ifdef MENU_USE_RTT
//     rt_thread_t tid;
//     // ������ʾ�߳�
//     tid = rt_thread_create("display", show_process, RT_NULL, 1024 * 2, 11, 5);
//     // ������ʾ�߳�
//     if (RT_NULL != tid)
//     {
//         rt_thread_startup(tid);
//     }
// #endif
// }

//// ����ҹ������ģʽ
// static uint16 IPS200_BGCOLOR = RGB565_WHITE;
// void day_night()
//{
//     if (IS_OK)
//     {
//         if (IPS200_BGCOLOR == RGB565_WHITE)
//         {
//             IPS200_BGCOLOR = RGB565_BLACK;
//             ips200_set_color(RGB565_WHITE, RGB565_BLACK);
//             showstr(0, (SON_NUM + 1) * 16, "BLACK");
//         }
//         else if (IPS200_BGCOLOR == RGB565_BLACK)
//         {
//             IPS200_BGCOLOR = RGB565_WHITE;
//             ips200_set_color(RGB565_BLACK, RGB565_WHITE);
//             showstr(0, (SON_NUM + 1) * 16, "WHITE");
//         }
//     }
// }
//// ���ı�����ɫ
// void rand_color()
//{
//     if (IS_OK)
//     {
//         uint16 color;
//         color = rand() % (32768 * 2);
//         ips200_set_color(color, ~color);
//         showstr(0, (SON_NUM + 1) * 16, "rand");
//     }
// }

///*
// �������û��Լ����Զ��庯��
// 1.�˵�����ͼ����ʾ�ͳ��������ʾ����
//*/

// void photo_display()
//{
//     if (IS_OK)
//     {
//         display_mode = 1;  // ����ͼ����ʾģʽ
//         ips200_clear();    // ����
//     }
// }

//// ���봫����������ʾ����
// void enter_sensor_display()
//{
//     if (IS_OK) {
//         display_mode = 2;  // ���봫������ʾģʽ
//         ips200_clear();    // ����
//     }
// }

//// ���뷢���������
// void enter_launch_program()
//{
//     if (IS_OK) {
//         display_mode = 3;  // ���뷢������ģʽ
//         ips200_clear();    // ����
//     }
// }

// void NULL_FUN()
//{
// }

// float test_a = 1.1f;
// int test_b = 100;
// double test_c = 100;
// uint16 test_d = 20;
// uint32 test_e = 32;

// void UNIT_SET()
//{
//     // �˵���Ԫ���β�����ʼ��
//     unit_param_set(&test_a, TYPE_DOUBLE, 0.5, 3, 3, NORMAL_PAR, "test_a");
//     unit_param_set(&test_b, TYPE_DOUBLE, 2, 6, 3, NORMAL_PAR, "test_b");
//     unit_param_set(&test_c, TYPE_DOUBLE, 11.11, 4, 4, NORMAL_PAR, "test_c");
//     unit_param_set(&test_d, TYPE_UINT16, 1, 6, 0, NORMAL_PAR, "test_d");
//     unit_param_set(&test_e, TYPE_UINT32, 1, 6, 0, NORMAL_PAR, "test_e");
// }

// void FUN_INIT()
//{
//     // �˵���Ԫ����ָ���ʼ��
//     fun_init(NULL_FUN, "NULL_FUN1");
//     fun_init(NULL_FUN, "NULL_FUN2");
//     fun_init(day_night, "day_night");
//     fun_init(rand_color, "rand_color");
//     fun_init(photo_display, "photo_dis");
//     fun_init(enter_sensor_display, "Sensor Data");     // ����������������ʾ���
//     fun_init(enter_launch_program, "Launch Program"); // ���������������
// }
