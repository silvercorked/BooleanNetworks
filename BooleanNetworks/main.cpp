
import BooleanNetwork;
import CellularAutomata;
import Statistics;

import PrimitiveTypes;

import <iostream>;
import <iterator>;
import <ranges>;
import <algorithm>;
import <vector>;
import <utility>;

auto main() -> int {
	
	/*
	next steps
	add bucketizing function (similar to frequency but ranges of values like histogram buckets)
	add variance online algorithms
	*/

	
	Network net{};
	bool stuffsGood = true;
	stuffsGood &= net.addNode("and1", NodeFunctions::AND_2, 2);
	stuffsGood &= net.addNode("and2", NodeFunctions::AND_2, 2);
	stuffsGood &= net.addNode("or1", NodeFunctions::OR_2, 2);
	stuffsGood &= net.addNode("buffer1", NodeFunctions::BUFFER_1, 1);
	stuffsGood &= net.addNode("neg1", NodeFunctions::NEGATE_1, 1);
	stuffsGood &= net.addNode("neg2", NodeFunctions::NEGATE_1, 1);
	stuffsGood &= net.connectNodes("and1", "or1");
	stuffsGood &= net.connectNodes("and1", "neg2");
	
	stuffsGood &= net.connectNodes("and2", "or1");

	stuffsGood &= net.connectNodes("or1", "buffer1");
	stuffsGood &= net.connectNodes("or1", "neg1");

	stuffsGood &= net.connectNodes("buffer1", "and2");

	stuffsGood &= net.connectNodes("neg1", "and1");
	stuffsGood &= net.connectNodes("neg1", "and2");

	stuffsGood &= net.connectNodes("neg2", "and1");

	if (!stuffsGood && !net.isValid())
		throw std::runtime_error("Error During Network Setup");

	for (u32 i = 0; i < 6; i++) {
		net.doStep();
		auto res = net.getAllNodeResults();
		std::cout << "!!!!!! " << (i == 0 ? "INITIAL" : "NEW") << " ROUND: " << (i + 1) << " !!!!!!" << std::endl;
		for (const auto& [nodeName, val] : res) {
			std::cout << "\t" << nodeName << ": " << val << std::endl;
		}
	}
	
	//std::vector<f32> skewTest = { 3, 8, 10, 17, 24, 27 };
	//f32 popSkew = Stats::populationSkewness(skewTest);
	//f32 sampSkew = Stats::sampleSkewness(skewTest);
	//std::cout << "pop skew: " << popSkew << std::endl;
	//std::cout << "samp skew: " << sampSkew << std::endl;

	constexpr const u32 iterations = 1000;
	CellularAutomata a(
		1024,
		15,
		CellularAutomataResources::ruleFunc22,
		CellularAutomataResources::meanFieldApproximationRule22,
		CellularAutomataParentConfigEnum::NEIGHBORHOOD_PARENTS,
		CellularAutomataInitialConfigEnum::ONE_HIGH
	);
	
	std::pair<std::vector<f64>, std::vector<f64>> vals{};
	vals.first.reserve(iterations);
	vals.second.reserve(iterations);

	std::cout << std::endl;
	for (u32 i = 0; i < iterations; i++) {
		auto val = a.gather(); // grab one at a time to keep output coming out
		std::cout << "run: " << (i + 1) << std::endl
			<< "\testimate: " << val.first << std::endl
			<< "\tactual: " << val.second << std::endl;
		vals.first.push_back(val.first);
		vals.second.push_back(val.second);
	}
	std::cout << std::endl;

	auto estMean = Stats::arithmeticMean(vals.first);
	auto actMean = Stats::arithmeticMean(vals.second);

	auto estGeoMean = Stats::geometricMean(vals.first);
	auto actGeoMean = Stats::geometricMean(vals.second);

	auto estHarmMean = Stats::harmonicMean(vals.first);
	auto actHarmMean = Stats::harmonicMean(vals.second);

	auto estMedian = Stats::median(vals.first);
	auto actMedian = Stats::median(vals.second);

	auto estMidrange = Stats::midrange(vals.first);
	auto actMidrange = Stats::midrange(vals.second);

	//auto estMode = Stats::mode(vals.first);
	//auto actMode = Stats::mode(vals.second);

	auto estSampVariance = Stats::sampleVariance(vals.first, estMean);
	auto actSampVariance = Stats::sampleVariance(vals.second, actMean);

	auto estSampStdDeviation = Stats::sampleStandardDeviation(vals.first, estMean);
	auto actSampStdDeviation = Stats::sampleStandardDeviation(vals.second, actMean);

	auto estPopVariance = Stats::populationVariance(vals.first, estMean);
	auto actPopVariance = Stats::populationVariance(vals.second, actMean);

	auto estPopStdDeviation = Stats::populationStandardDeviation(vals.first, estMean);
	auto actPopStdDeviation = Stats::populationStandardDeviation(vals.second, actMean);

	auto estSampSkewness = Stats::sampleSkewness(vals.first, estMean, estSampStdDeviation);
	auto actSampSkewness = Stats::sampleSkewness(vals.second, actMean, actSampStdDeviation);

	auto estPopSkewness = Stats::populationSkewness(vals.first, estMean, estPopStdDeviation);
	auto actPopSkewness = Stats::populationSkewness(vals.second, actMean, actPopStdDeviation);

	auto covariance = Stats::covariance(vals.first, vals.second, estMean, actMean);


	std::cout << "-------------Results-------------" << std::endl;
	std::cout << "\testimated mean: " << estMean << std::endl;
	std::cout << "\tactual mean: " << actMean << std::endl;

	std::cout << "\testimated geometric mean: " << estGeoMean << std::endl;
	std::cout << "\tactual geometric mean: " << actGeoMean << std::endl;

	std::cout << "\testimated harmonic mean: " << estHarmMean << std::endl;
	std::cout << "\tactual harmonic mean: " << actHarmMean << std::endl;

	std::cout << "\testimated median: " << estMedian << std::endl;
	std::cout << "\tactual median: " << actMedian << std::endl;

	std::cout << "\testimated midrange: " << estMidrange << std::endl;
	std::cout << "\tactual midrange: " << actMidrange << std::endl;

	//std::cout << "\testimated mode: "; // https://stackoverflow.com/questions/10750057/how-do-i-print-out-the-contents-of-a-vector
	//std::ranges::copy(estMode, std::ostream_iterator<f64>(std::cout, ", "));
	//std::cout << std::endl;
	//std::cout << "\tactual mode: ";
	//std::ranges::copy(actMode, std::ostream_iterator<f64>(std::cout, ", "));
	//std::cout << std::endl;

	std::cout << "\testimated sample variance: " << estSampVariance << std::endl;
	std::cout << "\tactual sample variance: " << actSampVariance << std::endl;

	std::cout << "\testimated sample deviation: " << estSampStdDeviation << std::endl;
	std::cout << "\tactual sample deviation: " << actSampStdDeviation << std::endl;

	std::cout << "\testimated population variance: " << estPopVariance << std::endl;
	std::cout << "\tactual population variance: " << actPopVariance << std::endl;

	std::cout << "\testimated population deviation: " << estPopStdDeviation << std::endl;
	std::cout << "\tactual population deviation: " << actPopStdDeviation << std::endl;

	std::cout << "\testimated sample skewness: " << estSampSkewness << std::endl;
	std::cout << "\tactual sample skewness: " << actSampSkewness << std::endl;

	std::cout << "\testimated population skewness: " << estPopSkewness << std::endl;
	std::cout << "\tactual population skewness: " << actPopSkewness << std::endl;

	std::cout << "\tcovariance: " << covariance << std::endl;
	std::cout << "-----------End Results-----------" << std::endl;
	
	return 0;
}
