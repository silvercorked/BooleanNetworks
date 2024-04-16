
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

	
	constexpr const u32 iterations = 10000;
	std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(1); // increasing this increases runtime of all instances. I think its related to reading and writing on the model and vals variables
	u32 k = 3;
	std::vector<CellularAutomata> models;
	std::vector<std::pair<std::vector<f64>, std::vector<f64>>> vals;
	for (; k <= 25; k += 2) {
		models.push_back(CellularAutomata{
			1024 * 32,
			k,
			CellularAutomataResources::ruleFunc22,
			CellularAutomataResources::meanFieldApproximationRule22,
			CellularAutomataParentConfigEnum::RANDOMIZE_PARENTS,
			CellularAutomataInitialConfigEnum::RANDOM
		});
		vals.push_back(std::make_pair<std::vector<f64>, std::vector<f64>>({}, {}));
		vals[vals.size() - 1].first.reserve(iterations);
		vals[vals.size() - 1].second.reserve(iterations);
	}
	
	for (i32 i = 0; i < models.size(); i++) {
		tp->queueTask([&model = models[i], &localVals = vals[i]]() -> void {
			auto currentTime = std::chrono::high_resolution_clock::now();
			for (u32 j = 0; j < iterations; j++) {
				auto val = model.gatherWithThreads(8); // grab one at a time to keep output coming out
				std::cout << "k: " << model.getK() << " run: " << (j + 1) << std::endl;
				//	<< "\testimate: " << val.first << std::endl
				//	<< "\tactual: " << val.second << std::endl;
				localVals.first.push_back(val.first);
				localVals.second.push_back(val.second);
				auto newTime = std::chrono::high_resolution_clock::now();
				float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>(newTime - currentTime).count();
				currentTime = newTime;
				std::cout << "Iteration Time: " << frameTime << std::endl;
			}
		});
	}

	while (tp->busy()) { std::this_thread::yield(); }
	tp.reset(); // destroy, thus joining threads. turns to nullptr
	
	for (i32 i = 0; i < models.size(); i++) {
		out << "estimated " << models[i].getK() << ", actual" << models[i].getK() << ", ";
	}
	out << std::endl;

	for (i32 i = 0; i < vals[0].first.size(); i++) {
		for (i32 j = 0; j < models.size(); j++) {
			out << vals[j].first[i] << ", " << vals[j].second[i] << ", ";
		}
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

	for (i32 i = 0; i < models.size(); i++) {
		auto estMean = Stats::arithmeticMean(vals[i].first);
		auto actMean = Stats::arithmeticMean(vals[i].second);

		auto estGeoMean = Stats::geometricMean(vals[i].first);
		auto actGeoMean = Stats::geometricMean(vals[i].second);

		auto estHarmMean = Stats::harmonicMean(vals[i].first);
		auto actHarmMean = Stats::harmonicMean(vals[i].second);

		auto estMedian = Stats::median(vals[i].first);
		auto actMedian = Stats::median(vals[i].second);

		auto estMidrange = Stats::midrange(vals[i].first);
		auto actMidrange = Stats::midrange(vals[i].second);

		//auto estMode = Stats::mode(vals.first);
		//auto actMode = Stats::mode(vals.second);

		auto estSampVariance = Stats::sampleVariance(vals[i].first, estMean);
		auto actSampVariance = Stats::sampleVariance(vals[i].second, actMean);

		auto estSampStdDeviation = Stats::sampleStandardDeviation(vals[i].first, estMean);
		auto actSampStdDeviation = Stats::sampleStandardDeviation(vals[i].second, actMean);

		auto estPopVariance = Stats::populationVariance(vals[i].first, estMean);
		auto actPopVariance = Stats::populationVariance(vals[i].second, actMean);

		auto estPopStdDeviation = Stats::populationStandardDeviation(vals[i].first, estMean);
		auto actPopStdDeviation = Stats::populationStandardDeviation(vals[i].second, actMean);

		auto estSampSkewness = Stats::sampleSkewness(vals[i].first, estMean, estSampStdDeviation);
		auto actSampSkewness = Stats::sampleSkewness(vals[i].second, actMean, actSampStdDeviation);

		auto estPopSkewness = Stats::populationSkewness(vals[i].first, estMean, estPopStdDeviation);
		auto actPopSkewness = Stats::populationSkewness(vals[i].second, actMean, actPopStdDeviation);

		auto covariance = Stats::covariance(vals[i].first, vals[i].second, estMean, actMean);
		auto sampCovariance = Stats::sampleCovariance(vals[i].first, vals[i].second, estMean, actMean);

		auto estMin = Stats::min(vals[i].first);
		auto actMin = Stats::min(vals[i].second);

		auto estMax = Stats::max(vals[i].first);
		auto actMax = Stats::max(vals[i].second);

		auto popPCC = Stats::populationPearsonCorrelationCoefficient(
			vals[i].first, vals[i].second,
			estMean, actMean,
			estPopStdDeviation, actPopStdDeviation
		);
		auto sampPCC = Stats::samplePearsonCorrelationCoefficient(
			vals[i].first, vals[i].second,
			estMean, actMean
		);

		std::cout << "-------------Results (k = " << models[i].getK() << ") -------------" << std::endl;
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
		std::cout << "\tsample covariance: " << sampCovariance << std::endl;

		std::cout << "\testimated min: " << estMin << std::endl;
		std::cout << "\tactual min: " << actMin << std::endl;

		std::cout << "\testimated max: " << estMax << std::endl;
		std::cout << "\tactual max: " << actMax << std::endl;

		std::cout << "\tpopulation Pearson Correlation Coefficient: " << popPCC << std::endl;
		std::cout << "\tsample Pearson Correlation Coefficient: " << sampPCC << std::endl;
		std::cout << "-----------End Results (k = " << models[i].getK() << ") -----------" << std::endl;

		out << "k = " << models[i].getK() << ","
			<< estMean << "," << actMean << ","
			<< estGeoMean << "," << actGeoMean << ","
			<< estHarmMean << "," << actHarmMean << ","
			<< estMedian << "," << actMedian << ","
			<< estMidrange << "," << actMidrange << ","
			<< estSampVariance << "," << actSampVariance << ","
			<< estSampStdDeviation << "," << actSampStdDeviation << ","
			<< estPopVariance << "," << actPopVariance << ","
			<< estPopStdDeviation << "," << actPopStdDeviation << ","
			<< estSampSkewness << "," << actSampSkewness << ","
			<< estPopSkewness << "," << actPopSkewness << ","
			<< covariance << "," << sampCovariance << ","
			<< estMin << "," << actMin << ","
			<< estMax << "," << actMax << ","
			<< sampPCC << "," << popPCC << ","
			<< std::endl;
	}

	return 0;
}
