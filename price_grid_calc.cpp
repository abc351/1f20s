#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 40
#define MAX_HOTELS 1000
#define MAX_LINE_LENGTH 1024
#define MAX_FILES 20

typedef struct {
    char name[MAX_LINE_LENGTH];
    double x;
    double y;
    double rating;
} Hotel;

typedef struct {
    char name[MAX_LINE_LENGTH];
    int price;
} HotelPrice;

// 호텔 데이터 로드
int load_hotel_positions(const char* filename, Hotel hotels[]) {
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

        for (int i = 0; i < 1; i++) {
            token = strtok(NULL, "\t");
        }
        hotels[count].rating = atof(token);

        token = strtok(NULL, "\t");
        hotels[count].x = atof(token);

        token = strtok(NULL, "\t");
        hotels[count].y = atof(token);

        count++;
    }

    fclose(file);
    return count;
}

// 파일에서 호텔 가격 데이터 로드
int load_prices_from_file(const char* filename, HotelPrice hotel_prices[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "\t");
        strncpy(hotel_prices[count].name, token, MAX_LINE_LENGTH);

        for (int i = 0; i < 2; i++) {
            token = strtok(NULL, "\t");
        }
        // 숫자만 남기고 나머지 제거
        char cleaned_token[100];
        int index = 0;
        for (int j = 0; token[j] != '\0'; j++) {
            if (token[j] >= '0' && token[j] <= '9') {
                cleaned_token[index++] = token[j];
            }
        }
        cleaned_token[index] = '\0'; // Null-terminate the string

        hotel_prices[count].price = atoi(cleaned_token);

        count++;
    }

    fclose(file);
    return count;
}

// 평균 가격 계산
double calculate_average_price(const char* folders[], int folder_count, const char* hotel_name, int suffix) {
    HotelPrice hotel_prices[MAX_HOTELS];
    int total_price = 0;
    int count = 0;

    for (int i = 0; i < folder_count; i++) {
        char filepath[MAX_LINE_LENGTH];
        snprintf(filepath, sizeof(filepath), "%s/%s_15_%d.txt", folders[i], folders[i],suffix);
        int price_count = load_prices_from_file(filepath, hotel_prices);

        for (int j = 0; j < price_count; j++) {
            if (strcmp(hotel_prices[j].name, hotel_name) == 0) {
                total_price += hotel_prices[j].price;
                count++;
                break;
            }
        }
    }

    return count > 0 ? (double)total_price / count : -1000;
}

// 2D 배열 채우기
void fill_grid(double grid[GRID_SIZE][GRID_SIZE], Hotel hotels[], int hotel_count, const char* folders[], int folder_count,int suffix) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (i == 20 && j == 20) 
                printf("!");
            double x_min = -40 + 2 * i;
            double x_max = x_min + 2;
            double y_min = -40 + 2 * j;
            double y_max = y_min + 2;

            double total_price = 0;
            int hotel_in_zone = 0;

            for (int k = 0; k < hotel_count; k++) {
                if (hotels[k].rating == (double)suffix || hotels[k].rating == (double)suffix+0.5) {
                    if (hotels[k].x >= x_min && hotels[k].x < x_max && hotels[k].y >= y_min && hotels[k].y < y_max) {
                        double avg_price = calculate_average_price(folders, folder_count, hotels[k].name,suffix);
                        if (avg_price > 0) {
                            total_price += avg_price;
                            hotel_in_zone++;
                        }
                    }
                }
            }

            grid[i][j] = hotel_in_zone > 0 ? total_price / hotel_in_zone : -1000;
        }
    }
}

// 결과 출력
void print_grid(double grid[GRID_SIZE][GRID_SIZE],const char *filename) {
    FILE* f = fopen(filename, "w");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (j > 0) printf("\t");
            fprintf(f,"%.0f\t", grid[i][j]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}
// 새 grid 생성
void create_combined_grid(double grid2[GRID_SIZE][GRID_SIZE], double grid3[GRID_SIZE][GRID_SIZE], double combined_grid[GRID_SIZE][GRID_SIZE], double ratio23) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid2[i][j] > 0 && grid3[i][j] > 0) {
                combined_grid[i][j] = (grid2[i][j] + grid3[i][j] * ratio23) / 2;
            }
            else if (grid2[i][j] > 0) {
                combined_grid[i][j] = grid2[i][j];
            }
            else if (grid3[i][j] > 0) {
                combined_grid[i][j] = grid3[i][j] * ratio23;
            }
            else {
                combined_grid[i][j] = -1000;
            }
        }
    }
}

int main() {
    const char* hotel_pos_file = "hotel_pos.txt";
    const char* folders[] = { "1026", "1027", "1028", "1029", "1030", "1031", "1101", "1102", "1103", "1104",
                             "1105", "1106", "1107", "1108", "1109", "1110", "1111", "1112", "1113", "1114" };
    int folder_count = sizeof(folders) / sizeof(folders[0]);

    Hotel hotels[MAX_HOTELS];
    int hotel_count = load_hotel_positions(hotel_pos_file, hotels);

    double grid2[GRID_SIZE][GRID_SIZE] = { 0 };
    double grid3[GRID_SIZE][GRID_SIZE] = { 0 };
    double combined_grid[GRID_SIZE][GRID_SIZE] = { 0 };

    fill_grid(grid2, hotels, hotel_count, folders, folder_count, 2);
    fill_grid(grid3, hotels, hotel_count, folders, folder_count, 3);

    // grid2/grid3 비율 계산
    double total_ratio = 0;
    int ratio_count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid2[i][j] > 0 && grid3[i][j] > 0) {
                total_ratio += grid2[i][j] / grid3[i][j];
                ratio_count++;
            }
        }
    }
    double ratio23 = ratio_count > 0 ? total_ratio / ratio_count : 1.0;

    create_combined_grid(grid2, grid3, combined_grid, ratio23);

    print_grid(grid2, "grid2.txt");
    print_grid(grid3, "grid3.txt");
    print_grid(combined_grid, "combined_grid.txt");

    printf("Ratio23: %.2f\n", ratio23);
    return 0;
}
