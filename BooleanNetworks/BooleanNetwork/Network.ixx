module;

export module BooleanNetwork:Network;

import PrimitiveTypes;

import :Node;

import <vector>;
import <memory>;
import <unordered_map>;
import <string>;

export class Network {
	std::unordered_map<std::string, std::shared_ptr<Link>> links;
	std::unordered_map<std::string, std::shared_ptr<Node>> nodes;

public:
	Network();
	~Network();

	Network(const Network&) = delete;
	Network operator=(const Network&) = delete;

	auto addEmptyLink(const std::string&) -> bool;
	auto addLink(const std::string&, bool, const std::string & = "", const std::string & = "") -> bool;
	auto addNode(const std::string&, FunctionType, u32) -> bool;
	auto addNode(const std::string&, FunctionType, u32, const std::vector<std::string>&, const std::vector<std::string>&) -> bool;
	auto connectNodes(const std::string&, const std::string&) -> bool;

	//auto getLink(const std::string&) const -> std::shared_ptr<Link>;
	//auto getNode(const std::string&) const -> std::shared_ptr<Node>;
	auto setLinkFrom(const std::string&, const std::string&) -> bool;
	auto setLinkTo(const std::string&, const std::string&) -> bool;
	auto setLinkValue(const std::string&, bool) -> bool;
	auto setNodeInputs(const std::string&, const std::vector<std::string>&) -> bool;
	auto setNodeOutputs(const std::string&, const std::vector<std::string>&) -> bool;

	auto doStep() -> void;
	auto getAllLinkResults() const -> std::vector<std::pair<std::string, bool>>;
	auto getAllNodeResults() const -> std::vector<std::pair<std::string, bool>>;

	auto isValid() const -> bool;
};

Network::Network() : links(), nodes() {}
Network::~Network() {}

auto Network::addEmptyLink(const std::string& linkName) -> bool {
	if (this->links.find(linkName) != this->links.end()) return false; // need unique name
	this->links[linkName] = std::make_shared<Link>();
	return true;
}
auto Network::addLink(
	const std::string& linkName,
	bool value,
	const std::string& fromLink,
	const std::string& toLink
) -> bool {
	if (this->links.find(linkName) != this->links.end()) return false;  // need unique name
	auto fromNodePair = this->nodes.find(fromLink);
	auto toNodePair = this->nodes.find(toLink);
	if (fromNodePair == this->nodes.end() || toNodePair == this->nodes.end()) return false; // nodes must exist
	this->links[linkName] = std::make_shared<Link>(value, fromNodePair->second, toNodePair->second);
	return true;
}
auto Network::addNode(const std::string& nodeName, FunctionType function, u32 numBools) -> bool {
	if (this->nodes.find(nodeName) != this->nodes.end()) return false; // need unique name
	this->nodes[nodeName] = std::make_shared<Node>(function, numBools);
	return true;
}
auto Network::addNode(
	const std::string& nodeName,
	FunctionType function,
	u32 numBools,
	const std::vector<std::string>& inputLinkNames,
	const std::vector<std::string>& outputLinkNames
) -> bool {
	if (this->nodes.find(nodeName) != this->nodes.end()) return false; // need unique name
	if (numBools != inputLinkNames.size()) return false; // vector size of inputLinkNames cant be different than number of func inputs
	std::vector<std::shared_ptr<Link>> inputLinks{};
	inputLinks.reserve(inputLinkNames.size());
	std::vector<std::shared_ptr<Link>> outputLinks{};
	outputLinks.reserve(outputLinkNames.size());
	for (uint32_t i = 0; i < inputLinkNames.size(); i++) {
		auto linkFound = this->links.find(inputLinkNames[i]);
		if (linkFound == this->links.end()) return false;
		inputLinks.push_back(linkFound->second);
	}
	for (uint32_t i = 0; i < outputLinkNames.size(); i++) {
		auto linkFound = this->links.find(outputLinkNames[i]);
		if (linkFound == this->links.end()) return false;
		outputLinks.push_back(linkFound->second);
	}
	this->nodes[nodeName] = std::make_shared<Node>(function, numBools, inputLinks, outputLinks);
	return true;
}
auto Network::connectNodes(const std::string& from, const std::string& to) -> bool {
	if (this->nodes.find(from) == this->nodes.end()) return false; // needs to exist
	if (this->nodes.find(to) == this->nodes.end()) return false; // needs to exist
	auto fromOutLinks = this->nodes[from]->getOutputs();
	auto toInLinks = this->nodes[to]->getInputs();
	if (toInLinks.size() >= this->nodes[to]->numBools) return false; // cant have too many links (only need to check to side cause outputs are unlimited)

	std::string linkName;
	u32 count = 0;
	do {
		linkName = from + "_to_" + to + std::to_string(count);
	} while (!this->addEmptyLink(linkName)); // get a unique link name

	fromOutLinks.push_back(this->links[linkName]);
	toInLinks.push_back(this->links[linkName]);

	this->nodes[from]->setOutputs(fromOutLinks);
	this->nodes[to]->setInputs(toInLinks);
	return true;
}

auto Network::setLinkFrom(
	const std::string& linkName,
	const std::string& nodeToMakeFrom
) -> bool {
	auto linkPair = this->links.find(linkName); // can pre-load these cause success assumes means these already exist
	auto nodePair = this->nodes.find(nodeToMakeFrom);
	if (linkPair == this->links.end() || nodePair == this->nodes.end()) return false;
	linkPair->second->setFrom(nodePair->second);
	return true;
}
auto Network::setLinkTo(
	const std::string& linkName,
	const std::string& nodeToMakeTo
) -> bool {
	auto linkPair = this->links.find(linkName); // can pre-load these cause success assumes means these already exist
	auto nodePair = this->nodes.find(nodeToMakeTo);
	if (linkPair == this->links.end() || nodePair == this->nodes.end()) return false;
	linkPair->second->setTo(nodePair->second);
	return true;
}
auto Network::setLinkValue(
	const std::string& linkName,
	bool value
) -> bool {
	auto linkPair = this->links.find(linkName); // can pre-load these cause success assumes means these already exist
	if (linkPair == this->links.end()) return false;
	linkPair->second->setValue(value);
	linkPair->second->doStep();
	return true;
}
auto Network::setNodeInputs(
	const std::string& nodeName,
	const std::vector<std::string>& inputLinkNames
) -> bool {
	auto nodePair = this->nodes.find(nodeName); // can pre-load these cause success assumes means these already exist
	if (nodePair == this->nodes.end()) return false;
	if (nodePair->second->numBools != inputLinkNames.size()) return false; // vector size of inputLinkNames cant be different than number of func inputs
	std::vector<std::shared_ptr<Link>> inputLinks{};
	inputLinks.reserve(inputLinkNames.size());
	for (uint32_t i = 0; i < inputLinkNames.size(); i++) {
		auto linkFound = this->links.find(inputLinkNames[i]);
		if (linkFound == this->links.end()) return false;
		inputLinks.push_back(linkFound->second);
	}
	nodePair->second->setInputs(inputLinks);
	return true;
}
auto Network::setNodeOutputs(
	const std::string& nodeName,
	const std::vector<std::string>& outputLinkNames
) -> bool {
	auto nodePair = this->nodes.find(nodeName); // can pre-load these cause success assumes means these already exist
	if (nodePair == this->nodes.end()) return false;
	std::vector<std::shared_ptr<Link>> outputLinks{};
	outputLinks.reserve(outputLinkNames.size());
	for (uint32_t i = 0; i < outputLinkNames.size(); i++) {
		auto linkFound = this->links.find(outputLinkNames[i]);
		if (linkFound == this->links.end()) return false;
		outputLinks.push_back(linkFound->second);
	}
	nodePair->second->setOutputs(outputLinks);
	return true;
}

auto Network::doStep() -> void {
	for (auto& [nodeName, node] : this->nodes) { // nodes don't connect directly, so no issue doing them in any order
		node->doStep(); // let all nodes process at the same time, loading all results into link.next
	}
	for (auto& [linkName, link] : this->links) { // links don't connect directly, so no issue doing them in any order
		link->doStep(); // transfer link.next to link.curr, marking the completion of the discrete time step
	}
}
auto Network::getAllLinkResults() const -> std::vector<std::pair<std::string, bool>> {
	std::vector<std::pair<std::string, bool>> output;
	output.reserve(this->links.size());
	for (auto& [linkName, link] : this->links) {
		output.push_back(
			std::make_pair(linkName, link->getValue())
		);
	}
	return output;
}
auto Network::getAllNodeResults() const -> std::vector<std::pair<std::string, bool>> {
	std::vector<std::pair<std::string, bool>> output;
	output.reserve(this->nodes.size());
	for (auto& [nodeName, node] : this->nodes) {
		output.push_back(
			std::make_pair(nodeName, node->getLastResult())
		);
	}
	return output;
}
auto Network::isValid() const -> bool {
	bool valid = true;
	for (const auto& [nodeName, node] : this->nodes)
		valid &= node->isValid();
	return valid;
}
