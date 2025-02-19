#include "RecommendationSystemLoader.h"
#include "RecommendationSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>

// Helper function to trim whitespace from both ends of a string
static inline std::string trim_whitespace(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

std::shared_ptr<RecommendationSystem> RecommendationSystemLoader::create_rs_from_movies(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::shared_ptr<RecommendationSystem> rs = std::make_shared<RecommendationSystem>();
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream line_stream(line);

        // Parse movie name and year
        std::string movie_with_year;
        if (!(line_stream >> movie_with_year)) {
            throw std::runtime_error("Invalid file format: missing movie name and year.");
        }

        size_t dash_pos = movie_with_year.find('-');
        if (dash_pos == std::string::npos) {
            throw std::runtime_error("Invalid format: movie name and year must be separated by a dash '-'.");
        }

        // Extract and clean movie name
        std::string movie_name = trim_whitespace(movie_with_year.substr(0, dash_pos));
        int year = std::stoi(movie_with_year.substr(dash_pos + 1));

        // Parse features
        std::vector<double> features;
        double feature_value;
        while (line_stream >> feature_value) {
            if (feature_value < 1.0 || feature_value > 10.0) {
                throw std::runtime_error("Feature value out of range [1-10] for movie: " + movie_name);
            }
            features.push_back(feature_value);
        }

        if (rs->get_movie(movie_name, year)) {
            std::cerr << "[WARNING] Skipping duplicate movie: " 
                      << movie_name << " (" << year << ")\n";
            continue;
        }

        // Add movie to the recommendation system
        try {
            rs->add_movie_to_rs(movie_name, year, features);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Failed to add movie " << movie_name 
                      << " (" << year << "): " << e.what() << "\n";
            throw;
        }
    }

    file.close();
    return rs;
}