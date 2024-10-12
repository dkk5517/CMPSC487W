from datetime import datetime
import sqlite3

# Connect to the SQLite database
conn = sqlite3.connect('rentals.db')
cursor = conn.cursor()
def create_reservation(driver_name, car_type, check_out, return_time):
    # Check if the reservation is at least 24 hours ahead
    check_out_time = datetime.strptime(check_out, "%Y-%m-%d %H:%M")
    if (check_out_time - datetime.now()).total_seconds() < 86400:
        return "Error: Reservation must be made at least 24 hours in advance."
    
    # Check if the selected car type is available
    cursor.execute("SELECT available FROM Cars WHERE car_type = ? AND available = 1", (car_type,))
    if cursor.fetchone() is None:
        return "Error: No available cars of the selected type."
    
    # Insert the reservation into the Reservations table
    cursor.execute('''INSERT INTO Reservations(driver_name, car_type, check_out, return_time)
                      VALUES (?, ?, ?, ?)''', (driver_name, car_type, check_out, return_time))
    conn.commit()
    return "Reservation created successfully!"

def view_reservations():
    # Fetch all reservations
    cursor.execute("SELECT * FROM Reservations")
    reservations = cursor.fetchall()
    return reservations

def request_extension(reservation_id, new_return_time):
    # Check if extension is possible (no conflicting reservations)
    cursor.execute("SELECT car_type, return_time FROM Reservations WHERE reservation_id = ?", (reservation_id,))
    result = cursor.fetchone()
    if not result:
        return "Reservation not found."
    
    car_type, current_return_time = result
    cursor.execute('''SELECT * FROM Reservations 
                      WHERE car_type = ? AND check_out < ? AND return_time > ?''', 
                   (car_type, new_return_time, current_return_time))
    
    if cursor.fetchone():
        return "Error: Extension denied due to another reservation."
    
    # Update reservation with the new return time
    cursor.execute("UPDATE Reservations SET return_time = ?, extension_requested = 1 WHERE reservation_id = ?",
                   (new_return_time, reservation_id))
    conn.commit()
    return "Extension granted successfully!"
