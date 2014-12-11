#include "NeuralNetwork.h"
#include <iostream>

NeuralNetwork::NeuralNetwork(const size_t is, const std::vector<size_t>& s, double(*weightsInit)()){
	inputSize = is;
	sizes = s;
	nlayers.reserve(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		nlayers.push_back(AffineMatrix<double>(i == 0 ? inputSize : sizes[i - 1], size_t(sizes[i]), weightsInit));

	prevActivations.resize(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		prevActivations[i].resize(sizes[i]);
}
void NeuralNetwork::backprop(std::vector<double> outputdelta){
	//Based on heavy reading from http://www.willamette.edu/~gorr/classes/cs449/backprop.html

	const double learningRate = 0.04;

	for (int i = nlayers.size() - 1; i >= 0; i--){
		std::vector<double> activity = prevActivations[i];
		activity.push_back(1);

		nlayers[i].callback([outputdelta, learningRate, activity](const size_t i, const size_t j, double& oldValue){
			oldValue += learningRate * outputdelta[i] * activity[j];
		});
		nlayers[i].biases += learningRate * outputdelta;

		//std::cout << outputdelta;

		outputdelta = - hadamardProduct(nlayers[i].transposeMultiply(outputdelta), thresholding_prime(prevActivations[i]));
	}
}
std::vector<double> NeuralNetwork::frontprop(std::vector<double> input){
	for (size_t i = 0; i < nlayers.size(); i++)
		input = thresholding(prevActivations[i] = (nlayers[i] * input));
	return input;
}
std::vector<double> NeuralNetwork::thresholding(std::vector<double> v){
	std::for_each(v.begin(), v.end(), [](double& d){d = tanh(d); });
	return v;
}
std::vector<double> NeuralNetwork::thresholding_prime(std::vector<double> v){
	std::for_each(v.begin(), v.end(), [](double& d){d = 1 - tanh(d) * tanh(d); });
	return v;
}
