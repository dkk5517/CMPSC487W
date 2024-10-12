import tkinter as tk
from tkinter import messagebox
from functions import *
# Function for submitting a reservation
def submit_reservation():
    driver_name = entry_driver_name.get()
    car_type = entry_car_type.get()
    check_out = entry_check_out.get()
    return_time = entry_return_time.get()
    
    if driver_name and car_type and check_out and return_time:
        result = create_reservation(driver_name, car_type, check_out, return_time)
        messagebox.showinfo("Reservation Status", result)
    else:
        messagebox.showwarning("Input Error", "Please fill all fields")

# Function to view existing reservations
def display_reservations():
    reservations = view_reservations()
    display_text = ""
    for res in reservations:
        display_text += f"ID: {res[0]}, Driver: {res[1]}, Car: {res[2]}, Check-Out: {res[3]}, Return: {res[4]}\n"
    messagebox.showinfo("Reservations", display_text)

# GUI Setup
root = tk.Tk()
root.title("Car Rental System")

# Labels and Entry Widgets for Reservation
label_driver_name = tk.Label(root, text="Driver Name")
label_driver_name.pack()
entry_driver_name = tk.Entry(root)
entry_driver_name.pack()

label_car_type = tk.Label(root, text="Car Type (Sedan, SUV, Pick-Up, Van)")
label_car_type.pack()
entry_car_type = tk.Entry(root)
entry_car_type.pack()

label_check_out = tk.Label(root, text="Check-Out (YYYY-MM-DD HH:MM)")
label_check_out.pack()
entry_check_out = tk.Entry(root)
entry_check_out.pack()

label_return_time = tk.Label(root, text="Return Time (YYYY-MM-DD HH:MM)")
label_return_time.pack()
entry_return_time = tk.Entry(root)
entry_return_time.pack()

# Submit Reservation Button
btn_submit = tk.Button(root, text="Create Reservation", command=submit_reservation)
btn_submit.pack()

# View Reservations Button
btn_view_reservations = tk.Button(root, text="View Reservations", command=display_reservations)
btn_view_reservations.pack()

root.mainloop()
