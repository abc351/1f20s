#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_HOTELS 1000
#define MAX_LINE_LENGTH 256
#define NAN_TEXT "nan"

// 구조체 정의
typedef struct {
    char name[MAX_LINE_LENGTH];
    double rating;
    double distance;
} Hotel;

int triangulate(double x1, double y1, double d1, double x2, double y2, double d2, double d3, double* x, double* y) {
    // Constants to handle floating-point precision issues. 
    const double epsilon = 1e-10;
    // Calculate the constants C1 and C2 based on the distance equations.
    double C1 = (d1 * d1 - d3 * d3) - (x1 * x1 + y1 * y1);
    double C2 = (d2 * d2 - d3 * d3) - (x2 * x2 + y2 * y2);

    // Compute D1 and D2 by dividing the constants by -2.
    double D1 = -C1 / 2.0;
    double D2 = -C2 / 2.0;

    // Calculate the determinant of the matrix formed by the known points.
    double det = x1 * y2 - x2 * y1;

    // Check if the determinant is too close to zero (singular matrix).
    if (fabs(det) < epsilon) {
        // Return -1 to indicate an error due to singularity.
        return -1;
    }

    // Apply Cramer's Rule to solve for x.
    double x_result = (D1 * y2 - D2 * y1) / det;

    // Apply Cramer's Rule to solve for y.
    double y_result = (x1 * D2 - x2 * D1) / det;

    // Assign the calculated coordinates to the output pointers.
    *x = x_result;
    *y = y_result;

    // Return 0 to indicate successful computation.
    return 0;
}

// 파일 데이터를 로드하는 함수
int load_data(const char* folder, const char* prefix, Hotel hotels[]) {
    char filepath[MAX_LINE_LENGTH];
    FILE* file;
    int count = 0;

    // 세 가지 파일 유형
    const char* suffixes[] = { "_2.txt", "_3.txt", "_145.txt" };
    for (int i = 0; i < 3; i++) {
        snprintf(filepath, sizeof(filepath), "%s\\%s%s", folder, prefix, suffixes[i]);
        file = fopen(filepath, "r");
        if (!file) {
            continue;
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            char* token = strtok(line, "\t");
            strncpy(hotels[count].name, token, MAX_LINE_LENGTH);

            for (int j = 0; j < 3; j++) {
                token = strtok(NULL, "\t");
            }
            hotels[count].rating = atof(token);

            token = strtok(NULL, "\t");
            hotels[count].distance = atof(token);

            count++;
        }
        fclose(file);
    }
    return count;
}

// 결과 출력 함수
void write_results(const char* filename, Hotel seoul_hotels[], int seoul_count,
    Hotel data[][MAX_HOTELS], int counts[], const double coords[][2]) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open output file");
        return;
    }

    for (int i = 0; i < seoul_count; i++) {
        double x = 0, y = 0;
        int found = 0;

        for (int j = 1; j < 5; j++) { // 네 개의 다른 지역 데이터
            for (int k = 0; k < counts[j]; k++) {
                if (strcmp(seoul_hotels[i].name, data[j][k].name) == 0) {
                    for (int m = j + 1; m < 5; m++) {
                        for (int n = 0; n < counts[m]; n++) {
                            if (strcmp(seoul_hotels[i].name, data[m][n].name) == 0) {
                                // 두 개의 추가 거리값과 좌표를 찾았음
                                double d1 = seoul_hotels[i].distance;
                                double d2 = data[j][k].distance;
                                double d3 = data[m][n].distance;

                                if (triangulate(coords[j][0], coords[j][1], d2,
                                    coords[m][0], coords[m][1], d3,
                                    d1, &x, &y) == 0) {
                                    found = 1;
                                }
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
                if (found) break;
            }
            if (found) break;
        }

        if (found) {
            fprintf(file, "%s\t%.1f\t%.2f\t%.2f\n", seoul_hotels[i].name, seoul_hotels[i].rating, x, y);
        }
        else {
            //fprintf(file, "%s\t%.1f\t%s\t%s\n", seoul_hotels[i].name, seoul_hotels[i].rating, NAN_TEXT, NAN_TEXT);
        }
    }

    fclose(file);
}

int main() {
    const char* folder = "dist";
    const double coords[5][2] = {
        {0, 0},         // Seoul City Hall
        {13.73, -5.17},     // GangdongguOffice
        {-1.77, -14.68}, // GwacheonCityHall
        {-13.60, 1.99}, // GangseoguOffice
        {6.51,9.99}   // DobongguOffice
    };

    Hotel hotels[MAX_HOTELS];
    int counts[5] = { 0 };
    Hotel data[5][MAX_HOTELS];

    // Load data for each prefix
    const char* prefixes[] = { "seoul", "gdong", "gwach", "gseo", "dbong" };
    for (int i = 0; i < 5; i++) {
        counts[i] = load_data(folder, prefixes[i], data[i]);
    }

    // Calculate and write results for Seoul City Hall
    write_results("hotel_pos.txt", data[0], counts[0], data, counts, coords);

    printf("Results written to output.txt\n");
    return 0;
}
