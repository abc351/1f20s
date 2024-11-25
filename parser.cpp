#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
std::string parse(const std::string& in) {
    std::string result;
    std::istringstream iss(in);
    std::string line, prev_line;
    bool found_first_hotel = false;
    std::string hotel_name, stars, rating, total_price, distance;
    bool hotel_in_progress = false;

    while (std::getline(iss, line)) {
        // Remove any carriage return characters
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!found_first_hotel) {
            if (line.find("5점 만점 중") != std::string::npos) {
                found_first_hotel = true;
                hotel_in_progress = true;
                size_t pos = line.find("5점 만점 중");
                std::string rest = line.substr(pos + std::string("5점 만점 중").length());
                size_t end_pos = rest.find("점");
                if (end_pos != std::string::npos) {
                    stars = rest.substr(0, end_pos);
                    // Trim whitespace
                    stars.erase(0, stars.find_first_not_of(" \t"));
                    stars.erase(stars.find_last_not_of(" \t") + 1);
                }
                hotel_name = prev_line;
                rating.clear();
                total_price.clear();
                distance.clear();
            }
            else {
                prev_line = line;
                continue;
            }
        }
        else {
            if (line.find("5점 만점 중") != std::string::npos) {
                // Output the previous hotel's data if available
                if (!hotel_name.empty() && !total_price.empty()) {
                    if (rating.empty()) rating = "0.0";
                    if (stars.empty()) stars = "0";
                    if (distance.empty()) distance = "N/A";
                    result += hotel_name + "\t" + rating + "\t" + total_price + "\t" + stars + "\t" + distance + "\n";
                }
                size_t pos = line.find("5점 만점 중");
                std::string rest = line.substr(pos + std::string("5점 만점 중").length());
                size_t end_pos = rest.find("점");
                if (end_pos != std::string::npos) {
                    stars = rest.substr(0, end_pos);
                    // Trim whitespace
                    stars.erase(0, stars.find_first_not_of(" \t"));
                    stars.erase(stars.find_last_not_of(" \t") + 1);
                }
                // Start processing a new hotel
                hotel_name = prev_line;
                rating.clear();
                total_price.clear();
                distance.clear();
                hotel_in_progress = true;
            }
            else {
                // Extract the 10-point rating
                if (rating.empty() && line.find("10점 만점 중") != std::string::npos) {
                    size_t pos = line.find("10점 만점 중");
                    std::string rest = line.substr(pos + std::string("10점 만점 중").length());
                    size_t end_pos = rest.find("점");
                    if (end_pos != std::string::npos) {
                        rating = rest.substr(0, end_pos);
                        // Trim whitespace
                        rating.erase(0, rating.find_first_not_of(" \t"));
                        rating.erase(rating.find_last_not_of(" \t") + 1);
                    }
                }
                // Extract the total price
                if (total_price.empty() && line.find("총 요금:") != std::string::npos) {
                    size_t pos = line.find("총 요금:");
                    std::string rest = line.substr(pos + std::string("총 요금:").length());
                    rest.erase(0, rest.find_first_not_of(" \t"));
                    total_price = rest;
                }
                if (total_price.empty() && (line.find("저희 사이트에서 매진되었어요!") != std::string::npos|| line.find("예약 불가") != std::string::npos)) {
                    
                    total_price = -1;
                }
                
                // Extract the distance
                if (distance.empty() && (line.find("시내에서") != std::string::npos|| line.find("구에서") != std::string::npos)) {
                    size_t pos = line.find("에서");
                    std::string rest = line.substr(pos + std::string("에서").length());
                    size_t km_pos = rest.find("km");
                    if (km_pos != std::string::npos) {
                        distance = rest.substr(0, km_pos);
                        distance.erase(0, distance.find_first_not_of(" \t"));
                        distance.erase(distance.find_last_not_of(" \t") + 1);
                    }
                }
                // If both rating and total price are found, output the data
                if (!total_price.empty()) {
                    if (rating.empty()) rating = "0.0";
                    if (stars.empty()) stars = "0";
                    if (distance.empty()) distance = "N/A";
                    result += hotel_name + "\t" + rating + "\t" + total_price + "\t" + stars + "\t" + distance + "\n";
                    hotel_in_progress = false;
                    rating.clear();
                    total_price.clear();
                    hotel_name.clear();
                    distance.clear();
                }
            }
            prev_line = line;
        }
    }
    // Output the last hotel's data if available
    if (found_first_hotel && hotel_in_progress && !hotel_name.empty() && !total_price.empty()) {
        if (rating.empty()) rating = "0.0";
        if (stars.empty()) stars = "0";
        if (distance.empty()) distance = "-1";
        result += hotel_name + "\t" + rating + "\t" + total_price + "\t" + stars + "\t" + distance + "\n";
    }
    return result;
}
std::string parse2(const std::string& in) {
    std::string result;
    std::istringstream iss(in);
    std::string line, prev_line;
    bool found_first_hotel = false;
    std::string hotel_name, stars,rating, total_price;
    bool hotel_in_progress = false;

    while (std::getline(iss, line)) {
        // Remove any carriage return characters
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!found_first_hotel) {
            if (line.find("5점 만점 중") != std::string::npos) {
                found_first_hotel = true;
                hotel_in_progress = true;
                size_t pos = line.find("5점 만점 중");
                std::string rest = line.substr(pos + std::string("5점 만점 중").length());
                size_t end_pos = rest.find("점");
                if (end_pos != std::string::npos) {
                    stars = rest.substr(0, end_pos);
                    // Trim whitespace
                    stars.erase(0, stars.find_first_not_of(" \t"));
                    stars.erase(stars.find_last_not_of(" \t") + 1);
                }
                hotel_name = prev_line;
                rating.clear();
                total_price.clear();
            }
            else {
                prev_line = line;
                continue;
            }
        }
        else {
            if (line.find("5점 만점 중") != std::string::npos) {
                // Output the previous hotel's data if available
                if (!hotel_name.empty() && !total_price.empty()) {
                    if (rating.empty()) rating = "0.0";
                    if (stars.empty()) stars = "0";
                    result += hotel_name + "\t" + rating + "\t" + total_price + "\t"+stars+"\n";
                }
                size_t pos = line.find("5점 만점 중");
                std::string rest = line.substr(pos + std::string("5점 만점 중").length());
                size_t end_pos = rest.find("점");
                if (end_pos != std::string::npos) {
                    stars = rest.substr(0, end_pos);
                    // Trim whitespace
                    stars.erase(0, stars.find_first_not_of(" \t"));
                    stars.erase(stars.find_last_not_of(" \t") + 1);
                }
                // Start processing a new hotel
                hotel_name = prev_line;
                rating.clear();
                total_price.clear();
                hotel_in_progress = true;
            }
            else {
                // Extract the 10-point rating
                if (rating.empty() && line.find("10점 만점 중") != std::string::npos) {
                    size_t pos = line.find("10점 만점 중");
                    std::string rest = line.substr(pos + std::string("10점 만점 중").length());
                    size_t end_pos = rest.find("점");
                    if (end_pos != std::string::npos) {
                        rating = rest.substr(0, end_pos);
                        // Trim whitespace
                        rating.erase(0, rating.find_first_not_of(" \t"));
                        rating.erase(rating.find_last_not_of(" \t") + 1);
                    }
                }
                // Extract the total price
                if (total_price.empty() && line.find("총 요금:") != std::string::npos) {
                    size_t pos = line.find("총 요금:");
                    std::string rest = line.substr(pos + std::string("총 요금:").length());
                    rest.erase(0, rest.find_first_not_of(" \t"));
                    total_price = rest;
                }
                // If both rating and total price are found, output the data
                if ( !total_price.empty()) {
                    if (rating.empty()) rating = "0.0";
                    if (stars.empty()) stars = "0";
                    result += hotel_name + "\t" + rating + "\t" + total_price + "\t" + stars + "\n";
                    hotel_in_progress = false;
                    rating.clear();
                    total_price.clear();
                    hotel_name.clear();
                }
            }
            prev_line = line;
        }
    }
    // Output the last hotel's data if available
    
    if (found_first_hotel && hotel_in_progress && !hotel_name.empty() && !total_price.empty()) {
        if (rating.empty()) rating = "0.0";
        if (stars.empty()) stars = "0";
        result += hotel_name + "\t" + rating + "\t" + total_price + "\t" + stars + "\n";
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "사용법: 프로그램명 출력파일명" << std::endl;
        return 1;
    }

    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        std::cerr << "클립보드를 열 수 없습니다." << std::endl;
        return 1;
    }

    // Get clipboard data in Unicode text format
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL) {
        std::cerr << "클립보드에 텍스트 데이터가 없습니다." << std::endl;
        CloseClipboard();
        return 1;
    }

    // Lock the global memory handle to get a pointer to the text
    wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
    if (pszText == NULL) {
        std::cerr << "글로벌 메모리를 잠글 수 없습니다." << std::endl;
        CloseClipboard();
        return 1;
    }

    // Convert the wide string to a narrow string
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszText, -1, NULL, 0, NULL, NULL);
    char* temp = new char[bufferSize * 2];
    WideCharToMultiByte(CP_ACP, 0, pszText, -1, temp, bufferSize, NULL, NULL);
    std::string clipboardText(temp); // -1 to exclude null terminator
    // Unlock the memory and close the clipboard
    GlobalUnlock(hData);
    CloseClipboard();

    // Process the clipboard text
    std::string parsedData = parse(clipboardText);

    // Write the result to a file
    std::ofstream outFile(argv[1]);
    if (!outFile) {
        std::cerr << "출력 파일을 열 수 없습니다." << std::endl;
        return 1;
    }
    outFile << parsedData;
    outFile.close();

    return 0;
}
