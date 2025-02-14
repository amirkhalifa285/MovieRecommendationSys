#include "User.h"
#include "RecommendationSystem.h"
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cmath> 

User::User(const std::string& name, std::shared_ptr<RecommendationSystem> rs)
    : name(name), 
      rs(rs),
      movie_ratings(0, &sp_movie_hash, &sp_movie_equal) {}


std::string User::get_name() const {
    return name;
}

void User::add_movie_to_user(const std::string& name, int year,
                             const std::vector<double>& features,
                             double rating) {
    for (double val : features) {
        if (val < 1.0 || val > 10.0) {
            throw std::invalid_argument("Feature values must be between 1 and 10.");
        }
    }

    try {
        // Add movie to recommendation system
        sp_movie movie = rs->add_movie_to_rs(name, year, features);

        if (!movie) {
            throw std::runtime_error("add_movie_to_rs returned null sp_movie.");
        }

        movie_ratings[movie] = rating;

    } catch (const std::exception& e) {
        std::cerr << "[EXCEPTION] Error adding movie \"" << name << " (" << year
                  << ")\": " << e.what() << "\n";
        throw;
    }
}

sp_movie User::get_rs_recommendation_by_content() {
    return rs->recommend_by_content(movie_ratings);
}

sp_movie User::get_rs_recommendation_by_cf(int k) {
    return rs->recommend_by_cf(*this, k);
}

double User::get_rs_prediction_score_for_movie(const std::string &name, int year, int k) {
    sp_movie movie = rs->get_movie(name, year);
    if (!movie) {
        throw std::runtime_error("Movie not found.");
    }
    return rs->predict_movie_score(*this, movie, k);
}

const RecommendationSystem& User::get_rs() const {
    return *rs;
}

const rank_map& User::get_rank() const {
    return movie_ratings; // return map
}

std::ostream& operator<<(std::ostream& os, const User& user) {
    os << "name: " << user.get_name() << "\n";

    std::vector<sp_movie> sorted_movies;
    for (const auto& [movie, features] : user.get_rs().get_movies()) {
        sorted_movies.push_back(movie);
    }

    std::sort(sorted_movies.begin(), sorted_movies.end(), 
              [](const sp_movie& a, const sp_movie& b) {
                  return *a < *b;
              });

    for (const auto& movie : sorted_movies) {
        os << *movie;
    }

    os << "\n";

    return os;
}
