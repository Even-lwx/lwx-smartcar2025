#include "Image Binarization.h"

#include "zf_device_mt9v03x.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


extern uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];


// ͼ��ߴ綨�壨����ʵ������ͷ�ֱ����޸ģ�
#define IMAGE_HEIGHT 120
#define IMAGE_WIDTH  188

// ��򷨶�ֵ������
int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH]) {
    int histogram[256] = {0};  // �Ҷ�ֱ��ͼ
    int total_pixels = IMAGE_HEIGHT * IMAGE_WIDTH;
    double sum = 0.0;          // �ܻҶ�ֵ
    double sumB = 0.0;         // ������Ҷ��ܺ�
    int wB = 0;                // ������������
    int wF = 0;                // ǰ����������
    double maxVariance = 0.0;   // �����䷽��
    int threshold = 0;          // �����ֵ
    
    // ����1: ����Ҷ�ֱ��ͼ
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            unsigned char pixel = image[i][j];
            histogram[pixel]++;
            sum += (double)pixel;
        }
    }
    
    // ����2: �������п��ܵ���ֵ
    for (int t = 0; t < 256; t++) {
        wB += histogram[t];  // ����������������
        if (wB == 0) continue;
        
        wF = total_pixels - wB;  // ǰ����������
        if (wF == 0) break;
        
        sumB += (double)(t * histogram[t]);  // ������Ҷ��ܺ�
        
        double mB = sumB / wB;              // ������ƽ���Ҷ�
        double mF = (sum - sumB) / wF;      // ǰ����ƽ���Ҷ�
        
        // ������䷽��
        double variance = (double)wB * (double)wF * (mB - mF) * (mB - mF);
        
        // ���������ֵ
        if (variance > maxVariance) {
            maxVariance = variance;
            threshold = t;
        }
    }
    
    return threshold;
}

// Ӧ����ֵ���ж�ֵ��
void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH], 
                   unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH], 
                   int threshold) {
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
        }
    }
}

// �򵥵�3x3��ֵ�˲�����ѡ��
void meanFilter(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH], 
               unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH]) {
    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
            int sum = 0;
            // 3x3�������
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    sum += input[i + m][j + n];
                }
            }
            output[i][j] = sum / 9;
        }
    }
}































