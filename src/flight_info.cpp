#include <flight_info.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

location_info::location_info()
{
    latitude_min_ = 0.0;
    latitude_max_ = 0.0;
    longitude_min_ = 0.0;
    longitude_max_ = 0.0;
}

location_info::location_info(float latitude, float longitude, ushort range, bool imperial)
{
    auto degrees = imperial ? range / DEGREES_TO_MI : range / DEGREES_TO_KM;
    latitude_min_ = latitude - degrees / 2.0;
    latitude_max_ = latitude + degrees / 2.0;
    longitude_min_ = longitude - degrees / 2.0;
    longitude_max_ = longitude + degrees / 2.0;

    log_i("lat_min: %f, lat_max: %f, lon_min: %f, lon_max: %f", latitude_min_, latitude_max_, longitude_min_, longitude_max_);
}

std::list<flight_info> get_flights(const location_info &location, bool air, bool ground, bool gliders, bool vehicles)
{
    const String flight_data_base_url = "http://data-cloud.flightradar24.com/zones/fcgi/feed.js";
    const String bounds = String(location.latitude_max_) + "," + String(location.latitude_min_) + "," + String(location.longitude_min_) + "," + String(location.longitude_max_);
    const String flight_data_url = flight_data_base_url + "?" + "bounds=" + bounds + "&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=" + String(ground) + "&air=" + String(air) + "&vehicles=" + String(vehicles) + "&estimated=1&maxage=14400&gliders=" + String(gliders) + "&stats=0";

    HTTPClient client;
    log_i("Request states=%s", flight_data_url.c_str());
    if (!client.begin(flight_data_url))
    {
        log_w("Failed to start client. DNS/TCP error?");
        return std::list<flight_info>();
    }

    const auto httpResultCode = client.GET();
    if (httpResultCode != HTTP_CODE_OK)
    {
        client.end();
        log_w("HTTP error: %d", httpResultCode);
        return std::list<flight_info>();
    }

    auto response = client.getString();
    log_d("Body=%s", response.c_str());
    // Parse JSON states object 32k
    DynamicJsonDocument doc_flight_data(32 * 1024);
    const auto error = deserializeJson(doc_flight_data, response);
    if (error != DeserializationError::Ok)
    {
        client.end();
        log_e("Deserialize. Error=%s", error.c_str());
        return std::list<flight_info>();
    }

    client.end();

    std::list<flight_info> flights;
    auto flight_data_root = doc_flight_data.as<JsonObject>();
    for (const auto &kvp : flight_data_root)
    {
        if (!kvp.value().is<JsonArray>())
            continue;

        log_i("KVP=%s", kvp.key().c_str());
        auto items = kvp.value().as<JsonArray>();
        struct flight_info flight
        {
            .icao_address = items[0].as<const char *>(),
            .latitude = items[1].as<const float>(),
            .longitude = items[2].as<const float>(),
            .heading = items[3].as<const int>(),
            .altitude = items[4].as<const int>(),
            .ground_speed = items[5].as<const int>(),
            .squawk = items[6].as<const char *>(),
            .radar = items[7].as<const char *>(),
            .aircraft_code = items[8].as<const char *>(),
            .registration = items[9].as<const char *>(),
            .timestamp = items[10].as<time_t>(),
            .iata_origin_airport = items[11].as<const char *>(),
            .iata_destination_airport = items[12].as<const char *>(),
            .flight = items[13].as<const char *>(),
            .on_ground = items[14].as<const bool>(),
            .vertical_speed = items[15].as<const int>(),
            .call_sign = items[16].as<const char *>(),
            .icao_airline = items[18].as<const char *>()
        };
        flights.push_back(flight);
    }

    return flights;
}

String flight_info::toString() const
{
    return "ICAO " + icao_address + ": Flight " + flight + " from " + iata_origin_airport + " to " + iata_destination_airport + ", Squawk: " + String(squawk) + ", Radar: " + radar + ", Registration: " + registration + ", Lat: " + String(latitude) + ", Lon: " + String(longitude) + ", Altitude: " + String(altitude) + " ft, Ground speed: " + ground_speed + " kts, Heading: " + String(heading) + " degrees, Vertical speed: " + String(vertical_speed) + ", Aircraft code: " + aircraft_code + ", Airline: " + icao_airline;
}