#include "Movie.h"
#include <iostream>

Movie::Movie(const std::string& name, int year) : name(name), year(year) {
}

std::string Movie::get_name() const {
    return name;
}

int Movie::get_year() const {
    return year;
}

bool Movie::operator<(const Movie& other) const {
    if (year != other.year) {
        return year < other.year;
    }
    return name < other.name;
}

std::ostream& operator<<(std::ostream& os, const Movie& movie) {
    os << movie.get_name() << " (" <<movie.get_year()<< ")\n";
    return os;
}

std::size_t sp_movie_hash(const sp_movie& movie) {
    if (!movie) {
        throw std::invalid_argument("Nullptr passed to sp_movie_hash");
    }
    std::size_t res = HASH_START;
    res = res * RES_MULT + std::hash<std::string>()(movie->get_name());
    res = res * RES_MULT + std::hash<int>()(movie->get_year());
    return res;
}

bool sp_movie_equal(const sp_movie& m1, const sp_movie& m2) {
    if (!m1 || !m2) {
        return false;
    }
    return !(*m1 < *m2) && !(*m2 < *m1);
}
