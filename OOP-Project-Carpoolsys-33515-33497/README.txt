IBA CARPOOL FACILITATION SYSTEM
================================

A C++ desktop application that lets IBA Karachi students coordinate
shared rides between their neighbourhoods and the IBA City and Main
campuses. Drivers post rides; passengers search and book them.


AUTHORS
-------
  Ayaan Memon   -- 33515
  Vikash Kumar  -- 33497

Course: Object Oriented Programming (OOP)


FEATURES
--------
  Drivers
    - Register an account with a Car or Bike
    - Post a ride (date, time, pickup area, drop-off area, gender pref)
    - View current active ride and passenger list
    - Cancel a ride (sends notifications to all booked passengers)
    - Browse completed / canceled ride history
    - Edit profile and vehicle details
    - Delete account

  Passengers
    - Register an account
    - Search rides by area pair and desired time (+/- 30 minutes)
    - Book a seat (respects gender preference and seat count)
    - Receive notifications (booked, canceled, completed)
    - Edit or delete account

  System
    - Rides auto-complete once their date/time passes
    - Apriori frequent-traveller analysis (pairs and triples)
    - IBA campus constraint: every ride must include City or Main campus
    - All data persisted to CSV files between sessions


REQUIREMENTS
------------
  - Windows 10/11 (64-bit)
  - MinGW-w64 g++ with C++17 support
  - GNU Make (mingw32-make or make)
  - Raylib static library (included under raylib/)


PROJECT STRUCTURE
-----------------
  include/
    enums.h           -- enums, Area/Gender/Status types, time helpers
    Student.h         -- Student class
    Ride.h            -- Ride class
    Vehicle.h         -- abstract Vehicle base class
    Car.h / Bike.h    -- concrete vehicle subclasses
    CarpoolSystem.h   -- system controller
    Notification.h    -- in-session notification wrapper
    gui.h             -- GUI entry point declaration

  src/
    main.cpp          -- program entry point; loads CSVs; launches GUI
    CarpoolSystem.cpp -- all business logic
    Student.cpp
    Ride.cpp
    Vehicle.cpp
    Car.cpp / Bike.cpp
    Notification.cpp
    gui.cpp           -- full Raylib GUI (~1250 lines)
    win_icon.cpp      -- sets exe and taskbar icon

  data/
    students.csv
    cars.csv
    bikes.csv
    rides.csv

  raylib/             -- Raylib headers and static library
  app_rc.o            -- pre-compiled icon resource
  carpool_icon.ico    -- application icon
  Institute_of_Business_Administration,_Karachi_(logo).png  -- IBA logo
  Makefile


BUILD
-----
  Open a terminal in the project root and run:

    make

  This produces carpool.exe in the project root.

  To delete the executable:

    make clean


RUN
---
  Ensure the following are in the same directory as carpool.exe:

    data/               (folder with CSV files)
    Institute_of_Business_Administration,_Karachi_(logo).png

  Then run:

    carpool.exe

  On first run the data/ CSVs may be empty; register a driver and
  a passenger to get started.


CSV FORMAT (for manual inspection)
-----------------------------------
  students.csv   id, name, phone, gender(0=M/1=F), isDriver(0/1), ridesCompleted
  cars.csv       id, ownerId, make, model, plate, capacity
  bikes.csv      id, ownerId, make, model, plate, capacity
  rides.csv      id, driverId, vehicleId, date(YYYY-MM-DD), time(HH:MM),
                 pickupArea, dropOffArea, genderPref, status, seatsLeft
                 [, pass1;pass2;...]


NOTES
-----
  - Phone numbers are digits only, maximum 11 characters.
  - Car seat capacity is 1 to 6; bikes are fixed at 1.
  - Ride time must be in the future at the time of creation.
  - Pickup and drop-off cannot be the same area.
  - At least one of pickup / drop-off must be an IBA campus
    (IBA City Campus or IBA Main Campus).
