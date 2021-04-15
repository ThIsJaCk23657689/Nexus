#pragma once

#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace Nexus {
	class Utill {
	public:
		static glm::vec3 limit(glm::vec3 vector, float number) {
			if (glm::length(vector) > number) {
				vector = glm::normalize(vector) * number;
			}
			return vector;
		}

		static glm::vec3 clamp(glm::vec3 vector, float min, float max) {
			if (glm::length(vector) < min) {
				vector = glm::normalize(vector) * min;
			}
			if (glm::length(vector) > max) {
				vector = glm::normalize(vector) * max;
			}
			return vector;
		}

		static void Show1DVectorStatistics(std::vector<float> data, std::string title) {
			auto max = std::max_element(data.begin(), data.end());
			auto min = std::min_element(data.begin(), data.end());

			double sum = std::accumulate(data.begin(), data.end(), 0.0);
			double mean = sum / data.size();

			double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
			double variance = sq_sum / data.size() - mean * mean;
			double stdev = std::sqrt(variance);

			std::cout << "========== statistics of " << title << " ==========" << std::endl
				<< "Total: " << data.size() << std::endl
				<< "Sum: " << sum << std::endl
				<< "Max Value: " << *max << std::endl
				<< "Min Value: " << *min << std::endl
				<< "Mean: " << mean << std::endl
				<< "Variance: " << variance << std::endl
				<< "Standard Deviation: " << stdev << std::endl;
		}
	};
}