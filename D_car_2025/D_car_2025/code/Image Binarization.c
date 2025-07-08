#include "zf_common_headfile.h"
#include "Image Binarization.h"

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];      // ����������������
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // �߽�ͼ��
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // ������ͼ��
int center_points[IMAGE_HEIGHT];                   // �洢ÿ�е����ĵ�����

// ��򷨣�������ֵ��
//--------------------------------------------------------------
int otsuThreshold(uint8 image[][IMAGE_WIDTH])
{
    uint32 histogram[256] = {0}; // �Ҷ�ֱ��ͼ
    uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
    uint64 sum = 0;           // �ܻҶ�ֵ
    uint64 sumB = 0;          // ������Ҷ��ܺ�
    uint32 wB = 0;            // ������������
    uint64 maxVariance = 0;   // �����䷽��
    int threshold = 0;        // �����ֵ
    int declineCount = 0;     // �����½�������
    const int maxDecline = 2; // ������������½�����

    // ����Ҷ�ֱ��ͼ���ܻҶ�ֵ
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            uint8 pixel = image[i][j];
            histogram[pixel]++;
            sum += pixel;
        }
    }

    // �������ܵ���ֵ�����õ���������ǰ��ֹ��
    for (int t = 0; t < 256; t += 2) // ���2�����㣬��������ٶ�
    {
        if (histogram[t] == 0)
            continue;

        wB += histogram[t];
        if (wB == 0)
            continue;

        uint32 wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += (uint64)t * histogram[t];

        // ���㱳����ǰ����ƽ���Ҷ�
        uint64 mB = sumB / wB;
        uint64 mF = (sum - sumB) / wF;

        // ������䷽��
        uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

        // ��ⷽ���½�����
        if (variance >= maxVariance)
        {
            maxVariance = variance;
            threshold = t;
            declineCount = 0; // �����½�������
        }
        else
        {
            declineCount++;
            if (declineCount > maxDecline)
            {
                break;
            }
        }
    }

    return threshold;
}

//  Ӧ����ֵ���������ɶ�ֵ��ͼ��
//--------------------------------------------------------------
void applyThreshold(uint8 input[][IMAGE_WIDTH],
                    uint8 output[][IMAGE_WIDTH],
                    int threshold)
{
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            // ʹ���������ʽ�Ż���ֵ��
            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
        }
    }
}

// ������߽����������߼��㺯��
void find_boundaries_and_centerline()
{
    // ��ʼ��ͼ��
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            boundary_image[y][x] = 0;
            centerline_image[y][x] = 0;
        }
        center_points[y] = -1; // ��ʼ��Ϊ��Чֵ
    }

    // ����������������
    const int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // ֻ����ͼ���м����򣬱���߽���
    for (int y = 1; y < IMAGE_HEIGHT - 1; y++)
    {
        int left_boundary = -1;
        int right_boundary = -1;

        // Ѱ����߽�
        for (int x = 1; x < IMAGE_WIDTH - 1; x++)
        {
            if (binaryImage[y][x] == 255)
            {
                // �����������Ƿ��б�����
                for (int i = 0; i < 8; i++)
                {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0)
                    {
                        left_boundary = x;
                        boundary_image[y][x] = 255; // ���Ϊ�߽�
                        break;
                    }
                }
                if (left_boundary != -1)
                    break;
            }
        }

        // Ѱ���ұ߽�
        for (int x = IMAGE_WIDTH - 2; x > 0; x--)
        {
            if (binaryImage[y][x] == 255)
            {
                // �����������Ƿ��б�����
                for (int i = 0; i < 8; i++)
                {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0)
                    {
                        right_boundary = x;
                        boundary_image[y][x] = 255; // ���Ϊ�߽�
                        break;
                    }
                }
                if (right_boundary != -1)
                    break;
            }
        }

        // ����������
        if (left_boundary != -1 && right_boundary != -1)
        {
            int center_x = (left_boundary + right_boundary) / 2;
            center_points[y] = center_x; // �洢���ĵ�

            // ��������ͼ���ϱ�����ĵ㣨��2���ط�Χ�ڣ�
            for (int offset = -2; offset <= 2; offset++)
            {
                int cx = center_x + offset;
                if (cx >= 0 && cx < IMAGE_WIDTH)
                {
                    centerline_image[y][cx] = 128; // ��ɫ���
                }
            }
        }
    }
}



/*=====================================================================================*/


uint8 Reference_Point = 0;           // ��̬�ο���
uint8 Reference_Col = 0;             // ��̬�ο���
uint8 White_Max_Point = 0;           // ��̬�׵����ֵ
uint8 White_Min_Point = 0;           // ��̬�׵���Сֵ
uint8 Reference_Contrast_Ratio = 20; // �ο��Աȶ�

uint8 Remote_Distance[Search_Image_W] = {0}; // �׵�Զ�˾���
uint8 Left_Edge_Line[Search_Image_H] = {0};  // ��߽�
uint8 Right_Edge_Line[Search_Image_H] = {0}; // �ұ߽�

/*******************************************************************************
 * �� �� �� * : LimitDouMid
 * �������� * :  ��value�޷���limit1��limit2֮��
 *******************************************************************************/
int32 LimitDouMid(int32 value, int32 limit1, int32 limit2)
{
    int32 buf = 0;

    if (limit1 > limit2) // Ҫ��limit1 < limit2
    {
        buf = limit1;
        limit1 = limit2;
        limit2 = buf;
    }

    if (value < limit1)
        return limit1;
    else if (value > limit2)
        return limit2;
    else
        return value;
}

/*******************************************************************************
 * �� �� �� * : Find_extreme_value
 * �������� * :  ����һά�������ֵ����Сֵ
 *  *va     :����ָ��
 *  num0    :��ʼ��
 *  num1    :������
 *  model   : 1 �������ֵ    0  ������Сֵ
 *******************************************************************************/
int Find_extreme_value(uint8 *va, uint8 num0, uint8 num1, uint8 model)
{
    uint8 i = 0, temp = 0, temp1 = 0, temp2 = 0, value = 0;
    if (num0 > num1)
    {
        temp1 = num0 - num1;
        temp2 = num1;
        va += num0;
        value = *va;
        if (model)
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va - i);
                if (temp > value)
                {
                    temp2 = num0 - i;
                    value = temp;
                }
            }
        }
        else
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va - i);
                if (temp < value)
                {
                    temp2 = num0 - i;
                    value = temp;
                }
            }
        }
    }
    else
    {
        temp1 = num1 - num0;
        temp2 = num0;
        va += num0;
        value = *va;
        if (model)
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va + i);
                if (temp > value)
                {
                    temp2 = i + num0;
                    value = temp;
                }
            }
        }
        else
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va + i);
                if (temp < value)
                {
                    temp2 = i + num0;
                    value = temp;
                }
            }
        }
    }
    return temp2;
}

// ��ȡͼ��ο��㣬�׵����ֵ���׵���Сֵ
void Get_Reference_Point(void)
{
    uint8 *p = mt9v03x_image[Search_Image_H - REFRENCEROW]; // ͼ������ָ��ָ���ͳ�Ƶ���ʼ��
    uint16 temp = 0;                                        // ����ͳ�Ƶ�������
    uint32 temp1 = 0;                                       // ��������ͳ�Ƶ�������ĺ�
    temp = REFRENCEROW * Search_Image_W;                    // �����ͳ�Ƶ�������
    for (int i = 0; i < temp; i++)
        temp1 += *(p + i);                   // ͳ�Ƶ����
    Reference_Point = (uint8)(temp1 / temp); // �����ƽ��ֵ����Ϊ����ͼ��Ĳο���

    // ���ݲο������׵����ֵ����Сֵ���޷��ںڵ���ֵ��255֮�䣩
    White_Max_Point = (uint8)LimitDouMid((int32)Reference_Point * WHITEMAXMUL / 10, BLACKPOINT, 255);
    White_Min_Point = (uint8)LimitDouMid((int32)Reference_Point * WHITEMINMUL / 10, BLACKPOINT, 255);
}

// ����ÿ�а׵���뼰ͼ��ο��У�������������
void Search_Reference_Col(void)
{
    int col, row;
    uint8 temp1 = 0, temp2 = 0;
    int temp3 = 0;
    uint8 *p = mt9v03x_image[0]; // ͼ������ָ��
    for (col = 0; col < Search_Image_W; col++)
    {
        for (row = Search_Image_H - 1; row > STOPROW; row--)
        {

            temp1 = *(p + row * Search_Image_W + col);             // ��ȡ��ǰ��Ҷ�ֵ
            temp2 = *(p + (row - STOPROW) * Search_Image_W + col); // ��ȡ�Աȵ�Ҷ�ֵ

            if (temp2 > White_Max_Point)
                continue; // �ж϶Աȵ��Ƿ�Ϊ�׵� ��Ϊ�׵���ֱ������

            if (temp1 < White_Min_Point)
            {
                Remote_Distance[col] = row - 1;
                break;
            } // �жϵ�ǰ���Ƿ�Ϊ�ڵ� ��Ϊ�ڵ��򲻽��жԱ�ֱ�Ӹ�ֵ

            temp3 = (temp1 - temp2) * 200 / (temp1 + temp2); // ����Աȶ�
            // �ж϶Աȶ��Ƿ������ֵ ���������ֵ����Ϊ�������߽� �����Ѿ�������������߽磬��ֱ�Ӹ�ֵ�߽�
            if (temp3 > Reference_Contrast_Ratio || row == STOPROW + 1)
            {
                Remote_Distance[col] = row - 1;
                break;
            }
        }
    }
    Reference_Col = (uint8)Find_extreme_value(Remote_Distance, 10, Search_Image_W - 10, 0); // �����Զ�İ׵�����Ϊ�ο���
    Reference_Col = (uint8)LimitDouMid(Reference_Col, 1, Search_Image_W - 2);               // �ο����޷� ��ֹ�������
}

// ���������߽�
void Search_line(void)
{
    uint8 *p = mt9v03x_image[0];                    // ͼ������ָ��
    uint8 row_max = Search_Image_H - STOPROW;       // �����ֵ
    uint8 row_min = STOPROW;                        // ����Сֵ
    uint8 col_max = Search_Image_W - 3;             // �����ֵ
    uint8 col_min = 3;                              // ����Сֵ
    int16 leftstartcol = Reference_Col;             // ��������ʼ��
    int16 rightstartcol = Reference_Col;            // ��������ʼ��
    int16 leftendcol = 0;                           // ��������ֹ��
    int16 rightendcol = Search_Image_W;             // ��������ֹ��
    uint8 temp1 = 0, temp2 = 0;                     // ��ʱ����  ���ڴ洢ͼ������
    int temp3 = 0;                                  // ��ʱ����  ���ڴ洢�Աȶ�
    int leftstop = 0, rightstop = 0, stoppoint = 0; // ������������

    int col, row;

    for (row = row_max; row > row_min; row--)
    {
        Left_Edge_Line[row] = col_min - CONTRASTOFFSET;
        Right_Edge_Line[row] = col_max + CONTRASTOFFSET;
    }

    for (row = row_max; row > row_min; row--)
    {
        p = &mt9v03x_image[row][0]; // ��ȡ�������λ��ָ��
			
			
        if (!leftstop)
        {
            for (col = leftstartcol; col >= leftendcol; col--)
            {
                temp1 = *(p + col);                  // ��ȡ��ǰ��Ҷ�ֵ
                temp2 = *(p + col - CONTRASTOFFSET); // ��ȡ�Աȵ�Ҷ�ֵ
                // �жϲο����Ƿ�Ϊ�ڵ� ��Ϊ�ڵ����������
                if (temp1 < White_Min_Point && col == leftstartcol && leftstartcol == Reference_Col)
                {
                    leftstop = 1; // ���������� ���ڽ������������
                    for (stoppoint = row; stoppoint >= 0; stoppoint--)
                        Left_Edge_Line[stoppoint] = col_min; // ���ʣ�����Ч�����
                    break;
                }
                // �ж���������ʼ���Ƿ�Ϊ�ڵ� ��Ϊ�ڵ�������ȷ����ʼ��Χ
                if ((temp1 < White_Min_Point && col == leftstartcol) || (leftstartcol != Reference_Col && col == col_min + 1))
                {
                    if (leftstartcol < Reference_Col)
                    {
                        col = Reference_Col;
                        leftstartcol = Reference_Col;
                    }
                }
                // �ж���������ֹ���Ƿ�Ϊ�׵� ��Ϊ�׵�������ȷ��������Χ
                if (temp1 > White_Min_Point && col == leftendcol)
                {
                    if (leftendcol > col_min)
                    {
                        leftendcol = col_min;
                    }
                }
                // �жϵ�ǰ���Ƿ�Ϊ�ڵ� ��Ϊ�ڵ��򲻽��жԱ�ֱ�Ӹ�ֵ
                if (temp1 < White_Min_Point && leftstartcol == Reference_Col)
                {
                    Left_Edge_Line[row] = (uint8)col;
                    break;
                }
                // �ж϶Աȵ��Ƿ�Ϊ�׵� ��Ϊ�׵���ֱ������
                if (temp2 > White_Max_Point)
                    continue;
                // ����Աȶ�
                temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
                // �ж϶Աȶ��Ƿ������ֵ ���������ֵ����Ϊ���б߽�  �����Ѿ�������������߽磬��ֱ�Ӹ�ֵ�б߽�
                if (temp3 > Reference_Contrast_Ratio || col == col_min + 1)
                {
                    Left_Edge_Line[row] = col - CONTRASTOFFSET + 1; // ���浱ǰ����߽�
                    // ˢ�����߷�Χ
                    // leftstartcol = (uint8)LimitDouMid(col + SEARCHRANGE, col, Reference_Col);
                    leftendcol = (uint8)LimitDouMid(col - SEARCHRANGE, col_min, col);
                    break;
                }
            }
        }
        if (!rightstop)
        {
            for (col = rightstartcol; col <= rightendcol; col++)
            {
                temp1 = *(p + col);                  // ��ȡ��ǰ��Ҷ�ֵ
                temp2 = *(p + col + CONTRASTOFFSET); // ��ȡ�Աȵ�Ҷ�ֵ
                // �жϲο����Ƿ�Ϊ�ڵ� ��Ϊ�ڵ����������
                if (temp1 < White_Min_Point && col == rightstartcol && rightstartcol == Reference_Col)
                {
                    rightstop = 1; // ���������� ���ٽ����ұ�������
                    for (stoppoint = row; stoppoint >= 0; stoppoint--)
                        Right_Edge_Line[stoppoint] = col_max; // ���ʣ�����Ч�ұ���
                    break;
                }
                // �ж���������ʼ���Ƿ�Ϊ�ڵ� ��Ϊ�ڵ�������ȷ�����߷�Χ
                if ((temp1 < White_Min_Point && col == rightstartcol) || (rightstartcol != Reference_Col && col == col_max - 1))
                {
                    if (rightstartcol < Reference_Col)
                    {
                        col = Reference_Col;
                        rightstartcol = Reference_Col;
                    }
                }
								
                // �ж���������ֹ���Ƿ�Ϊ�׵� ��Ϊ�׵�������ȷ��������Χ
                if (temp1 < White_Max_Point && col == rightendcol)
                {
                    if (rightendcol > col_max)
                        rightendcol = col_max;
                }
								
                // �жϵ�ǰ���Ƿ�Ϊ�ڵ� ��Ϊ�ڵ��򲻽��жԱ�ֱ�Ӹ�ֵ
                if (temp1 < White_Min_Point && rightstartcol == Reference_Col)
                {
                    Right_Edge_Line[row] = (uint8)col;
                    break;
                }
								
                // �жϴ��Աȵ��Ƿ�Ϊ�׵� ��Ϊ�׵���ֱ������
                if (temp2 > White_Max_Point)
								{
                    continue;
								}
                // ����Աȶ�
                temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
                // �ж϶Աȶ��Ƿ������ֵ ���������ֵ����Ϊ���б߽�  �����Ѿ�������������߽磬��ֱ�Ӹ�ֵ�б߽�
                if (temp3 > Reference_Contrast_Ratio || col == col_max - 1)
                {
                    Right_Edge_Line[row] = col + CONTRASTOFFSET - 1; // ���浱ǰ���ұ߽�
                    // ˢ�����߷�Χ
                    // rightstartcol = (uint8)LimitDouMid(col - SEARCHRANGE, Reference_Col, col);
                    rightendcol = (uint8)LimitDouMid(col + SEARCHRANGE, col, col_max);
                    break;
                }
            }
        }
    }
}




// �����ұ߽��������ϵ��߽�ͼ������
// boundary_image�У��߽���255��ǣ���������Ϊ0
void IntegrateBoundaryToImage(void)
{
    uint8 row, col;
    
    // ��ʼ���߽�ͼ��Ϊȫ�ڣ�0��
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        for (col = 0; col < IMAGE_WIDTH; col++)
        {
            boundary_image[row][col] = 0;
        }
    }
    
    // �����߽磨ʹ�ûҶ�ֵ128��
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        // ȷ���߽�����������Ч��Χ��
        if (Left_Edge_Line[row] >= 0 && Left_Edge_Line[row] < IMAGE_WIDTH)
        {
            // �ڱ߽�λ�ñ�ǣ����ʵ���չΪ������ص���ǿ��ʾ
            boundary_image[row][Left_Edge_Line[row]] = 255;
            
            // ��ѡ�������Ҹ���չ1�����أ��ñ߽������
            if (Left_Edge_Line[row] + 1 < IMAGE_WIDTH)
                boundary_image[row][Left_Edge_Line[row] + 1] = 255;
            if (Left_Edge_Line[row] - 1 >= 0)
                boundary_image[row][Left_Edge_Line[row] - 1] = 255;
        }
    }
    
    // ����ұ߽磨ʹ�ûҶ�ֵ255��
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        // ȷ���߽�����������Ч��Χ��
        if (Right_Edge_Line[row] >= 0 && Right_Edge_Line[row] < IMAGE_WIDTH)
        {
            // �ڱ߽�λ�ñ�ǣ����ʵ���չΪ������ص���ǿ��ʾ
            boundary_image[row][Right_Edge_Line[row]] = 255;
            
            // ��ѡ�������Ҹ���չ1�����أ��ñ߽������
            if (Right_Edge_Line[row] + 1 < IMAGE_WIDTH)
                boundary_image[row][Right_Edge_Line[row] + 1] = 255;
            if (Right_Edge_Line[row] - 1 >= 0)
                boundary_image[row][Right_Edge_Line[row] - 1] = 255;
        }
    }
}
//  ����������
//--------------------------------------------------------------
void image_output()
{

    // ����3: ��������ֵ
    int threshold = otsuThreshold(image_copy);

    // ����4: Ӧ����ֵ��ֵ��
    applyThreshold(image_copy, binaryImage, threshold);
    //find_boundaries_and_centerline();
	
	
	
	Get_Reference_Point();
	Search_Reference_Col();
	Search_line();
	IntegrateBoundaryToImage();
}
