import numpy as np

# Define file paths
input_file_path = '/mnt/data/least_square.txt'
output_file_path = '/mnt/data/out.txt'

# Indices for relevant columns
SCORE_IDX = 3  # 사용자 점수
RATING_IDX = 2  # 평점
LOCATION_IDX = 4  # 입지 점수
SALES_IDX = 5  # 판매 완료 여부
SLOPE_IDX = 6  # 기울기값
INTERCEPT_IDX = 7  # 기준값

# Data containers
data_slope = []
data_intercept = []

# Read and process the file
with open(input_file_path, 'rb') as raw_file:
    result = chardet.detect(raw_file.read())
    file_encoding = result['encoding']
with open(input_file_path, 'r', encoding=file_encoding) as file:
    for line in file:
        parts = line.strip().split("\t")
        try:
            # Extract relevant values
            x = float(parts[SCORE_IDX])
            y = float(parts[RATING_IDX])
            z = float(parts[LOCATION_IDX])
            w = float(parts[SALES_IDX])
            slope = float(parts[SLOPE_IDX])
            intercept = float(parts[INTERCEPT_IDX])

            # Filter for valid cases
            if x > 0 and y > 0:
                data_slope.append([x, y, z, w, slope])
                data_intercept.append([x, y, z, w, intercept])
        except ValueError:
            # Skip invalid lines
            continue

# Convert to numpy arrays
data_slope = np.array(data_slope)
data_intercept = np.array(data_intercept)

# Function for least squares computation
def compute_coefficients(data):
    X = data[:, :4]  # Independent variables
    V = data[:, 4]   # Dependent variable
    X = np.hstack([X, np.ones((X.shape[0], 1))])  # Add intercept term
    coefficients = np.linalg.lstsq(X, V, rcond=None)[0]
    return coefficients

# Calculate coefficients for both cases
coefficients_slope = compute_coefficients(data_slope)
coefficients_intercept = compute_coefficients(data_intercept)

# Write results to the output file
with open(output_file_path, 'w', encoding='utf-8') as file:
    file.write("\t".join(map(str, coefficients_slope)) + "\n")
    file.write("\t".join(map(str, coefficients_intercept)) + "\n")

output_file_path