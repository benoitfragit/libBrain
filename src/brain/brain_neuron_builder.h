/**
 * \file brain_neuron_builder.h
 * \brief Define the API to create an BrainNeuron
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa BrainNeuron
 */
#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_types.h"

/**
 * \fn BrainNeuron new_neuron(const BrainUint number_of_inputs,
 *                            const BrainSettings settings,
 *                            BrainDouble* out,
 *                            BrainWeight weighted_delta)
 * \brief method to build a neuron
 *
 * \param number_of_inputs input_signal_size
 * \param settings       network settings
 * \param out            a pointer to a BrainDouble owned by the BrainLayer
 * \param weighted_delta an array owned by the BrainLayer to update weighted deltas
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron new_neuron(const BrainUint           number_of_inputs,
                       const BrainSettings       settings,
                       BrainDouble*              out,
                       BrainWeight               weighted_delta);
/**
 * \fn void delete_neuron(BrainNeuron neuron)
 * \brief free all BrainNeuron memory
 *
 * \param neuron a BrainNeuron
 */
void        delete_neuron              (BrainNeuron       neuron);
/**
 * \fn void dump_neuron(const BrainNeuron neuron, const BrainUint layer_idx, const BrainUint neuron_idx, FILE* file)
 * \brief serialize a neuron to an XML file
 *
 * \param neuron     A BrainNeuron
 * \param layer_idx  the BrainLayer index
 * \param neuron_idx the BrainNeuron index
 * \param file       the file to serialize in
 */
void        dump_neuron                (const BrainNeuron neuron,
                                        const BrainUint   layer_idx,
                                        const BrainUint   neuron_idx,
                                        FILE*             file);
/**
 * \fn void set_neuron_input(BrainNeuron       neuron,
 *                           const BrainUint   number_of_inputs,
 *                           const BrainSignal in,
 *                           const BrainBool   is_an_hidden_unit)
 * \brief set the input signal on a neuron
 *
 * Each time a new input comes to a Neuron then if the neuron stay activated because of the
 * dropout selection then we compute the activation
 *
 * \param neuron            a BrainNeuron
 * \param number_of_inputs  input signal's length
 * \param in                the input signal
 * \param is_an_hidden_unit is this neuron in an hidden layer
 */
void set_neuron_input(BrainNeuron       neuron,
                      const BrainUint   number_of_inputs,
                      const BrainSignal in,
                      const BrainBool   is_an_hidden_unit);
/**
 * \fn void update_neuron_using_backpropagation(BrainNeuron neuron,
 *                                              const BrainDouble loss);
 * \brief update weights using backpropagation
 *
 * We first update the neuron's bias
 * Then we update the layer weighted deltas
 * Finally we update neuron's weight
 *
 * \param neuron a BrainNeuron
 * \param loss  the loss
 */
void update_neuron_using_backpropagation(BrainNeuron neuron,
                                         const BrainDouble loss);
/**
 * \fn void update_neuron_using_resilient(BrainNeuron neuron,
 *                                        const BrainDouble loss);
 * \brief update weights using resilient learning
 *
 * \param neuron a BrainNeuron
 * \param loss  the loss
 */
void update_neuron_using_resilient(BrainNeuron neuron,
                                   const BrainDouble loss);
/**
 * \fn void set_neuron_bias(BrainNeuron neuron, const BrainDouble bias)
 * \brief set the bias of a neuron
 *
 * \param neuron The neuron
 * \param bias   The neuron's bias
 */
void set_neuron_bias(BrainNeuron neuron,
                     const BrainDouble bias);
/**
 * \fn void set_neuron_weight(BrainNeuron neuron, const BrainUint index, const BrainDouble weight)
 * \brief set a weight of a neuron
 *
 * \param neuron The neuron
 * \param index  The neuron's weight index
 * \param weight The weight
 */
void set_neuron_weight(BrainNeuron neuron,
                       const BrainUint index,
                       const BrainDouble weight);
#endif /* BRAIN_NEURON_BUILDER_H */
