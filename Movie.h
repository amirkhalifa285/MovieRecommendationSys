#ifndef EX5_MOVIE_H
#define EX5_MOVIE_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>

#define HASH_START 17
#define RES_MULT 31


class Movie;
typedef std::shared_ptr<Movie> sp_movie;

typedef std::size_t (*hash_func)(const sp_movie& movie);
typedef bool (*equal_func)(const sp_movie& m1, const sp_movie& m2);

std::size_t sp_movie_hash(const sp_movie& movie);
bool sp_movie_equal(const sp_movie& m1, const sp_movie& m2);

class Movie {
private:
    std::string name;
    int year;

public:
    Movie(const std::string& name, int year);

    std::string get_name() const;
    int get_year() const;

    bool operator<(const Movie& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Movie& movie);
};

#endif // EX5_MOVIE_H
