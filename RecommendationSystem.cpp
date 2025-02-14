#include "RecommendationSystem.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>


// Keep this unchanged - it's mathematically correct (I hope)
double cosine_similarity(const std::vector<double>& a, const std::vector<double>& b) {
    double dot_product = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot_product += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a == 0.0 || norm_b == 0.0) return 0.0;
    return dot_product / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

sp_movie RecommendationSystem::recommend_by_content(const User& user) const {
    return recommend_by_content(user.get_rank());
}

const std::unordered_map<sp_movie, std::vector<double>, decltype(&sp_movie_hash), decltype(&sp_movie_equal)>& 
RecommendationSystem::get_movies() const {
    return movies;
}
// Add a movie to the recommendation system with its features.
sp_movie RecommendationSystem::add_movie_to_rs(const std::string& name, int year,
                                               const std::vector<double>& features) {
   

    if (features.empty()) {
        throw std::invalid_argument("Features cannot be empty.");
    }

    // Enforce feature size consistency (only if movies is not empty)
    if (!movies.empty()) {
        size_t expected_size = movies.begin()->second.size();
       

        if (features.size() != expected_size) {
            std::cerr << "[ERROR] Feature size mismatch for " << name 
                      << ". Expected " << expected_size << ", got " << features.size() << std::endl;
            throw std::runtime_error("Feature size mismatch for " + name);
        }
    }

    sp_movie existing_movie = get_movie(name, year);
    if (existing_movie) {
       
        return existing_movie;
    }

    auto movie = std::make_shared<Movie>(name, year);
    movies[movie] = features;

   
    return movie;
}

sp_movie RecommendationSystem::get_movie(const std::string& name, int year) const {

    auto temp_movie = std::make_shared<Movie>(name, year);
    auto it = movies.find(temp_movie);
    return (it != movies.end()) ? it->first : nullptr;
}

const std::vector<double>& RecommendationSystem::get_movie_features(const sp_movie& movie) const {
    auto it = movies.find(movie);
    if (it == movies.end()) {
        throw std::runtime_error("Movie not found in recommendation system");
    }
    return it->second;
}

sp_movie RecommendationSystem::recommend_by_content(const rank_map& user_ratings) const {
    double average = 0.0;
    for (const auto& [movie, rating] : user_ratings) {
        average += rating;
    }
    average /= user_ratings.size();

    std::vector<double> preference_vector(movies.begin()->second.size(), 0.0);
    for (const auto& [movie, rating] : user_ratings) {
        const auto& features = get_movie_features(movie);
        double adjusted_rating = rating - average;
        for (size_t i = 0; i < features.size(); ++i) {
            preference_vector[i] += adjusted_rating * features[i];
        }
    }

    // Find best movie
    double max_similarity = -std::numeric_limits<double>::infinity();
    sp_movie best_movie = nullptr;

    for (const auto& [movie, features] : movies) {
        if (user_ratings.count(movie)) continue;

        double similarity = cosine_similarity(preference_vector, features);
        
        if (similarity > max_similarity) {
            max_similarity = similarity;
            best_movie = movie;
        }
    }

    return best_movie;
}

double RecommendationSystem::predict_movie_score(const User& user,
                                                const sp_movie& movie, int k) const {
    // Get the user's ratings from the User object
    const auto& user_ratings = user.get_rank();
    if (user_ratings.empty()) {
    throw std::invalid_argument("User has no ratings");
}
    if (user_ratings.find(movie) != user_ratings.end()) {
        return user_ratings.at(movie);
    }

    const auto& target_features = get_movie_features(movie);
    std::vector<std::pair<double, sp_movie>> similarities;

    //similarities
    for (const auto& [rated_movie, rating] : user_ratings) {
        const auto& rated_features = get_movie_features(rated_movie);
        double similarity = cosine_similarity(target_features, rated_features);
        similarities.emplace_back(similarity, rated_movie);
    }

    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    k = std::min(k, static_cast<int>(similarities.size()));
    similarities.resize(k);

    double numerator = 0.0, denominator = 0.0;
    for (const auto& [similarity, rated_movie] : similarities) {
        numerator += similarity * user_ratings.at(rated_movie);
        denominator += similarity;
    }

    return (denominator == 0.0) ? 0.0 : numerator / denominator;
}

sp_movie RecommendationSystem::recommend_by_cf(const User& user, int k) const {
    const auto& user_ratings = user.get_rank(); // Get ratings from User
    double max_score = -std::numeric_limits<double>::infinity();
    sp_movie best_movie = nullptr;

    for (const auto& [movie, features] : movies) {
        if (user_ratings.find(movie) == user_ratings.end()) { 
            double score = predict_movie_score(user, movie, k); 
            if (score > max_score) {
                max_score = score;
                best_movie = movie;
            }
        }
    }
    return best_movie;
}

// Overload the stream insertion operator to print the recommendation system's movies.
std::ostream& operator<<(std::ostream& os, const RecommendationSystem& rs) {
    std::vector<sp_movie> sorted_movies;
    for (const auto& [movie, features] : rs.movies) {
        sorted_movies.push_back(movie);
    }

    std::sort(sorted_movies.begin(), sorted_movies.end(), 
              [](const sp_movie& a, const sp_movie& b) {
                  return *a < *b;
              });

    for (const auto& movie : sorted_movies) {
        os << *movie;
    }

    return os;
}
