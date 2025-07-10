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






