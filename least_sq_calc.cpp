#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HOTELS 1000
#define MAX_LINE_LENGTH 1024
#define GRID_SIZE 40
#define NUM_FOLDERS 20

typedef struct {
    char name[MAX_LINE_LENGTH];
    double rating;
    double x;
    double y;
    double location_score;
} Hotel;

void find_best_fit(double v1, double v2, double v3, double v5, double v7, double v10, double v15, double* a, double* b) {
    double x_values[] = { 1, 2, 3, 5, 7, 10, 15 };
    double y_values[] = { v1, v2, v3, v5, v7, v10, v15 };
    int n = sizeof(x_values) / sizeof(x_values[0]);

    *a = 0.0;
    *b = 0.0;
    double learning_rate = 0.001;
    int max_iterations = 10000;

    for (int iter = 0; iter < max_iterations; iter++) {
        double sum_error_a = 0.0, sum_error_b = 0.0;
        int valid_points = 0;

        for (int i = 0; i < n; i++) {
            if (y_values[i] <= 0) continue; // y가 음수인 점은 무시

            double x = x_values[i];
            double y = y_values[i];
            double y_pred = (*a) * x + (*b);

            double error = y_pred - y;
            sum_error_a += error * x;
            sum_error_b += error;
            valid_points++;
        }

        if (valid_points == 0) break; // 유효한 데이터가 없으면 종료

        *a -= learning_rate * (2.0 / valid_points) * sum_error_a;
        *b -= learning_rate * (2.0 / valid_points) * sum_error_b;
    }
}

// 숫자만 추출
void clean_numeric(char* token, char* cleaned) {
    int index = 0;
    for (int i = 0; token[i] != '\0'; i++) {
        if (isdigit(token[i])||token[i]=='.') {
            cleaned[index++] = token[i];
        }
    }
    cleaned[index] = '\0';
}


// 호텔 데이터 로드
int load_hotels(const char* filename, Hotel hotels[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open hotel position file");
        return 0;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "\t");
        strncpy(hotels[count].name, token, MAX_LINE_LENGTH);

        token = strtok(NULL, "\t");
        hotels[count].rating = atof(token);

        token = strtok(NULL, "\t");
        hotels[count].x = atof(token);

        token = strtok(NULL, "\t");
        hotels[count].y = atof(token);

        hotels[count].location_score = 0; // 초기화
        count++;
    }

    fclose(file);
    return count;
}

// combined_grid 읽기
void load_grid(const char* filename, double grid[GRID_SIZE][GRID_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open grid file");
        return;
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fscanf(file, "%lf", &grid[i][j]);
            if (grid[i][j] == -1000) {
                grid[i][j] = 0; // -1000을 0으로 대체
            }
        }
    }

    fclose(file);
}

// 가격 읽기
int read_price(const char* filename, const char* hotel_name, float* user_score) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "\t");
        if (strcmp(token, hotel_name) == 0) {
            // 사용자 점수 읽기
            token = strtok(NULL, "\t");
            char cleaned[MAX_LINE_LENGTH];
            clean_numeric(token, cleaned);
            *user_score = atof(cleaned);

            // 가격 읽기
            token = strtok(NULL, "\t");
            clean_numeric(token, cleaned);

            fclose(file);
            return atoi(cleaned);
        }
    }

    fclose(file);
    return -1;
}

// 계산 및 저장
void calculate_and_save(Hotel hotels[], int hotel_count, double grid[GRID_SIZE][GRID_SIZE], const char folders[][100]) {
    FILE* output = fopen("least_square.txt", "w");
    if (!output) {
        perror("Failed to open output file");
        return;
    }

    for (int index = 0; index <= 9; index++) {
        for (int i = 0; i < hotel_count; i++) {
            int grid_x = (int)((hotels[i].x + 40) / 2);
            int grid_y = (int)((hotels[i].y + 40) / 2);
            hotels[i].location_score = grid[grid_y][grid_x];

            double x_values[7] = { 1, 2, 3, 5, 7, 10, 15 };
            double y_values[7];
            float user_scores;
            int negative_count = 0;

            for (int j = 0; j < 7; j++) {
                char filename[MAX_LINE_LENGTH];
                int folder_index = index + (j == 0 ? 0 : (j == 1 ? 1 : (j == 2 ? 2 : (j == 3 ? 4 : (j == 4 ? 6 : (j == 5 ? 9 : 14))))));
                snprintf(filename, sizeof(filename), "%s/%s_%d_%.0f.txt", folders[folder_index], folders[folder_index], (int)x_values[j], hotels[i].rating);

                y_values[j] = read_price(filename, hotels[i].name, &user_scores);
                if (y_values[j] == -1) negative_count++;
            }

            if (negative_count >= 3) continue; // y값이 -1인 경우가 3개 이상이면 건너뜀

            double a, b;
            find_best_fit(y_values[0], y_values[1], y_values[2], y_values[3], y_values[4], y_values[5], y_values[6], &a, &b);
            bool sold = (y_values[1] < 0);
            fprintf(output, "%s\t%d\t%.1f\t%.1f\t%.1f\t%d\t%.4f\t%.4f\n",
                hotels[i].name, index, hotels[i].rating, user_scores, hotels[i].location_score, (int)sold, a, b);
        }
    }

    fclose(output);
}

int main() {
    char folders[20][100] = {
        "1026", "1027", "1028", "1029", "1030", "1031", "1101", "1102", "1103", "1104",
        "1105", "1106", "1107", "1108", "1109", "1110", "1111", "1112", "1113", "1114"
    };

    Hotel hotels[MAX_HOTELS];
    int hotel_count = load_hotels("hotel_pos.txt", hotels);

    double grid[GRID_SIZE][GRID_SIZE] = { 0 };
    load_grid("combined_grid.txt", grid);

    calculate_and_save(hotels, hotel_count, grid, folders);

    printf("Calculation completed and saved to least_square.txt\n");
    return 0;
}