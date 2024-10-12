import sqlite3

# Connect to the SQLite database
conn = sqlite3.connect('rentals.db')
cursor = conn.cursor()

# Check if the table exists
def check_tables():
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    tables = cursor.fetchall()
    print("Tables in the database:", tables)

# Create Reservations table if it doesn't exist
cursor.execute('''CREATE TABLE IF NOT EXISTS Reservations (
                    reservation_id INTEGER PRIMARY KEY AUTOINCREMENT,
                    driver_name TEXT NOT NULL,
                    car_type TEXT NOT NULL,
                    check_out DATETIME NOT NULL,
                    return_time DATETIME NOT NULL,
                    extension_requested BOOLEAN DEFAULT 0
                 )''')

# Create Cars table if it doesn't exist
cursor.execute('''CREATE TABLE IF NOT EXISTS Cars (
                    car_id INTEGER PRIMARY KEY AUTOINCREMENT,
                    car_type TEXT NOT NULL,
                    available BOOLEAN NOT NULL DEFAULT 1,
                    rate_per_day REAL NOT NULL,
                    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP
                 )''')

conn.commit()

# Check the tables in the database
check_tables()
