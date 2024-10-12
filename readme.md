# Car Rental System for Mr. Johnson

This project is a Python-based car rental system for managing reservations of cars like sedans, SUVs, pick-ups, and vans. The system allows drivers to make reservations, request extensions, and provides a view for Mr. Johnson to manage the reservations.

## Table of Contents

- [Project Files](#project-files)
- [Requirements](#requirements)
- [Setup Instructions](#setup-instructions)
- [How to Use](#how-to-use)
- [Screenshots](#screenshots)
- [Additional Notes](#additional-notes)

## Project Files

- `db_setup.py`: Contains code to initialize the database and create tables (`Cars` and `Reservations`).
- `functions.py`: Contains functions to manage reservations (create, view, request extensions).
- `interface.py`: A Tkinter-based GUI that allows users to interact with the car rental system.

## Requirements

- **Python 3.x**
- **SQLite3** (comes pre-installed with Python)
- **Tkinter** (comes pre-installed with Python)

## Setup Instructions

1. **Clone the Repository**:
   First, open your terminal and clone the repository using the following command:

   ```bash
   git clone https://github.com/dkk5517/CMPSC487W.git
   cd CMPSC487W
Create the Database: Before running the application, you need to set up the database. Run the db_setup.py script to create the necessary tables:

    
    python db_setup.py
This script will create the Cars and Reservations tables in a SQLite database.

Run the Application: Start the application by running the interface.py file:

     
     python interface.py
This will open the graphical user interface for the car rental system.

### How to Use
For Drivers
Make a Reservation:

Fill out the reservation form in the GUI, selecting the car type (sedan, SUV, pick-up, or van) and specifying the check-out time.
Click the "Reserve" button to submit your reservation.
Request an Extension:

If you need to extend your reservation, provide the necessary details and click the "Request Extension" button.
For Mr. Johnson
View Reservations:
The GUI provides a view of all current reservations. Click the button to load the list of reservations and see details about each one.

Additional Notes
Ensure that your Python environment is set up correctly with the required libraries.
If you encounter any issues, check the console for error messages, which can help diagnose problems.
