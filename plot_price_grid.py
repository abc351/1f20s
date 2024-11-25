import numpy as np
import matplotlib.pyplot as plt

# 파일에서 데이터 읽기
filename = "combined_grid.txt"
grid = []

with open(filename, "r") as file:
    for line in file:
        # 값 읽고 -1000을 0으로 대체
        row = [float(value) if float(value) != -1000 else 0 for value in line.strip().split("\t")]
        grid.append(row)

# NumPy 배열로 변환
grid = np.array(grid)

# 3D 막대 그래프 그리기
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# X, Y 좌표 만들기
x = np.arange(grid.shape[1]) - 20  # -20부터 시작하도록 설정
y = np.arange(grid.shape[0]) - 20
x, y = np.meshgrid(x, y)

# Flatten X, Y, Z 좌표
x = x.flatten()
y = y.flatten()
z = np.zeros_like(x)
dx = dy = 1  # 막대의 두께
dz = grid.flatten()

# 3D 바 그래프
ax.bar3d(x, y, z, dx, dy, dz, shade=True)

# 그래프 설정
ax.set_title("3D Bar Graph of Combined Grid")
ax.set_xlabel("X-axis (blocks)")
ax.set_ylabel("Y-axis (blocks)")
ax.set_zlabel("Value")

plt.show()
