#include "zf_common_headfile.h"
#include "Image Binarization.h"

// ʹ������ͷʵ�ʳߴ綨��
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH MT9V03X_W

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������

// �Ż����򷨶�ֵ��������ʹ�õ���������ǰ��ֹ��
int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH])
{
    unsigned int histogram[256] = {0}; // �Ҷ�ֱ��ͼ
    unsigned int total_pixels = (unsigned int)IMAGE_HEIGHT * IMAGE_WIDTH;
    unsigned long long sum = 0;         // �ܻҶ�ֵ
    unsigned long long sumB = 0;        // ������Ҷ��ܺ�
    unsigned int wB = 0;                // ������������
    unsigned long long maxVariance = 0; // �����䷽��
    int threshold = 0;                  // �����ֵ
    int declineCount = 0;               // �����½�������
    const int maxDecline = 2;           // ������������½�����

    // ����1: ����Ҷ�ֱ��ͼ���ܻҶ�ֵ
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            unsigned char pixel = image[i][j];
            histogram[pixel]++;
            sum += pixel;
        }
    }

    // ����2: �������ܵ���ֵ�����õ���������ǰ��ֹ��
    for (int t = 0; t < 256; t++)
    {
        if (histogram[t] == 0)
            continue;

        wB += histogram[t];
        if (wB == 0)
            continue;

        unsigned int wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += (unsigned long long)t * histogram[t];

        // ���㱳����ǰ����ƽ���Ҷ�
        unsigned long long mB = sumB / wB;
        unsigned long long mF = (sum - sumB) / wF;

        // ������䷽��
        unsigned long long variance = (unsigned long long)wB * wF * (mB - mF) * (mB - mF);

        // ��ⷽ���½����ƣ����õ������ԣ�
        if (variance >= maxVariance)
        {
            // ���ָ���ķ���������ֵ
            maxVariance = variance;
            threshold = t;
            declineCount = 0; // �����½�������
        }
        else
        {
            // ���ʼ�½�
            declineCount++;

            // ��������½�����������ֵ����ǰ��ֹѭ��
            if (declineCount > maxDecline)
            {
                break;
            }
        }
    }

    return threshold;
}

// Ӧ����ֵ���ж�ֵ��
void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH],
                    unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH],
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

void image_output()
{
    // 1. ��������ֵ��ֱ��ʹ��ԭʼͼ��
    int threshold = otsuThreshold(image_copy);

    // 2. Ӧ����ֵ���ж�ֵ��
    applyThreshold(image_copy, binaryImage, threshold);

    // 3. ����binaryImage����������
    // ����������ʾ����������
}