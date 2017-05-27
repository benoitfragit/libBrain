/**
 * \file brain_layer.h
 * \brief Define the API to build a Layer
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef BRAIN_LAYER_H
#define BRAIN_LAYER_H

#include "brain_types.h"

/**
 * \fn BrainLayer new_layer(const BrainUint number_of_neurons,
 *                          const BrainUint number_of_inputs,
 *                          BrainLayer prev)
 * \brief Fonction to create a BrainLayer from an XML context
 *
 * \param number_of_neurons Number of neurons in this layer
 * \param number_of_inputs size of the input signal
 * \param prev the previous layer
 *
 * \return a new allocated BrainLayer or NULL if it failed
 */
BrainLayer  new_layer                 (const BrainUint number_of_neurons,
                                       const BrainUint number_of_inputs,
                                       BrainLayer prev);
/**
 * \fn BrainNeuron get_layer_neuron(const BrainLayer layer, const BrainUint index)
 * \brief get a Neuron from the layer
 *
 * \param layer a BrainLayer
 * \param index the neuron to extract
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron get_layer_neuron          (const BrainLayer layer,
                                       const BrainUint index);
/**
 * \fn BrainUint get_layer_number_of_neuron(const BrainLayer layer)
 * \brief get the number of neuron in this layer
 *
 * \param layer a BrainLayer
 * \return the number of neuron in this layer
 */
BrainUint   get_layer_number_of_neuron(const BrainLayer layer);
/**
 * \fn BrainSignal get_layer_output(const BrainLayer layer)
 * \brief get the output of a layer
 *
 * \param layer a BrainLayer
 * \return a BrainSignal output of the this layer
 */
BrainSignal get_layer_output          (const BrainLayer layer);
/**
 * \fn BrainLayer get_layer_next_layer(const BrainLayer layer)
 * \brief get the next layer
 *
 * \param layer a BrainLayerex
 * \return a BrainLayer or NULL if it is the output layer
 */
BrainLayer  get_layer_next_layer      (const BrainLayer layer);
/**
 * \fn BrainLayer get_layer_previous_layer(const BrainLayer layer)
 * \brief get the previous layer
 *
 * \param layer a BrainLayer
 * \return a BrainLayer or NULL if it is the input layer
 */
BrainLayer  get_layer_previous_layer      (const BrainLayer layer);
/**
 * \fn void delete_layer(BrainLayer layer)
 * \brief free all layer allocated memory
 *
 * \param layer a BrainLayer
 */
void        delete_layer              (BrainLayer layer);
/**
 * \fn void set_layer_input(const BrainLayer layer,
 *                          const BrainUint number_of_inputs,
 *                          const BrainSignal in)
 * \brief set the layer input
 *
 * this method set the input of a layer, then compute the activation
 * before sending the layer's output to the next_layer
 *
 * \param layer            a BrainLayer
 * \param number_of_inputs length of the input signal
 * \param in               the input_signal
 */
void        set_layer_input           (BrainLayer layer,
                                       const BrainUint number_of_inputs,
                                       const BrainSignal in);
/**
 * \fn BrainDouble backpropagate_output_layer(BrainLayer output_layer, const BrainUint number_of_output, const BrainSignal output, const BrainSignal desired)
 * \brief apply backpropagation algorithm on an output layer and return the error between computed and desired outputs
 *
 * \param output_layer the BrainLayer
 * \param number_of_output the size of the output vector
 * \param desired the desired output
 *
 * \return the error rate
 */
BrainDouble backpropagate_output_layer(BrainLayer output_layer,
                                const BrainUint number_of_output,
                                const BrainSignal desired);
/**
 * \fn void backpropagate_hidden_layer(BrainLayer hidden_layer)
 * \brief apply backpropagation algorithm on an hidden layer
 *
 * \param hidden_layer the BrainLayer
 */
void backpropagate_hidden_layer(BrainLayer hidden_layer);
/**
 * \fn void apply_layer_correction(BrainLayer layer)
 * \brief apply correction to a layer to reduce total error
 *
 * \param layer a BrainLayer
 */
void apply_layer_correction(BrainLayer layer);
#endif /* BRAIN_LAYER_H */