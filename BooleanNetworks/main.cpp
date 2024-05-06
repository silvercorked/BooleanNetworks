
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
import <future>;
import <thread>;
import <optional>;

constexpr const u32 iterations = 5000;
constexpr const u32 maxK = 39;
constexpr const u32 power = 13;
const u32 nodeCount = std::pow(2, power);
const std::string baseFileName("output");
constexpr const u32 startK = 3;
constexpr const u32 kIncrementationRate = 2;

class StatsPack {
	public:
	u32 k; u32 trial; u32 nodeCount;
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
	f64 MWuStat; f64 MWWtStat; f64 spearman; f64 wilcoxon;
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
auto doTrial(StatsPack& data, u32 iterations, bool allowPeriodicity) -> bool {
	auto model = CellularAutomata{
		data.nodeCount,
		data.k,
		CellularAutomataResources::ruleFunc22,
		CellularAutomataResources::CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS,
		CellularAutomataResources::CellularAutomataInitialConfigEnum::RANDOM
	};
	PeriodicityTracker pt(32);
	data.vals.second.clear();
	data.vals.second.reserve(iterations); // may not all be used cause periodicity tracker might cause early break
	auto currentTime = std::chrono::high_resolution_clock::now();
	for (u32 j = 0; j < iterations; j++) {
		
		data.vals.second.push_back(model.gather());
		pt.addSample(std::move(model.getNodeValues()));
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		std::cout << "Trial #: " << std::to_string(data.trial) << " Iteration Time: " << frameTime <<
			std::endl << "nodeCount: " << std::to_string(data.nodeCount) << " k: " << model.getK() << " run: " << (j + 1) << std::endl;
		if (!allowPeriodicity) {
			if (pt.checkForPeriodicityOfRecentWithThreads())
				return false; // ended early
		}
	}
	return true;
}
auto doStats(StatsPack& data) -> bool {
	if (data.vals.first.size() != data.vals.second.size()) {
		if (data.vals.first.size() > data.vals.second.size()) {
			data.vals.first.resize(data.vals.second.size());
		}
		else {
			throw std::runtime_error("estimated data is less than actual data");
			//data.vals.second.resize(data.vals.first.size()); // shouldn't ever happen
		}
	}
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

	data.MWuStat = Stats::mannWhitenyUStatistic(data.vals.first, data.vals.second);
	data.MWWtStat = Stats::MWWSignedRanksTStatistic(data.vals.first, data.vals.second);
	data.spearman = Stats::spearmanRankCorrelationCoefficient(data.vals.first, data.vals.second);
	data.wilcoxon = Stats::wilcoxonSignedRankTest(data.vals.first, data.vals.second);
	return true;
}

auto dumpStatsPackInFile(StatsPack data, std::ofstream& out, const u32 maxIterations) -> void {
	out << "Trial: " << data.trial << " estimated " << data.k << ", Trial: " << data.trial << " actual " << data.k << ", ";
	out << std::endl;

	for (i32 i = 0; i < maxIterations; i++) {
		if (i < data.vals.second.size())
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
		<< ", Mann Whiteny U Statistic, Mann-Whiteny-Wilcoxon Signed Ranks T Statistic"
		<< ", Spearman Rank Correlation Coefficient, Wilcoxon Signed Rank Test"
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

	std::cout << "\tMann Whiteny U Statistic: " << data.MWuStat << std::endl;
	std::cout << "\tMann-Whiteny-Wilcoxon Signed Ranks T Statistic: " << data.MWWtStat << std::endl;
	std::cout << "\tSpearman Rank Correlation Coefficient: " << data.spearman << std::endl;
	std::cout << "\tWilcoxon Signed Rank Test: " << data.wilcoxon << std::endl;
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
		<< data.MWuStat << "," << data.MWWtStat << ","
		<< data.spearman << "," << data.wilcoxon << ","
		<< std::endl;
}

auto doTrialsForK(u32 k, u32 nodeCountMulti) -> void {
	std::cout << "k: " << k << " thread id: " << std::this_thread::get_id() << std::endl;
	StatsPack data{};
	data.nodeCount = nodeCount * nodeCountMulti;
	data.k = k;
	const u32 trialMaxPerK = 1;
	bool hasHadFullLengthTrial = false;
	bool isDone = false;
	bool allowPeriodicity = false;
	doEstimate(data, k, iterations); // only need to do estimate once, since it won't change per trial
	for (u32 trial = 0; !isDone; trial++) {
		if (trial < trialMaxPerK) {
			isDone = false;
		}
		else {
			isDone = hasHadFullLengthTrial;
			if (isDone)
				break;
		}
		if (trial >= 250)
			allowPeriodicity = true; // if after 1000 attempts, don't get a non-recurring sample, end trial
		StatsPack curr = data; // copy
		curr.trial = trial + 1;
		bool ranFullLength = doTrial(curr, iterations, allowPeriodicity);
		hasHadFullLengthTrial |= ranFullLength;
		doStats(curr);
		if (ranFullLength) {// only save to file if got all iterations. (avoids lots of small files from incomplete tests
			std::string filepath(baseFileName);
			filepath += "_k" + std::to_string(curr.k) + "_t" + std::to_string(curr.trial) + "_node" + std::to_string(power) + ".csv";
			std::ofstream outputFile(filepath.c_str());
			dumpStatsPackInFile(curr, outputFile, iterations);
		}
		
	}
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
	/*StatsPack data{};
	data.k = 3;
	StatsPack curr = data; // copy
	curr.trial = 1;
	std::string filepath(baseFileName);
	filepath += "_k" + std::to_string(curr.k) + "_t" + std::to_string(curr.trial) + "test.csv";
	std::ofstream outputFile(filepath.c_str());
	doEstimate(curr, data.k, iterations);
	doTrial(curr, data.k, iterations);
	doStats(curr);
	dumpStatsPackInFile(curr, outputFile, iterations);
	for (const auto elem : curr.vals.first) {
		std::cout << "val: " << elem << std::endl;
	}*/
	std::vector<std::future<void>> futures;
	u32 maxThreads = 3;
	u32 currThreads = 0;
	const auto checkTime = std::chrono::microseconds(1);
	auto checkIfAnyFuturesAreDone = [&checkTime](const std::vector<std::future<void>>& futures) -> std::optional<std::size_t> {
		for (auto i = 0; i < futures.size(); i++) {
			if (std::future_status::ready == futures[i].wait_for(checkTime))
				return std::make_optional(i); // ith future is done
		}
		return {};
	};

	for (u32 nodeCountMulti = 1; nodeCountMulti <= 8; nodeCountMulti *= 2) {
		for (u32 k = startK; k <= maxK;) {
			if (currThreads >= maxThreads) {
				auto ret = checkIfAnyFuturesAreDone(futures);
				if (ret.has_value()) {
					futures.erase(futures.begin() + ret.value());
					currThreads--;
				}
				else {
					std::this_thread::yield(); // give it another shot in next time slice
				}
			}
			else {
				futures.emplace_back(
					std::async(std::launch::async, doTrialsForK, k, nodeCountMulti)
				);
				currThreads++;
				k += kIncrementationRate;
			}
		}
	}
	
	for (auto i = 0; i < futures.size(); i++) {
		futures[i].wait();
	}
	/*std::vector<f32> ranksTest = { 3, 8, 10, 17, 24, 27, -4, -5, -6, 5, 3, 3, 8, -5 };
	std::vector<f32> ranksTest2 = { 1, 2, 3, -1, -5, -4, 25, 17, -90, 2, 3, -1, -2, 5 };
	auto ranks = Stats::getOrdinalRanks(ranksTest);
	auto fractionalRanks = Stats::getFractionalRanks(ranksTest);
	for (auto i = 0; i < ranksTest.size(); i++) {
		std::cout << "value: " << ranksTest[i] << " rank: " << ranks[i] << " fractional rank: " << fractionalRanks[i] << std::endl;
	}

	auto uStat = Stats::mannWhitenyUStatistic(ranksTest, ranksTest2);
	auto tStat = Stats::MWWSignedRanksTStatistic(ranksTest, ranksTest2);
	auto pStat = Stats::spearmanRankCorrelationCoefficient(ranksTest, ranksTest2);
	auto wStat = Stats::wilcoxonSignedRankTest(ranksTest, ranksTest2);

	std::cout << "u: " << uStat << " t: " << tStat << " pStat: " << pStat << " w: " << wStat << std::endl;
	*/
	/*const u32 iterations = 5000;
	std::vector<f64> data{};
	auto estimator = CellularAutomataEstimator{
		7,
		CellularAutomataResources::meanFieldApproximationRule22
	};
	data.reserve(iterations);
	for (u32 j = 0; j < iterations; j++) {
		data.push_back(estimator.gather());
		//if (j > 2500 && j % 50 == 0)
			//estimator.setEstimate(data[j] + 0.01);
	}
	for (const auto elem : data) {
		std::cout << "val: " << elem << std::endl;
	}*/

	return 0;
}
