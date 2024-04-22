module;

export module BooleanNetwork:Node;

import PrimitiveTypes;

import <functional>;
import <vector>;
import <memory>;
import <cassert>;

using FunctionType = const std::function<bool(const std::vector<bool>&)>;

class Link; // forwarding reference
export class Node {
	std::vector<std::shared_ptr<Link>> inputs;
	std::vector<std::shared_ptr<Link>> outputs;
	FunctionType func;
	bool lastResult; // mostly just for debugging and printing

public:
	const u32 numBools;

	Node() = delete;
	Node(FunctionType f, u32 n, std::vector<std::shared_ptr<Link>> inputs = {}, std::vector<std::shared_ptr<Link>> outputs = {});
	~Node() = default;

	Node(const Node&) = delete;
	Node operator=(const Node&) = delete;

	auto getInputs() const -> std::vector<std::shared_ptr<Link>>;
	auto getOutputs() const -> std::vector<std::shared_ptr<Link>>;
	auto setInputs(std::vector<std::shared_ptr<Link>>) -> void;
	auto setOutputs(std::vector<std::shared_ptr<Link>>) -> void;
	auto getLastResult() const -> bool;

	auto isValid() const -> bool;

	auto doStep() -> void;
};

export class Link {
	std::shared_ptr<Node> from;
	std::shared_ptr<Node> to;
	bool curr;
	bool next;

public:
	Link();
	Link(bool, std::shared_ptr<Node>, std::shared_ptr<Node>);
	~Link() = default;

	Link(const Link&) = delete;
	Link operator=(const Link&) = delete;

	auto getFrom() const -> std::shared_ptr<Node>;
	auto getTo() const -> std::shared_ptr<Node>;
	auto getValue() const -> bool;
	auto setFrom(std::shared_ptr<Node>) -> void;
	auto setTo(std::shared_ptr<Node>) -> void;
	auto setValue(bool) -> void;

	auto doStep() -> void;
};

Node::Node(FunctionType f, u32 n, std::vector<std::shared_ptr<Link>> inputs, std::vector<std::shared_ptr<Link>> outputs) :
	func(f), inputs(inputs), outputs(outputs), numBools(n), lastResult(false)
{}

auto Node::getInputs() const -> std::vector<std::shared_ptr<Link>> {
	return this->inputs;
}
auto Node::getOutputs() const -> std::vector<std::shared_ptr<Link>> {
	return this->outputs;
}
auto Node::setInputs(std::vector<std::shared_ptr<Link>> inputs) -> void {
	this->inputs = inputs;
}
auto Node::setOutputs(std::vector<std::shared_ptr<Link>> outputs) -> void {
	this->outputs = outputs;
}
auto Node::getLastResult() const -> bool {
	return this->lastResult;
}
auto Node::isValid() const -> bool {
	return this->numBools == this->inputs.size();
}
auto Node::doStep() -> void {
	assert(this->inputs.size() == this->numBools);
	std::vector<bool> inputToFunc;
	inputToFunc.reserve(this->numBools);
	for (const auto& link : this->inputs) {
		inputToFunc.push_back(link->getValue());
	}
	this->lastResult = this->func(inputToFunc);
	for (auto& out : this->outputs) {
		out->setValue(this->lastResult);
	}
}

Link::Link() {
	this->from = nullptr;
	this->to = nullptr;
	this->curr = false;
	this->next = false;
}
Link::Link(bool value, std::shared_ptr<Node> from, std::shared_ptr<Node> to) {
	this->from = from;
	this->to = to;
	this->curr = value;
	this->next = false;
}

auto Link::getFrom() const -> std::shared_ptr<Node> {
	return this->from;
}
auto Link::getTo() const -> std::shared_ptr<Node> {
	return this->to;
}
auto Link::getValue() const -> bool {
	return this->curr;
}
auto Link::setFrom(std::shared_ptr<Node> nFrom) -> void {
	this->from = nFrom;
}
auto Link::setTo(std::shared_ptr<Node> nTo) -> void {
	this->to = nTo;
}
auto Link::setValue(bool value) -> void {
	this->next = value;
}
auto Link::doStep() -> void { // doesn't do a "reset" on next. in this way links maintain value if not given a new one
	this->curr = this->next;
}
