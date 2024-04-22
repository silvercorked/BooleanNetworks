
import PrimitiveTypes;

import BooleanNetwork;
import Statistics;
import ThreadPool;
import PeriodicityTracker;

import <iostream>;
import <iterator>;
import <ranges>;
import <algorithm>;
import <vector>;
import <utility>;
import <fstream>;
import <string>;

constexpr const u32 iterations = 5000;
constexpr const u32 maxK = 25;
constexpr const u32 trialsPerK = 10;
constexpr const u32 nodeCount = 1024 * 8;
const std::string baseFileName("output");
constexpr const u32 startK = 3;

class StatsPack {
	public:
	u32 k; u32 trial;
	f64 estMean; f64 actMean;
	f64 estGeoMean; f64 actGeoMean;
	f64 estHarmMean; f64 actHarmMean;
	f64 estMedian; f64 actMedian;
	f64 estMidrange; f64 actMidrange;
	f64 estSampVariance; f64 actSampVariance;
	f64 estSampStdDeviation; f64 actSampStdDeviation;
	f64 estPopVariance; f64 actPopVariance;
	f64 estPopStdDeviation; f64 actPopStdDeviation;
	f64 estSampSkewness; f64 actSampSkewness;
	f64 estPopSkewness; f64 actPopSkewness;
	f64 covariance; f64 sampCovariance;
	f64 estMin; f64 actMin;
	f64 estMax; f64 actMax;
	f64 sampPCC; f64 popPCC;
	std::pair<std::vector<f64>, std::vector<f64>> vals;
	StatsPack() {
		this->vals = std::make_pair<std::vector<f64>, std::vector<f64>>({}, {});
	};
};
auto doEstimate(StatsPack& data, u32 k, u32 iterations) -> bool {
	auto estimator = CellularAutomataEstimator{
		k,
		CellularAutomataResources::meanFieldApproximationRule22
	};
	data.vals.first.clear();
	data.vals.first.reserve(iterations);
	for (u32 j = 0; j < iterations; j++) {
		data.vals.first.push_back(estimator.gather());
	}
	return true;
}
auto doTrial(StatsPack& data, u32 k, u32 iterations) -> bool {
	auto model = CellularAutomata{
		nodeCount,
		k,
		CellularAutomataResources::ruleFunc22,
		CellularAutomataResources::CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS,
		CellularAutomataResources::CellularAutomataInitialConfigEnum::RANDOM
	};
	PeriodicityTracker pt(10);
	data.vals.second.clear();
	data.vals.second.reserve(iterations); // may not all be used cause periodicity tracker might cause early break
	auto currentTime = std::chrono::high_resolution_clock::now();
	for (u32 j = 0; j < iterations; j++) {
		std::cout << "k: " << model.getK() << " run: " << (j + 1) << std::endl;
		data.vals.second.push_back(model.gatherWithThreads(16));
		pt.addSample(std::move(model.getNodeValues()));
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		std::cout << "Iteration Time: " << frameTime << std::endl;
		if (pt.checkForPeriodicityOfRecent())
			if (pt.checkForPeriodicityOfRecent()) // second one is here to place a breakpoint on confirms to examine process for validity
				break;
	}
	return true;
}
auto doStats(StatsPack& data) -> bool {
	data.estMean = Stats::arithmeticMean(data.vals.first);
	data.actMean = Stats::arithmeticMean(data.vals.second);

	data.estGeoMean = Stats::geometricMean(data.vals.first);
	data.actGeoMean = Stats::geometricMean(data.vals.second);

	data.estHarmMean = Stats::harmonicMean(data.vals.first);
	data.actHarmMean = Stats::harmonicMean(data.vals.second);

	data.estMedian = Stats::median(data.vals.first);
	data.actMedian = Stats::median(data.vals.second);

	data.estMidrange = Stats::midrange(data.vals.first);
	data.actMidrange = Stats::midrange(data.vals.second);

	//auto estMode = Stats::mode(vals.first);
	//auto actMode = Stats::mode(vals.second);

	data.estSampVariance = Stats::sampleVariance(data.vals.first, data.estMean);
	data.actSampVariance = Stats::sampleVariance(data.vals.second, data.actMean);

	data.estSampStdDeviation = Stats::sampleStandardDeviation(data.vals.first, data.estMean);
	data.actSampStdDeviation = Stats::sampleStandardDeviation(data.vals.second, data.actMean);

	data.estPopVariance = Stats::populationVariance(data.vals.first, data.estMean);
	data.actPopVariance = Stats::populationVariance(data.vals.second, data.actMean);

	data.estPopStdDeviation = Stats::populationStandardDeviation(data.vals.first, data.estMean);
	data.actPopStdDeviation = Stats::populationStandardDeviation(data.vals.second, data.actMean);

	data.estSampSkewness = Stats::sampleSkewness(data.vals.first, data.estMean, data.estSampStdDeviation);
	data.actSampSkewness = Stats::sampleSkewness(data.vals.second, data.actMean, data.actSampStdDeviation);

	data.estPopSkewness = Stats::populationSkewness(data.vals.first, data.estMean, data.estPopStdDeviation);
	data.actPopSkewness = Stats::populationSkewness(data.vals.second, data.actMean, data.actPopStdDeviation);

	data.covariance = Stats::covariance(data.vals.first, data.vals.second, data.estMean, data.actMean);
	data.sampCovariance = Stats::sampleCovariance(data.vals.first, data.vals.second, data.estMean, data.actMean);

	data.estMin = Stats::min(data.vals.first);
	data.actMin = Stats::min(data.vals.second);

	data.estMax = Stats::max(data.vals.first);
	data.actMax = Stats::max(data.vals.second);

	data.popPCC = Stats::populationPearsonCorrelationCoefficient(
		data.vals.first, data.vals.second,
		data.estMean, data.actMean,
		data.estPopStdDeviation, data.actPopStdDeviation
	);
	data.sampPCC = Stats::samplePearsonCorrelationCoefficient(
		data.vals.first, data.vals.second,
		data.estMean, data.actMean
	);
	return true;
}

auto dumpStatsPackInFile(StatsPack data, std::ofstream& out, const u32 maxIterations) -> void {
	out << "Trial: " << data.trial << " estimated " << data.k << ", Trial: " << data.trial << " actual " << data.k << ", ";
	out << std::endl;

	for (i32 i = 0; i < maxIterations; i++) {
		if (i < data.vals.first.size())
			out << data.vals.first[i] << ", " << data.vals.second[i] << ", ";
		else
			out << ", , "; // no vals
		out << std::endl;
	}

	out << std::endl;
	out << std::endl;

	out << ", estimated mean, actual mean"
		<< ", estimated geometric mean, actual geometric mean"
		<< ", estimated harmonic mean, actual harmonic mean"
		<< ", estimated median, actual median"
		<< ", estimated midrange, actual midrange"
		<< ", estimated sample variance, actual sample variance"
		<< ", estimated sample deviation, actual sample deviation"
		<< ", estimated population variance, actual population variance"
		<< ", estimated population deviation, actual population deviation"
		<< ", estimated sample skewness, actual sample skewness"
		<< ", estimated population skewness, actual population skewness"
		<< ", covariance, sample covariance"
		<< ", estimated min, actual min"
		<< ", estimated max, actual max"
		<< ", sample Pearson Correlation Coefficient, population Pearson Correlation Coefficient"
		<< std::endl;

	std::cout << "-------------Results (k = " << data.k << ") -------------" << std::endl;
	std::cout << "\testimated mean: " << data.estMean << std::endl;
	std::cout << "\tactual mean: " << data.actMean << std::endl;

	std::cout << "\testimated geometric mean: " << data.estGeoMean << std::endl;
	std::cout << "\tactual geometric mean: " << data.actGeoMean << std::endl;

	std::cout << "\testimated harmonic mean: " << data.estHarmMean << std::endl;
	std::cout << "\tactual harmonic mean: " << data.actHarmMean << std::endl;

	std::cout << "\testimated median: " << data.estMedian << std::endl;
	std::cout << "\tactual median: " << data.actMedian << std::endl;

	std::cout << "\testimated midrange: " << data.estMidrange << std::endl;
	std::cout << "\tactual midrange: " << data.actMidrange << std::endl;

	//std::cout << "\testimated mode: "; // https://stackoverflow.com/questions/10750057/how-do-i-print-out-the-contents-of-a-vector
	//std::ranges::copy(estMode, std::ostream_iterator<f64>(std::cout, ", "));
	//std::cout << std::endl;
	//std::cout << "\tactual mode: ";
	//std::ranges::copy(actMode, std::ostream_iterator<f64>(std::cout, ", "));
	//std::cout << std::endl;

	std::cout << "\testimated sample variance: " << data.estSampVariance << std::endl;
	std::cout << "\tactual sample variance: " << data.actSampVariance << std::endl;

	std::cout << "\testimated sample deviation: " << data.estSampStdDeviation << std::endl;
	std::cout << "\tactual sample deviation: " << data.actSampStdDeviation << std::endl;

	std::cout << "\testimated population variance: " << data.estPopVariance << std::endl;
	std::cout << "\tactual population variance: " << data.actPopVariance << std::endl;

	std::cout << "\testimated population deviation: " << data.estPopStdDeviation << std::endl;
	std::cout << "\tactual population deviation: " << data.actPopStdDeviation << std::endl;

	std::cout << "\testimated sample skewness: " << data.estSampSkewness << std::endl;
	std::cout << "\tactual sample skewness: " << data.actSampSkewness << std::endl;

	std::cout << "\testimated population skewness: " << data.estPopSkewness << std::endl;
	std::cout << "\tactual population skewness: " << data.actPopSkewness << std::endl;

	std::cout << "\tcovariance: " << data.covariance << std::endl;
	std::cout << "\tsample covariance: " << data.sampCovariance << std::endl;

	std::cout << "\testimated min: " << data.estMin << std::endl;
	std::cout << "\tactual min: " << data.actMin << std::endl;

	std::cout << "\testimated max: " << data.estMax << std::endl;
	std::cout << "\tactual max: " << data.actMax << std::endl;

	std::cout << "\tpopulation Pearson Correlation Coefficient: " << data.popPCC << std::endl;
	std::cout << "\tsample Pearson Correlation Coefficient: " << data.sampPCC << std::endl;
	std::cout << "-----------End Results (k = " << data.k << ") -----------" << std::endl;

	out << "k = " << data.k << ","
		<< data.estMean << "," << data.actMean << ","
		<< data.estGeoMean << "," << data.actGeoMean << ","
		<< data.estHarmMean << "," << data.actHarmMean << ","
		<< data.estMedian << "," << data.actMedian << ","
		<< data.estMidrange << "," << data.actMidrange << ","
		<< data.estSampVariance << "," << data.actSampVariance << ","
		<< data.estSampStdDeviation << "," << data.actSampStdDeviation << ","
		<< data.estPopVariance << "," << data.actPopVariance << ","
		<< data.estPopStdDeviation << "," << data.actPopStdDeviation << ","
		<< data.estSampSkewness << "," << data.actSampSkewness << ","
		<< data.estPopSkewness << "," << data.actPopSkewness << ","
		<< data.covariance << "," << data.sampCovariance << ","
		<< data.estMin << "," << data.actMin << ","
		<< data.estMax << "," << data.actMax << ","
		<< data.sampPCC << "," << data.popPCC << ","
		<< std::endl;
}

auto main() -> int {
	
	/*
	next steps
	add bucketizing function (similar to frequency but ranges of values like histogram buckets)
	add variance online algorithms
	*/

	/*
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
	}*/
	
	//std::vector<f32> skewTest = { 3, 8, 10, 17, 24, 27 };
	//f32 popSkew = Stats::populationSkewness(skewTest);
	//f32 sampSkew = Stats::sampleSkewness(skewTest);
	//std::cout << "pop skew: " << popSkew << std::endl;
	//std::cout << "samp skew: " << sampSkew << std::endl;

	for (u32 k = startK; k <= maxK; k += 2) {
		StatsPack data{};
		data.k = k;
		for (u32 trial = 0; trial < trialsPerK; trial++) {
			StatsPack curr = data; // copy
			curr.trial = trial + 1;
			std::string filepath(baseFileName);
			filepath += "_k" + std::to_string(curr.k) + "_t" + std::to_string(curr.trial) + ".csv";
			std::ofstream outputFile(filepath.c_str());
			doEstimate(curr, k, iterations);
			doTrial(curr, k, iterations);
			doStats(curr);
			dumpStatsPackInFile(curr, outputFile, iterations);
		}
	}

	return 0;
}
