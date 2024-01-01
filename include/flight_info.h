#pragma once

#include <Arduino.h>
#include <list>

// Conversions to metric
#define FT_TO_M 0.3048
#define KTS_TO_KMH 1.852f

// One degree is 110.567 km (69.091 miles)
#define DEGREES_TO_KM 110.567
#define DEGREES_TO_MI 69.091

struct location_info
{
    float latitude_min_;
    float latitude_max_;
    float longitude_min_;
    float longitude_max_;

    location_info();
    location_info(float latitude, float longitude, ushort range, bool imperial);
};

struct flight_info
{
    String icao_address;             //  0 => ICAO 24BIT ADDRESS - 4CA853
    float latitude;                  //  1 => LAT - 52.3247
    float longitude;                 //  2 => LON - 4.9812
    int heading;                     //  3 => TRACK - 263 (Degrees)
    int altitude;                    //  4 => ALTITUDE - 2100  (Feet)
    int ground_speed;                //  5 => SPEED - 144 (Knots)
    String squawk;                   //  6 => SQUAWK -
    String radar;                    //  7 => RADAR -
    String aircraft_code;            //  8 => TYPE - B744 => Boeing 747-4B5(BCF)
    String registration;             //  9 => REGISTRATION - N709CK
    time_t timestamp;                // 10 => TIMSTAMP - 1593976456
    String iata_origin_airport;      // 11 => FROM - AMS
    String iata_destination_airport; // 12 => TO - JFK
    String flight;                   // 13 => FLIGHT NUMBER - LH8160
    bool on_ground;                  // 14 => ON GROUND - 0
    int vertical_speed;              // 15 => VSPEED - 0
    String call_sign;                // 16 => TRA822
    String icao_airline;             // 18 => OPERATOR - RYR

    String toString() const;

    int altitude_metric() const { return altitude * FT_TO_M; }
    int ground_speed_metric() const { return ground_speed * KTS_TO_KMH; }
    int vertical_speed_metric() const { return vertical_speed * KTS_TO_KMH; }

    bool squawk_hijack() const { return squawk == "7500"; }
    bool squawk_radio_failure() const { return squawk == "7600"; }
    bool squawk_emergency() const { return squawk == "7700"; }
};

std::list<flight_info> get_flights(const location_info &location, bool air, bool ground, bool gliders, bool vehicles);