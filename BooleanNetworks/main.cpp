
import PrimitiveTypes;

import BooleanNetwork;
import CellularAutomata;
import Statistics;
import ThreadPool;

import <iostream>;
import <iterator>;
import <ranges>;
import <algorithm>;
import <vector>;
import <utility>;
import <fstream>;

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

auto main() -> int {
	
	std::ofstream out("output.csv");

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

	
	constexpr const u32 iterations = 100;
	constexpr const u32 maxK = 25;
	constexpr const u32 trialsPerK = 3;
	constexpr const u32 nodeCount = 1024 * 32;
	u32 k = 3;
	std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(1); // increasing this increases runtime of all instances. I think its related to reading and writing on the model and vals variables
	
	
	std::vector<StatsPack> stats((((maxK - k) / 2) + 1) * trialsPerK);
	std::vector<CellularAutomata> models;
	std::vector<std::pair<std::vector<f64>, std::vector<f64>>> vals;
	//stats.resize();
	
	u32 l = 0;
	for (; k <= maxK; k += 2) {
		for (i32 i = 0; i < trialsPerK; i++) {
			tp->queueTask([=, trial = i, statPack = &stats[l]]() -> void {
				auto model = CellularAutomata{
					nodeCount,
					k,
					CellularAutomataResources::ruleFunc22,
					CellularAutomataResources::meanFieldApproximationRule22,
					CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS,
					CellularAutomataInitialConfigEnum::RANDOM
				};
				statPack->vals.first.reserve(iterations);
				statPack->vals.second.reserve(iterations);
				auto currentTime = std::chrono::high_resolution_clock::now();
				for (u32 j = 0; j < iterations; j++) {
					auto val = model.gatherWithThreads(16); // grab one at a time to keep output coming out
					std::cout << "k: " << model.getK() << " run: " << (j + 1) << std::endl;
					//	<< "\testimate: " << val.first << std::endl
					//	<< "\tactual: " << val.second << std::endl;
					statPack->vals.first.push_back(val.first);
					statPack->vals.second.push_back(val.second);
					auto newTime = std::chrono::high_resolution_clock::now();
					float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count();
					currentTime = newTime;
					std::cout << "Iteration Time: " << frameTime << std::endl;
				}
				statPack->k = k;
				statPack->trial = trial;

				statPack->estMean = Stats::arithmeticMean(statPack->vals.first);
				statPack->actMean = Stats::arithmeticMean(statPack->vals.second);

				statPack->estGeoMean = Stats::geometricMean(statPack->vals.first);
				statPack->actGeoMean = Stats::geometricMean(statPack->vals.second);

				statPack->estHarmMean = Stats::harmonicMean(statPack->vals.first);
				statPack->actHarmMean = Stats::harmonicMean(statPack->vals.second);

				statPack->estMedian = Stats::median(statPack->vals.first);
				statPack->actMedian = Stats::median(statPack->vals.second);

				statPack->estMidrange = Stats::midrange(statPack->vals.first);
				statPack->actMidrange = Stats::midrange(statPack->vals.second);

				//auto estMode = Stats::mode(vals.first);
				//auto actMode = Stats::mode(vals.second);

				statPack->estSampVariance = Stats::sampleVariance(statPack->vals.first, statPack->estMean);
				statPack->actSampVariance = Stats::sampleVariance(statPack->vals.second, statPack->actMean);

				statPack->estSampStdDeviation = Stats::sampleStandardDeviation(statPack->vals.first, statPack->estMean);
				statPack->actSampStdDeviation = Stats::sampleStandardDeviation(statPack->vals.second, statPack->actMean);

				statPack->estPopVariance = Stats::populationVariance(statPack->vals.first, statPack->estMean);
				statPack->actPopVariance = Stats::populationVariance(statPack->vals.second, statPack->actMean);

				statPack->estPopStdDeviation = Stats::populationStandardDeviation(statPack->vals.first, statPack->estMean);
				statPack->actPopStdDeviation = Stats::populationStandardDeviation(statPack->vals.second, statPack->actMean);

				statPack->estSampSkewness = Stats::sampleSkewness(statPack->vals.first, statPack->estMean, statPack->estSampStdDeviation);
				statPack->actSampSkewness = Stats::sampleSkewness(statPack->vals.second, statPack->actMean, statPack->actSampStdDeviation);

				statPack->estPopSkewness = Stats::populationSkewness(statPack->vals.first, statPack->estMean, statPack->estPopStdDeviation);
				statPack->actPopSkewness = Stats::populationSkewness(statPack->vals.second, statPack->actMean, statPack->actPopStdDeviation);

				statPack->covariance = Stats::covariance(statPack->vals.first, statPack->vals.second, statPack->estMean, statPack->actMean);
				statPack->sampCovariance = Stats::sampleCovariance(statPack->vals.first, statPack->vals.second, statPack->estMean, statPack->actMean);

				statPack->estMin = Stats::min(statPack->vals.first);
				statPack->actMin = Stats::min(statPack->vals.second);

				statPack->estMax = Stats::max(statPack->vals.first);
				statPack->actMax = Stats::max(statPack->vals.second);

				statPack->popPCC = Stats::populationPearsonCorrelationCoefficient(
					statPack->vals.first, statPack->vals.second,
					statPack->estMean, statPack->actMean,
					statPack->estPopStdDeviation, statPack->actPopStdDeviation
				);
				statPack->sampPCC = Stats::samplePearsonCorrelationCoefficient(
					statPack->vals.first, statPack->vals.second,
					statPack->estMean, statPack->actMean
				);
			});
			l++;
		}
	}
	

	while (tp->busy()) { std::this_thread::yield(); }
	tp.reset(); // destroy, thus joining threads. turns to nullptr
	
	/*for (i32 i = 0; i < stats.size(); i++) {
		out << "estimated " << stats[i].k << ", actual" << stats[i].k << ", ";
	}
	out << std::endl;

	for (i32 i = 0; i < vals[0].first.size(); i++) {
		for (i32 j = 0; j < models.size(); j++) {
			out << vals[j].first[i] << ", " << vals[j].second[i] << ", ";
		}
		out << std::endl;
	}

	out << std::endl;
	out << std::endl;*/

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

	for (i32 i = 0; i < stats.size(); i++) {
	//	auto estMean = Stats::arithmeticMean(vals[i].first);
	//	auto actMean = Stats::arithmeticMean(vals[i].second);

	//	auto estGeoMean = Stats::geometricMean(vals[i].first);
	//	auto actGeoMean = Stats::geometricMean(vals[i].second);

	//	auto estHarmMean = Stats::harmonicMean(vals[i].first);
	//	auto actHarmMean = Stats::harmonicMean(vals[i].second);

	//	auto estMedian = Stats::median(vals[i].first);
	//	auto actMedian = Stats::median(vals[i].second);

	//	auto estMidrange = Stats::midrange(vals[i].first);
	//	auto actMidrange = Stats::midrange(vals[i].second);

	//	//auto estMode = Stats::mode(vals.first);
	//	//auto actMode = Stats::mode(vals.second);

	//	auto estSampVariance = Stats::sampleVariance(vals[i].first, estMean);
	//	auto actSampVariance = Stats::sampleVariance(vals[i].second, actMean);

	//	auto estSampStdDeviation = Stats::sampleStandardDeviation(vals[i].first, estMean);
	//	auto actSampStdDeviation = Stats::sampleStandardDeviation(vals[i].second, actMean);

	//	auto estPopVariance = Stats::populationVariance(vals[i].first, estMean);
	//	auto actPopVariance = Stats::populationVariance(vals[i].second, actMean);

	//	auto estPopStdDeviation = Stats::populationStandardDeviation(vals[i].first, estMean);
	//	auto actPopStdDeviation = Stats::populationStandardDeviation(vals[i].second, actMean);

	//	auto estSampSkewness = Stats::sampleSkewness(vals[i].first, estMean, estSampStdDeviation);
	//	auto actSampSkewness = Stats::sampleSkewness(vals[i].second, actMean, actSampStdDeviation);

	//	auto estPopSkewness = Stats::populationSkewness(vals[i].first, estMean, estPopStdDeviation);
	//	auto actPopSkewness = Stats::populationSkewness(vals[i].second, actMean, actPopStdDeviation);

	//	auto covariance = Stats::covariance(vals[i].first, vals[i].second, estMean, actMean);
	//	auto sampCovariance = Stats::sampleCovariance(vals[i].first, vals[i].second, estMean, actMean);

	//	auto estMin = Stats::min(vals[i].first);
	//	auto actMin = Stats::min(vals[i].second);

	//	auto estMax = Stats::max(vals[i].first);
	//	auto actMax = Stats::max(vals[i].second);

	//	auto popPCC = Stats::populationPearsonCorrelationCoefficient(
	//		vals[i].first, vals[i].second,
	//		estMean, actMean,
	//		estPopStdDeviation, actPopStdDeviation
	//	);
	//	auto sampPCC = Stats::samplePearsonCorrelationCoefficient(
	//		vals[i].first, vals[i].second,
	//		estMean, actMean
	//	);

		std::cout << "-------------Results (k = " << stats[i].k << ") -------------" << std::endl;
		std::cout << "\testimated mean: " << stats[i].estMean << std::endl;
		std::cout << "\tactual mean: " << stats[i].actMean << std::endl;

		std::cout << "\testimated geometric mean: " << stats[i].estGeoMean << std::endl;
		std::cout << "\tactual geometric mean: " << stats[i].actGeoMean << std::endl;

		std::cout << "\testimated harmonic mean: " << stats[i].estHarmMean << std::endl;
		std::cout << "\tactual harmonic mean: " << stats[i].actHarmMean << std::endl;

		std::cout << "\testimated median: " << stats[i].estMedian << std::endl;
		std::cout << "\tactual median: " << stats[i].actMedian << std::endl;

		std::cout << "\testimated midrange: " << stats[i].estMidrange << std::endl;
		std::cout << "\tactual midrange: " << stats[i].actMidrange << std::endl;

		//std::cout << "\testimated mode: "; // https://stackoverflow.com/questions/10750057/how-do-i-print-out-the-contents-of-a-vector
		//std::ranges::copy(estMode, std::ostream_iterator<f64>(std::cout, ", "));
		//std::cout << std::endl;
		//std::cout << "\tactual mode: ";
		//std::ranges::copy(actMode, std::ostream_iterator<f64>(std::cout, ", "));
		//std::cout << std::endl;

		std::cout << "\testimated sample variance: " << stats[i].estSampVariance << std::endl;
		std::cout << "\tactual sample variance: " << stats[i].actSampVariance << std::endl;

		std::cout << "\testimated sample deviation: " << stats[i].estSampStdDeviation << std::endl;
		std::cout << "\tactual sample deviation: " << stats[i].actSampStdDeviation << std::endl;

		std::cout << "\testimated population variance: " << stats[i].estPopVariance << std::endl;
		std::cout << "\tactual population variance: " << stats[i].actPopVariance << std::endl;

		std::cout << "\testimated population deviation: " << stats[i].estPopStdDeviation << std::endl;
		std::cout << "\tactual population deviation: " << stats[i].actPopStdDeviation << std::endl;

		std::cout << "\testimated sample skewness: " << stats[i].estSampSkewness << std::endl;
		std::cout << "\tactual sample skewness: " << stats[i].actSampSkewness << std::endl;

		std::cout << "\testimated population skewness: " << stats[i].estPopSkewness << std::endl;
		std::cout << "\tactual population skewness: " << stats[i].actPopSkewness << std::endl;

		std::cout << "\tcovariance: " << stats[i].covariance << std::endl;
		std::cout << "\tsample covariance: " << stats[i].sampCovariance << std::endl;

		std::cout << "\testimated min: " << stats[i].estMin << std::endl;
		std::cout << "\tactual min: " << stats[i].actMin << std::endl;

		std::cout << "\testimated max: " << stats[i].estMax << std::endl;
		std::cout << "\tactual max: " << stats[i].actMax << std::endl;

		std::cout << "\tpopulation Pearson Correlation Coefficient: " << stats[i].popPCC << std::endl;
		std::cout << "\tsample Pearson Correlation Coefficient: " << stats[i].sampPCC << std::endl;
		std::cout << "-----------End Results (k = " << stats[i].k << ") -----------" << std::endl;

		out << "k = " << stats[i].k << ","
			<< stats[i].estMean << "," << stats[i].actMean << ","
			<< stats[i].estGeoMean << "," << stats[i].actGeoMean << ","
			<< stats[i].estHarmMean << "," << stats[i].actHarmMean << ","
			<< stats[i].estMedian << "," << stats[i].actMedian << ","
			<< stats[i].estMidrange << "," << stats[i].actMidrange << ","
			<< stats[i].estSampVariance << "," << stats[i].actSampVariance << ","
			<< stats[i].estSampStdDeviation << "," << stats[i].actSampStdDeviation << ","
			<< stats[i].estPopVariance << "," << stats[i].actPopVariance << ","
			<< stats[i].estPopStdDeviation << "," << stats[i].actPopStdDeviation << ","
			<< stats[i].estSampSkewness << "," << stats[i].actSampSkewness << ","
			<< stats[i].estPopSkewness << "," << stats[i].actPopSkewness << ","
			<< stats[i].covariance << "," << stats[i].sampCovariance << ","
			<< stats[i].estMin << "," << stats[i].actMin << ","
			<< stats[i].estMax << "," << stats[i].actMax << ","
			<< stats[i].sampPCC << "," << stats[i].popPCC << ","
			<< std::endl;
	}

	return 0;
}
