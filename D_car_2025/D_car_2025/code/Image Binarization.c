#include "zf_common_headfile.h"
#include <math.h>
#include "Image Binarization.h"

uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];      // ����������������
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // �߽�ͼ��
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // ������ͼ��
int center_points[IMAGE_HEIGHT];                   // �洢ÿ�е����ĵ�����

/**
 * @brief  otsu�㷨��ȡ��ֵ
 * @param  *image ͼ������ָ��
 * @param  col ͼ����
 * @param  row ͼ��߶�
 * @retval ��ֵ
 */
int otsu_get_threshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    int threshold = 0;
    uint8 *data = image;
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum = 0;
    // ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i * width + j]]++; // ����ǰ�ĵ������ֵ��Ϊ����������±�
            gray_sum += (int)data[i * width + j];   // �Ҷ�ֵ�ܺ�
        }
    }
    // ����ÿ������ֵ�ĵ�������ͼ���еı���
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    // �����Ҷȼ�[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];        // ��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        u0tmp += j * pixelPro[j]; // �������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ
        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;
        u0 = u0tmp / w0;   // ����ƽ���Ҷ�
        u1 = u1tmp / w1;   // ǰ��ƽ���Ҷ�
        u = u0tmp + u1tmp; // ȫ��ƽ���Ҷ�
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
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

// // ��򷨣�������ֵ��
// //--------------------------------------------------------------
// int otsuThreshold(uint8 image[][IMAGE_WIDTH])
// {
//     uint32 histogram[256] = {0}; // �Ҷ�ֱ��ͼ
//     uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
//     uint64 sum = 0;           // �ܻҶ�ֵ
//     uint64 sumB = 0;          // ������Ҷ��ܺ�
//     uint32 wB = 0;            // ������������
//     uint64 maxVariance = 0;   // �����䷽��
//     int threshold = 0;        // �����ֵ
//     int declineCount = 0;     // �����½�������
//     const int maxDecline = 2; // ������������½�����

//     // ����Ҷ�ֱ��ͼ���ܻҶ�ֵ
//     for (int i = 0; i < IMAGE_HEIGHT; i++)
//     {
//         for (int j = 0; j < IMAGE_WIDTH; j++)
//         {
//             uint8 pixel = image[i][j];
//             histogram[pixel]++;
//             sum += pixel;
//         }
//     }

//     // �������ܵ���ֵ�����õ���������ǰ��ֹ��
//     for (int t = 0; t < 256; t += 2) // ���2�����㣬��������ٶ�
//     {
//         if (histogram[t] == 0)
//             continue;

//         wB += histogram[t];
//         if (wB == 0)
//             continue;

//         uint32 wF = total_pixels - wB;
//         if (wF == 0)
//             break;

//         sumB += (uint64)t * histogram[t];

//         // ���㱳����ǰ����ƽ���Ҷ�
//         uint64 mB = sumB / wB;
//         uint64 mF = (sum - sumB) / wF;

//         // ������䷽��
//         uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

//         // ��ⷽ���½�����
//         if (variance >= maxVariance)
//         {
//             maxVariance = variance;
//             threshold = t;
//             declineCount = 0; // �����½�������
//         }
//         else
//         {
//             declineCount++;
//             if (declineCount > maxDecline)
//             {
//                 break;
//             }
//         }
//     }

//     return threshold;
// }
