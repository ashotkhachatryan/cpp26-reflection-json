#include <meta>
#include <iostream>
#include <type_traits>
#include <string_view>

#include "json.hpp"
using json = nlohmann::json;

struct Name {
    std::string first;
    std::string last;
};

struct Contact {
    std::string email;
    std::string phone;
};

struct Coordinates {
    double latitude;
    double longitude;
};

struct Address {
    std::string country;
    std::string city;
    std::string postcode;
    Coordinates coordinates;
};

struct Preferences {
    bool newsletter;
    bool darkMode;
    std::string language;
};

struct Student {
    int id;
    bool active;
    int age;
    double gpa;
    Name name;
    Contact contact;
    Address address;
    Preferences preferences;
};

template <typename T>
constexpr T DeserializeObject(const std::string& s) {
    json j = json::parse(s);
    T obj;
    template for (constexpr auto e : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
        using Type = typename[:type_of(e):];
        
        if constexpr (std::is_fundamental_v<Type> || std::is_same_v<std::string, Type>) {
            obj.[:e:] = j[std::meta::identifier_of(e)];
        }
        else {
            obj.[:e:] = DeserializeObject<Type>(j[std::meta::identifier_of(e)].dump());
        }
    }
    return obj;
}

template <typename T>
constexpr std::string SerializeObject(const T& obj) {
    json j;
    template for (constexpr auto e : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
        using Type = typename[:type_of(e):];
        if constexpr (std::is_fundamental_v<Type> || std::is_same_v<std::string, Type>) {
            j[std::meta::identifier_of(e)] = obj.[:e:];
        }
        else {
            j[std::meta::identifier_of(e)] = SerializeObject<Type>(obj.[:e:]);
        }
    }
    return j.dump();
}

int main() {
    std::string js = R"(
{
    "id": 1001,
    "active": true,
    "age": 21,
    "gpa": 3.82,
    "name": {
      "first": "Alice",
      "last": "Johnson"
    },
    "contact": {
      "email": "alice@example.com",
      "phone": "5551234"
    },
    "address": {
      "country": "UK",
      "city": "London",
      "postcode": "E17",
      "coordinates": {
        "latitude": 51.584,
        "longitude": -0.019
      }
    },
    "preferences": {
      "newsletter": false,
      "darkMode": true,
      "language": "en"
    }
  }
)";

    // Deserialize string into Student object
    Student s = DeserializeObject<Student>(js);
    // Serialize it back
    std::string res = SerializeObject<Student>(s);
    std::cout << res << std::endl;
}
