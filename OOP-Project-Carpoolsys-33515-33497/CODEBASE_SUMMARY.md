# IBA Carpool System — Codebase Summary

## Overview

A C++17 OOP carpool management system for IBA students, with a 1920×1080 Raylib GUI and CSV-based persistence. Rides must include at least one IBA campus (City or Main). Two roles: **Driver** (creates/manages rides) and **Passenger** (searches/books rides).

---

## File Structure

```
include/
  enums.h          — all enums, inline helpers, time/date utils
  Student.h        — Student class
  Ride.h           — Ride class
  Vehicle.h        — abstract Vehicle base class (pure virtual getType)
  Car.h / Bike.h   — concrete Vehicle subclasses
  CarpoolSystem.h  — system controller (maps, IDs, business logic)
  Notification.h   — simple notification wrapper
  gui.h            — runGUI() entry point

src/
  Student.cpp
  Ride.cpp
  Vehicle.cpp
  Car.cpp / Bike.cpp
  CarpoolSystem.cpp
  Notification.cpp
  gui.cpp          — entire Raylib GUI (~1280 lines)
  main.cpp         — loads CSVs, launches GUI
  win_icon.cpp     — sets exe/taskbar icon from embedded RC resource

data/
  students.csv / cars.csv / bikes.csv / rides.csv
```

---

## Core Classes

### `Student`
Fields: `studentId`, `name`, `phoneNumber`, `gender`, `isDriver`, `ridesCompleted`

Key OOP features:
- Explicit copy constructor
- `operator==` / `operator!=` (by student ID)
- `friend operator<<`

IDs: `STU-1`, `STU-2`, …

---

### `Ride`
Fields: `rideId`, `driverId`, `vehicleId`, `rideDate` (YYYY-MM-DD), `rideTime` (HH:MM), `pickupArea`, `dropOffArea`, `genderPref`, `status`, `seatsLeft`, `passengerIds`

Key OOP features:
- `operator<` — chronological ordering (date then time), used by `std::sort`
- `operator==` (by ride ID)
- `friend operator<<`
- `addPassenger` — checks duplicates, decrements seats, auto-sets FULL
- `removePassenger` — reopens FULL → OPEN
- `rawAddPassenger` — file-load only, no side effects

IDs: `RID-1`, `RID-2`, …

---

### `Vehicle` (abstract)
Pure virtual `getType() = 0`. Subclasses: `Car` (returns `"Car"`), `Bike` (returns `"Bike"`).
Stored as `Vehicle*` in `vehicleMap` (polymorphism).

`friend operator<<` uses virtual dispatch for `getType()`.

IDs: `VEH-1`, `VEH-2`, …

---

### `CarpoolSystem`
The central controller. Holds:
- `std::map<string, Student>   studentMap`
- `std::map<string, Vehicle*>  vehicleMap`  (heap-allocated, deleted in destructor)
- `std::map<string, Ride>      rideMap`
- `std::map<string, vector<Notification>> notifMap`
- Auto-incrementing counters: `nextStudentId`, `nextVehicleId`, `nextRideId`

**Key methods:**

| Method | Notes |
|--------|-------|
| `createStudent` / `addCar` / `addBike` | Creates, persists to CSV |
| `updateStudent` / `updateVehicle` | Edit profile/vehicle |
| `deleteStudent` | Cascades: cancels active rides, notifies passengers, removes vehicle |
| `createRide` | Validates campus constraint, future time, no active ride |
| `bookRide` | Enforces gender pref, driver≠passenger, no duplicates |
| `cancelRide` | Only if future & active; notifies all passengers |
| `checkAutoComplete` | Auto-completes past open/full rides; increments ride counts |
| `searchRides` | Filters by area + ±30 min window; sorted by `std::sort` + lambda |
| `runApriori` | Frequent traveller analysis (singles → pairs → triples) |
| `generateItemsets` | Recursive backtracking k-itemset counter (replaces hardcoded loops) |

---

## GUI (`src/gui.cpp`)

Built with **Raylib** at 1920×1080. Screen-state machine using `enum class Screen` (11 states). All rendering is inside `CarpoolGUI` (private class), exposed via `runGUI(CarpoolSystem&)`.

### Color Palette

| Constant | Value | Usage |
|----------|-------|-------|
| `C_BG` | dark maroon | window background |
| `C_PANEL` | white | cards/panels |
| `C_PANEL2` | light grey | hover, sub-panels |
| `C_ACCENT` | IBA maroon | primary buttons, headings |
| `C_HEADER` | very dark maroon | top header bar |
| `C_TEXT` | near-white | text on dark bg |
| `C_DARK` | near-black | text on white panels |
| `C_DLABEL` | grey | labels on white panels |
| `C_MUTED` | muted pink | secondary text on dark bg |

### Widget Helpers (private, mostly static)

| Helper | Purpose |
|--------|---------|
| `btn(r, label, bg, hover, fs, textCol)` | Generic clickable button |
| `btnP(r, label)` | Maroon primary button (white text) |
| `btnD(r, label)` | Red danger button (white text) |
| `panel(r, bg)` | Bordered rectangle panel |
| `header(title)` | Header bar + IBA logo (top-right) + centered title |
| `textInput(inp, r, placeholder)` | Text field, all printable chars |
| `phoneTextInput(inp, r, placeholder)` | Digits only, max 11 chars |
| `areaGrid(x, y, colW, selected, label)` | 3-col area picker; IBA campuses in green |
| `prefSelector(x, y, w, sel)` | 3-tab gender preference toggle |
| `toggle2(x, y, w, sel, lbl0, lbl1)` | 2-option toggle (Male/Female, Car/Bike) |
| `drawGlobalExit()` | Red X always on top; sets `shouldExit` |

### Screens

| Screen | Description |
|--------|-------------|
| `ROLE_SELECT` | Driver/Passenger cards with Login + Register buttons each |
| `LOGIN` | Student ID entry |
| `REGISTER_FORM` | Name, phone, gender; + vehicle fields for drivers |
| `DRIVER_MENU` | 3 cards: Create/Current Ride, History, Edit Profile |
| `CREATE_RIDE` | Area grid × 2, time, date offset (today/tomorrow/+2), gender pref |
| `CURRENT_RIDE` | Active ride details + passenger list + Cancel button |
| `RIDE_HISTORY` | Scrollable list of completed/canceled rides |
| `PASSENGER_MENU` | Search Rides card + Edit Profile card + Notifications panel |
| `SEARCH_RIDE` | Area grids + desired time input |
| `SEARCH_RESULTS` | Scrollable ride cards with Book button; shows vehicle Make/Model |
| `EDIT_PROFILE` | Edit name/phone/gender; drivers also edit vehicle; red `!` delete button |

### State Fields (private members)

- `cr_*` — create-ride form state
- `sr_*` — search form state
- `reg_*` — registration form state
- `ep_*` — edit-profile form state
- `scr_history/notifs/results` — scroll offsets

---

## Persistence (CSV)

All saves happen immediately after any mutation. Formats:

- **students.csv**: `id,name,phone,gender(0/1),isDriver(0/1),ridesCompleted`
- **cars.csv / bikes.csv**: `id,ownerId,make,model,plate,capacity`
- **rides.csv**: `id,driverId,vehicleId,date,time,pickup,dropoff,genderPref,status,seatsLeft[,pass1;pass2]`

Loader handles old format (7-field students with homeArea; rides without date field).

---

## OOP & Algorithm Highlights (PDF compliance)

| Requirement | Implementation |
|-------------|---------------|
| Inheritance | `Vehicle → Car / Bike` (abstract base, pure virtual `getType`) |
| Polymorphism | `vehicleMap` stores `Vehicle*`; virtual dispatch in `operator<<` and display |
| Copy constructor | `Student::Student(const Student&)` |
| Operator overloading | `==`, `!=` on Student; `<`, `==`, `<<` on Ride; `<<` on Vehicle |
| Recursion | `generateItemsets` — backtracking k-itemset generator for Apriori |
| Sorting | `searchRides` uses `std::sort` + lambda + `Ride::operator<` |
| STL containers | `std::map`, `std::vector`, `std::sort`, `std::find`, `std::pair` |
| Exception handling | `std::runtime_error` thrown/caught in registration and edit-profile forms |

---

## Build

```powershell
# Compile
g++ -std=c++17 -DUSE_GUI -Iinclude -Iraylib/include `
    src/main.cpp src/CarpoolSystem.cpp src/Student.cpp src/Ride.cpp `
    src/Vehicle.cpp src/Car.cpp src/Bike.cpp src/Notification.cpp `
    src/gui.cpp src/win_icon.cpp `
    app_rc.o raylib/lib/libraylib.a `
    -lgdi32 -lwinmm -lole32 -o carpool.exe
```

- `app_rc.o` — pre-compiled resource embedding `carpool_icon.ico` (exe + taskbar icon)
- Logo file `Institute_of_Business_Administration,_Karachi_(logo).png` must be in working directory
- Data files must be in `data/` subdirectory
