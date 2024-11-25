from scipy.optimize import least_squares

# Hotel positions
hotels = {
    "PresidentialHotel": np.array([0, 0]),
    "GrandWalkerhillSeoul": np.array([11.8, 0]),
    "ParasParaSeoul": np.array([0, 11.25])
}

# Given distances between hotels and offices
distances_array = np.array([
    [0.15, 14.9, 14.88, 13.76, 12.37],  # PresidentialHotel
    [11.8, 3.26, 16.59, 25.39, 14.4],  # GrandWalkerhillSeoul
    [11.25, 17.72, 25.74, 19.53, 2.22]  # ParasParaSeoul
])

# Residual function for optimization
def residuals(params):
    x_gangdong, x_gwacheon, y_gwacheon, x_gangseo, y_gangseo, x_dobong, y_dobong = params
    
    # Office positions
    offices = [
        np.array([0, 0]),  # Seoul City Hall
        np.array([x_gangdong, 0]),  # GangdongguOffice
        np.array([x_gwacheon, y_gwacheon]),  # GwacheonCityHall
        np.array([x_gangseo, y_gangseo]),  # GangseoguOffice
        np.array([x_dobong, y_dobong])  # DobongguOffice
    ]
    
    res = []
    for i, hotel_pos in enumerate(hotels.values()):
        for j, office_pos in enumerate(offices):
            calculated_distance = np.linalg.norm(hotel_pos - office_pos)
            res.append(calculated_distance - distances_array[i, j])
    
    return res

# Initial guesses for optimization
initial_guess = [10, 5, -10, -10, -5, 5, -10]

# Solve using least squares optimization
solution = least_squares(residuals, initial_guess)

# Extract optimized positions
x_gangdong = solution.x[0]
x_gwacheon, y_gwacheon = solution.x[1:3]
x_gangseo, y_gangseo = solution.x[3:5]
x_dobong, y_dobong = solution.x[5:7]

# Display refined results
optimized_positions_refined = {
    "GangdongguOffice": (x_gangdong, 0),
    "GwacheonCityHall": (x_gwacheon, y_gwacheon),
    "GangseoguOffice": (x_gangseo, y_gangseo),
    "DobongguOffice": (x_dobong, y_dobong)
}
optimized_positions_refined
