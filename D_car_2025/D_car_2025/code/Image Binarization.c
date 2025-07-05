// #include "zf_common_headfile.h"
// #include "Image Binarization.h"

//// ʹ������ͷʵ�ʳߴ綨��
// #define IMAGE_HEIGHT MT9V03X_H
// #define IMAGE_WIDTH MT9V03X_W

// extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
// unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������

//// �Ż����򷨶�ֵ��������ʹ�õ���������ǰ��ֹ��
// int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//     unsigned int histogram[256] = {0}; // �Ҷ�ֱ��ͼ
//     unsigned int total_pixels = (unsigned int)IMAGE_HEIGHT * IMAGE_WIDTH;
//     unsigned long long sum = 0;         // �ܻҶ�ֵ
//     unsigned long long sumB = 0;        // ������Ҷ��ܺ�
//     unsigned int wB = 0;                // ������������
//     unsigned long long maxVariance = 0; // �����䷽��
//     int threshold = 0;                  // �����ֵ
//     int declineCount = 0;               // �����½�������
//     const int maxDecline = 2;           // ������������½�����

//    // ����1: ����Ҷ�ֱ��ͼ���ܻҶ�ֵ
//    for (int i = 0; i < IMAGE_HEIGHT; i++)
//    {
//        for (int j = 0; j < IMAGE_WIDTH; j++)
//        {
//            unsigned char pixel = image[i][j];
//            histogram[pixel]++;
//            sum += pixel;
//        }
//    }

//    // ����2: �������ܵ���ֵ�����õ���������ǰ��ֹ��
//    for (int t = 0; t < 256; t++)
//    {
//        if (histogram[t] == 0)
//            continue;

//        wB += histogram[t];
//        if (wB == 0)
//            continue;

//        unsigned int wF = total_pixels - wB;
//        if (wF == 0)
//            break;

//        sumB += (unsigned long long)t * histogram[t];

//        // ���㱳����ǰ����ƽ���Ҷ�
//        unsigned long long mB = sumB / wB;
//        unsigned long long mF = (sum - sumB) / wF;

//        // ������䷽��
//        unsigned long long variance = (unsigned long long)wB * wF * (mB - mF) * (mB - mF);

//        // ��ⷽ���½����ƣ����õ������ԣ�
//        if (variance >= maxVariance)
//        {
//            // ���ָ���ķ���������ֵ
//            maxVariance = variance;
//            threshold = t;
//            declineCount = 0; // �����½�������
//        }
//        else
//        {
//            // ���ʼ�½�
//            declineCount++;

//            // ��������½�����������ֵ����ǰ��ֹѭ��
//            if (declineCount > maxDecline)
//            {
//                break;
//            }
//        }
//    }

//    return threshold;
//}

//// Ӧ����ֵ���ж�ֵ��
// void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     int threshold)
//{
//     for (int i = 0; i < IMAGE_HEIGHT; i++)
//     {
//         for (int j = 0; j < IMAGE_WIDTH; j++)
//         {
//             // ʹ���������ʽ�Ż���ֵ��
//             output[i][j] = (input[i][j] > threshold) ? 255 : 0;
//         }
//     }
// }

// void image_output()
//{
//     // 1. ��������ֵ��ֱ��ʹ��ԭʼͼ��
//     int threshold = otsuThreshold(image_copy);

//    // 2. Ӧ����ֵ���ж�ֵ��
//    applyThreshold(image_copy, binaryImage, threshold);

//    // 3. ����binaryImage����������
//    // ����������ʾ����������
//}

// #include "zf_common_headfile.h"
// #include "Image Binarization.h"

// ʹ������ͷʵ�ʳߴ綨��
// #define IMAGE_HEIGHT MT9V03X_H
// #define IMAGE_WIDTH MT9V03X_W

// extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
// unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������

// 1. Ԥ������ (���ǰ)
//--------------------------------------------------------------
// ������ֵ�˲� (3x3����)
// void medianFilter3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // ֻ�����ڲ����أ���Ե����ԭֵ
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // �ռ�3x3���ڵ�����ֵ
//            uint8 window[9];
//            int idx = 0;
//            for (int di = -1; di <= 1; di++) {
//                for (int dj = -1; dj <= 1; dj++) {
//                    window[idx++] = input[i + di][j + dj];
//                }
//            }
//
//            // ʹ�ò�����������ֵ (����ȫ�������)
//            for (int k = 1; k < 9; k++) {
//                uint8 key = window[k];
//                int l = k - 1;
//                while (l >= 0 && window[l] > key) {
//                    window[l + 1] = window[l];
//                    l--;
//                }
//                window[l + 1] = key;
//            }
//
//            output[i][j] = window[4]; // ��ֵ
//        }
//    }
//
//    // ���Ʊ�Ե����
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// �Աȶ�����
// void contrastStretch(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // Ѱ����С�����Ҷ�ֵ
//    uint8 min_val = 255;
//    uint8 max_val = 0;
//
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            if (input[i][j] < min_val) min_val = input[i][j];
//            if (input[i][j] > max_val) max_val = input[i][j];
//        }
//    }
//
//    // ����������
//    if (max_val == min_val) max_val = min_val + 1;
//
//    // Ӧ����������
//    const uint8 range = max_val - min_val;
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            // ʹ�����������Ż�
//            int32_t value = ((int32_t)(input[i][j] - min_val) * 255) / range;
//            output[i][j] = (value > 255) ? 255 : (value < 0) ? 0 : value;
//        }
//    }
//}

// 2. ��򷨺��ĺ��� (���Ż�)
//--------------------------------------------------------------
// int otsuThreshold(uint8 image[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint32 histogram[256] = {0};       // �Ҷ�ֱ��ͼ
//    uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
//    uint64 sum = 0;                    // �ܻҶ�ֵ
//    uint64 sumB = 0;                   // ������Ҷ��ܺ�
//    uint32 wB = 0;                     // ������������
//    uint64 maxVariance = 0;            // �����䷽��
//    int threshold = 0;                 // �����ֵ
//    int declineCount = 0;              // �����½�������
//    const int maxDecline = 2;          // ������������½�����

//    // ����Ҷ�ֱ��ͼ���ܻҶ�ֵ
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            uint8 pixel = image[i][j];
//            histogram[pixel]++;
//            sum += pixel;
//        }
//    }

//    // �������ܵ���ֵ�����õ���������ǰ��ֹ��
//    for (int t = 0; t < 256; t++) {
//        if (histogram[t] == 0) continue;

//        wB += histogram[t];
//        if (wB == 0) continue;

//        uint32 wF = total_pixels - wB;
//        if (wF == 0) break;

//        sumB += (uint64)t * histogram[t];

//        // ���㱳����ǰ����ƽ���Ҷ�
//        uint64 mB = sumB / wB;
//        uint64 mF = (sum - sumB) / wF;

//        // ������䷽��
//        uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

//        // ��ⷽ���½�����
//        if (variance >= maxVariance) {
//            maxVariance = variance;
//            threshold = t;
//            declineCount = 0; // �����½�������
//        } else {
//            declineCount++;
//            if (declineCount > maxDecline) {
//                break;
//            }
//        }
//    }

//    return threshold;
//}

// 3. ������ (��ֵ����)
//--------------------------------------------------------------
// ���ٸ�ʴ���� (3x3�ṹԪ��)
// void erode3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//              uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // ֻ�����ڲ�����
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // �������������ǰ��(255)�����������������
//            if (input[i][j] == 255) {
//                // ���3x3�����Ƿ��б���(0)
//                uint8 isEroded = 0;
//                for (int di = -1; di <= 1 && !isEroded; di++) {
//                    for (int dj = -1; dj <= 1; dj++) {
//                        if (input[i + di][j + dj] == 0) {
//                            isEroded = 1;
//                            break;
//                        }
//                    }
//                }
//                output[i][j] = isEroded ? 0 : 255;
//            } else {
//                output[i][j] = 0; // �������ֲ���
//            }
//        }
//    }
//
//    // �����Ե���� (ֱ�Ӹ���)
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// �������Ͳ��� (3x3�ṹԪ��)
// void dilate3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//               uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // ֻ�����ڲ�����
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // ������������Ǳ���(0)�����������������
//            if (input[i][j] == 0) {
//                // ���3x3�����Ƿ���ǰ��(255)
//                uint8 isDilated = 0;
//                for (int di = -1; di <= 1 && !isDilated; di++) {
//                    for (int dj = -1; dj <= 1; dj++) {
//                        if (input[i + di][j + dj] == 255) {
//                            isDilated = 1;
//                            break;
//                        }
//                    }
//                }
//                output[i][j] = isDilated ? 255 : 0;
//            } else {
//                output[i][j] = 255; // ǰ�����ֲ���
//            }
//        }
//    }
//
//    // �����Ե���� (ֱ�Ӹ���)
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// ������ (�ȸ�ʴ������)
// void morphologicalOpen(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                      uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint8 temp[IMAGE_HEIGHT][IMAGE_WIDTH];
//    erode3x3(input, temp);
//    dilate3x3(temp, output);
//}

// ������ (�����ͺ�ʴ)
// void morphologicalClose(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                       uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint8 temp[IMAGE_HEIGHT][IMAGE_WIDTH];
//    dilate3x3(input, temp);
//    erode3x3(temp, output);
//}

// 4. Ӧ����ֵ����
//--------------------------------------------------------------
// void applyThreshold(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                   uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH],
//                   int threshold)
//{
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            // ʹ��λ�����Ż���ֵ��
//            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
//        }
//    }
//}

// 5. ����������
//--------------------------------------------------------------
// void image_output()
//{
//    // �ڴ��Ż���ʹ�õ�һ��ʱ������
//    static uint8 tempBuffer[IMAGE_HEIGHT][IMAGE_WIDTH];
//
//    // ����1: Ԥ���� (��ֵ�˲�)
//    medianFilter3x3(image_copy, tempBuffer);
//
//    // ����2: ��ǿ�Աȶ� (��ѡ�����ݹ�������)
//    // ���ͼ��Աȶȵͣ����öԱȶ�����
//    #ifdef ENABLE_CONTRAST_STRETCH
//    contrastStretch(tempBuffer, tempBuffer);
//    #endif
//
//    // ����3: ��������ֵ
//    int threshold = otsuThreshold(tempBuffer);
//
//    // ����4: Ӧ����ֵ��ֵ��
//    applyThreshold(tempBuffer, binaryImage, threshold);
//
//    // ����5: ������
//    // ������ȥ��С������
//    morphologicalOpen(binaryImage, tempBuffer);
//
//    // ���������С�׶�
//    morphologicalClose(tempBuffer, binaryImage);
//
//    // ����binaryImage�������մ�����
//    // ��������ʾ���������
//}

#include "zf_common_headfile.h"
#include "Image Binarization.h"

// ʹ������ͷʵ�ʳߴ綨��
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH MT9V03X_W

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // �߽�ͼ��
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // ������ͼ��
int center_points[IMAGE_HEIGHT];                   // �洢ÿ�е����ĵ�����



// ��򷨺��ĺ��� (���Ż�)
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
    for (int t = 0; t < 256; t+=2)//���2�����㣬��������ٶ�
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






//  Ӧ����ֵ����
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
void find_boundaries_and_centerline() {
    // ��ʼ��ͼ��
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            boundary_image[y][x] = 0;
            centerline_image[y][x] = 0;
        }
        center_points[y] = -1; // ��ʼ��Ϊ��Чֵ
    }

    // ����������������
    const int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // ֻ����ͼ���м����򣬱���߽���
    for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
        int left_boundary = -1;
        int right_boundary = -1;

        // Ѱ����߽�
        for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
            if (binaryImage[y][x] == 255) {
                // �����������Ƿ��б�����
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0) {
                        left_boundary = x;
                        boundary_image[y][x] = 255; // ���Ϊ�߽�
                        break;
                    }
                }
                if (left_boundary != -1) break;
            }
        }

        // Ѱ���ұ߽�
        for (int x = IMAGE_WIDTH - 2; x > 0; x--) {
            if (binaryImage[y][x] == 255) {
                // �����������Ƿ��б�����
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0) {
                        right_boundary = x;
                        boundary_image[y][x] = 255; // ���Ϊ�߽�
                        break;
                    }
                }
                if (right_boundary != -1) break;
            }
        }

        // ����������
        if (left_boundary != -1 && right_boundary != -1) {
            int center_x = (left_boundary + right_boundary) / 2;
            center_points[y] = center_x; // �洢���ĵ�
            
            // ��������ͼ���ϱ�����ĵ㣨��2���ط�Χ�ڣ�
            for (int offset = -2; offset <= 2; offset++) {
                int cx = center_x + offset;
                if (cx >= 0 && cx < IMAGE_WIDTH) {
                    centerline_image[y][cx] = 128; // ��ɫ���
                }
            }
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
  find_boundaries_and_centerline();
}
