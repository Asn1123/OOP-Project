CXX      = g++
CXXFLAGS = -std=c++17 -DUSE_GUI -Iinclude -Iraylib/include
LIBS     = app_rc.o raylib/lib/libraylib.a -lgdi32 -lwinmm -lole32 -static-libgcc -static-libstdc++ -Wl,-Bstatic,-lwinpthread,-Bdynamic
OUT      = carpool1.exe

SRCS = src/main.cpp \
       src/CarpoolSystem.cpp \
       src/Student.cpp \
       src/Ride.cpp \
       src/Vehicle.cpp \
       src/Car.cpp \
       src/Bike.cpp \
       src/Notification.cpp \
       src/gui.cpp \
       src/win_icon.cpp

all: $(OUT)

$(OUT): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LIBS) -o $(OUT)

clean:
	del /Q $(OUT) 2>nul || true
