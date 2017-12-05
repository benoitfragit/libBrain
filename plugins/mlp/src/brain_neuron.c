#include "brain_neuron.h"
#include "brain_activation.h"
#include "brain_random.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define EPSILON 1e-6

/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
struct Neuron
{
    /******************************************************************/
    /**                      FUNCTIONAL PARAMETERS                   **/
    /******************************************************************/
    ActivationPtrFunc _activation_function;   /*!< Activation function                                  */
    ActivationPtrFunc _derivative_function;   /*!< Derivative function                                  */
    LearningPtrFunc   _learning_function;     /*!< Learning function                                    */
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainSignal       _in;                    /*!< Input signal of an BrainNeuron                       */
    BrainSignal       _w;                     /*!< An array of weight without the bias                  */
    BrainSignal       _gradients;             /*!< weights gradient values                              */
    BrainSignal       _deltas;                /*!< weights delta values                                 */
    BrainSignal       _errors;                /*!< error to correct in the layer                        */
    BrainSignal       _out;                   /*!< An output value pointer owned by the BrainLayer      */
    BrainReal         _bias;                  /*!< Bias of the neuron                                   */
    BrainReal         _bias_delta;            /*!< Bias delta                                           */
    BrainReal         _bias_gradient;         /*!< Bias gradient                                        */
    BrainReal         _sum;                   /*!< Summation of all input time weight                   */
    /******************************************************************/
    /**                        TRAINING PARAMETERS                   **/
    /******************************************************************/
    BrainReal         _rprop_eta_plus;        /*!< Rprop eta for positive gradient transition           */
    BrainReal         _rprop_eta_minus;       /*!< Rprop eta for negative gradient transition           */
    BrainReal         _rprop_delta_min;       /*!< Rprop delta min                                      */
    BrainReal         _rprop_delta_max;       /*!< Rprop delta max                                      */
    BrainReal         _backprop_learning_rate;/*!< BackProp learning rate                               */
    BrainReal         _backprop_momemtum;     /*!< BackProp momentum value                              */
    BrainUint         _number_of_input;       /*!< Number of inputs                                     */
} Neuron;

static void
update_neuron_using_backpropagation(BrainNeuron neuron, const BrainReal loss)
{
    BRAIN_INPUT(update_neuron_using_backpropagation)

    if (neuron != NULL)
    {
        const BrainUint number_of_inputs      = neuron->_number_of_input;
        const BrainReal learning_rate         = neuron->_backprop_learning_rate;
        const BrainReal momentum              = neuron->_backprop_momemtum;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainReal neuron_gradient   = loss * derivative_function(neuron->_sum);
            BrainUint i = 0;
            /******************************************************/
            /**               BACKPROPAGATE $_i                  **/
            /******************************************************/
            neuron->_bias -= learning_rate * neuron_gradient - momentum * neuron->_bias;

            for (i = 0; i < number_of_inputs; ++i)
            {
                if (neuron->_errors)
                {
                    neuron->_errors[i] += neuron_gradient * neuron->_w[i];
                }

                neuron->_w[i] -= (learning_rate * neuron_gradient * neuron->_in[i] - momentum * neuron->_w[i]);
            }
        }
    }

    BRAIN_OUTPUT(update_neuron_using_backpropagation)
}

static void
update_neuron_using_resilient(BrainNeuron neuron, const BrainReal loss)
{
    BRAIN_INPUT(update_neuron_using_resilient)

    if (neuron != NULL)
    {
        const BrainReal rprop_eta_positive  = neuron->_rprop_eta_plus;
        const BrainReal rprop_eta_negative  = neuron->_rprop_eta_minus;
        const BrainReal rprop_delta_min     = neuron->_rprop_delta_min;
        const BrainReal rprop_delta_max     = neuron->_rprop_delta_max;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainReal neuron_gradient    = loss * derivative_function(neuron->_sum);
            const BrainUint   number_of_inputs = neuron->_number_of_input;
            BrainUint i                        = 0;
            BrainReal new_delta                = 0.;
            BrainReal new_correction           = 0.;
            BrainReal new_gradient             = 0.;
            BrainReal g                        = neuron_gradient;

            //first update the bias using RProp algorithm
            if (0.0 < neuron->_bias_gradient * g)
            {
                new_delta      = MIN(neuron->_bias_delta * rprop_eta_positive, rprop_delta_max);
                new_correction = new_delta;
                new_gradient   = g;

                if (0.0 < g)
                {
                    new_correction *= -1.;
                }
            }
            else if (neuron->_bias_gradient * g < 0.0)
            {
                new_delta       = MAX(neuron->_bias_delta * rprop_eta_negative, rprop_delta_min);
            }
            else if (fabs(neuron->_bias_gradient * g) <= EPSILON)
            {
                new_correction  = neuron->_bias_delta;
                new_gradient    = g;

                if (0. < g)
                {
                    new_correction *= -1.;
                }
            }

            neuron->_bias         += new_correction;
            neuron->_bias_delta    = new_delta;
            neuron->_bias_gradient = new_gradient;

            //then update all other weights using same technique
            for (i = 0; i < number_of_inputs; ++i)
            {
                g               = neuron_gradient * neuron->_in[i];
                new_delta       = 0.;
                new_correction  = 0.;
                new_gradient    = 0.;

                // Update error in the previous error
                if (neuron->_errors != NULL)
                {
                    neuron->_errors[i] += neuron_gradient * neuron->_w[i];
                }

                if (0.0 < neuron->_gradients[i] * g)
                {
                    new_delta      = MIN(neuron->_deltas[i] * rprop_eta_positive, rprop_delta_max);
                    new_correction = new_delta;
                    new_gradient   = g;

                    if (0.0 < g)
                    {
                        new_correction *= -1.;
                    }
                }
                else if (neuron->_gradients[i] * g < 0.0)
                {
                    new_delta       = MAX(neuron->_deltas[i] * rprop_eta_negative, rprop_delta_min);
                }
                else if (fabs(neuron->_gradients[i] * g) <= EPSILON)
                {
                    new_correction  = neuron->_deltas[i];
                    new_gradient    = g;

                    if (0.0 < g)
                    {
                        new_correction *= -1.;
                    }
                }

                neuron->_w[i]        += new_correction;
                neuron->_deltas[i]    = new_delta;
                neuron->_gradients[i] = new_gradient;
            }
        }
    }

    BRAIN_OUTPUT(update_neuron_using_resilient)
}

static LearningPtrFunc
get_learning_function(const BrainLearningType learning_type)
{
    switch (learning_type)
    {
        case BackPropagation: return &update_neuron_using_backpropagation;
        case Resilient: return &update_neuron_using_resilient;
        default:
            break;
    }

    return &update_neuron_using_backpropagation;
}

void
configure_neuron_with_context(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(configure_neuron_with_context)

    BrainActivationType activation_type = Sigmoid;

    if (neuron && context)
    {
        BrainChar* buffer = NULL;
        Context training_context = get_node_with_name_and_index(context, "training", 0);

        if (training_context != NULL)
        {
            Context backprop_context = get_node_with_name_and_index(training_context, "backprop", 0);

            if (backprop_context)
            {
                neuron->_learning_function      = get_learning_function(BackPropagation);
                neuron->_backprop_learning_rate = (BrainReal)node_get_double(backprop_context, "learning-rate", 1.2);
                neuron->_backprop_momemtum      = (BrainReal)node_get_double(backprop_context, "momentum", 0.0);
            }
            else
            {
                Context rprop_context = get_node_with_name_and_index(training_context, "rprop", 0);

                if (rprop_context != NULL)
                {
                    Context eta_context   = get_node_with_name_and_index(rprop_context, "resilient-eta", 0);
                    Context delta_context = get_node_with_name_and_index(rprop_context, "resilient-delta", 0);

                    neuron->_learning_function = get_learning_function(Resilient);

                    if (eta_context != NULL)
                    {
                        neuron->_rprop_eta_plus  = (BrainReal)node_get_double(eta_context, "positive", 1.25);
                        neuron->_rprop_eta_minus = (BrainReal)node_get_double(eta_context, "negative", 0.95);
                    }

                    if (delta_context != NULL)
                    {
                        neuron->_rprop_delta_max = (BrainReal)node_get_double(delta_context, "max", 50.0);
                        neuron->_rprop_delta_min = (BrainReal)node_get_double(delta_context, "min", 0.000001);
                    }
                }
            }
        }

        buffer = (BrainChar *)node_get_prop(context, "activation-function");
        activation_type = get_activation_type(buffer);
        neuron->_activation_function = activation(activation_type);
        neuron->_derivative_function = derivative(activation_type);

        if (buffer != NULL)
        {
            free(buffer);
        }
    }

    BRAIN_OUTPUT(configure_neuron_with_context)
}

void
activate_neuron(BrainNeuron neuron, const BrainBool is_activated)
{
    BRAIN_INPUT(activate_neuron)

    if (neuron != NULL)
    {
        ActivationPtrFunc activation_function = neuron->_activation_function;

        *(neuron->_out) = 0.;
        neuron->_sum    = 0.;

        /**************************************************************/
        /**                 COMPUTE A(<in, W>)                       **/
        /**************************************************************/
        if ((activation_function != NULL)
        &&  is_activated
        &&  neuron->_in
        &&  neuron->_w)
        {
            BrainUint j = 0;

            for (j = 0; j < neuron->_number_of_input; ++j)
            {
                neuron->_sum += neuron->_in[j] * neuron->_w[j];
            }

            neuron->_sum += neuron->_bias;
            *(neuron->_out) = activation_function(neuron->_sum);
        }
    }

    BRAIN_OUTPUT(activate_neuron)
}

void
delete_neuron(BrainNeuron neuron)
{
    BRAIN_INPUT(delete_neuron)

    if (neuron != NULL)
    {
        if (neuron->_w != NULL)
        {
            free(neuron->_w);
        }

        if (neuron->_gradients != NULL)
        {
            free(neuron->_gradients);
        }

        if (neuron->_deltas != NULL)
        {
            free(neuron->_deltas);
        }

        free(neuron);
    }

    BRAIN_OUTPUT(delete_neuron)
}

BrainNeuron
new_neuron(BrainSignal     in,
           const BrainUint number_of_inputs,
           BrainSignal     out,
           BrainSignal     errors)
{
    BRAIN_INPUT(new_neuron)
    BrainNeuron _neuron = NULL;

    if ((out              != NULL)
    &&  (number_of_inputs != 0))
    {
        BrainUint                index   = 0;
        BrainReal random_value_limit     = (BrainReal)number_of_inputs;

        _neuron                          = (BrainNeuron)calloc(1, sizeof(Neuron));

        _neuron->_out                    = out;
        _neuron->_number_of_input        = number_of_inputs;
        _neuron->_in                     = in;
        _neuron->_bias                   = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
        _neuron->_bias_gradient          = 0.;
        _neuron->_bias_delta             = 0.;
        _neuron->_sum                    = 0.;
        _neuron->_backprop_learning_rate = 1.12;
        _neuron->_backprop_momemtum      = 0.0;
        _neuron->_rprop_eta_plus         = 1.2;
        _neuron->_rprop_eta_minus        = 0.95;
        _neuron->_rprop_delta_min        = 0.000001;
        _neuron->_rprop_delta_max        = 50.;
        _neuron->_activation_function    = activation(Sigmoid);
        _neuron->_derivative_function    = derivative(Sigmoid);
        _neuron->_learning_function      = get_learning_function(BackPropagation);
        _neuron->_errors                 = errors;
        _neuron->_w                      = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainReal));
        _neuron->_deltas                 = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainReal));
        _neuron->_gradients              = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainReal));

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
        }
    }

    BRAIN_OUTPUT(new_neuron)

    return _neuron;
}

BrainUint
get_neuron_number_of_input(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input;
    }

    return 0;
}

BrainReal
get_neuron_bias(const BrainNeuron neuron)
{
    BrainReal ret = 0.;

    if (neuron != NULL)
    {
        ret = neuron->_bias;
    }

    return ret;
}

BrainReal
get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
{
    BrainReal ret = 0.0;

    if ((neuron != NULL) &&
        (index < neuron->_number_of_input))
    {
        ret = neuron->_w[index];
    }

    return ret;
}

void
neuron_learning(BrainNeuron neuron, const BrainReal loss)
{
    if (neuron)
    {
        LearningPtrFunc learning_function = neuron->_learning_function;

        if (learning_function)
        {
            learning_function(neuron, loss);
        }
    }
}

void
deserialize_neuron(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(deserialize_neuron)

    if (neuron && context)
    {
        const BrainReal neuron_bias       = (BrainReal)node_get_double(context, "bias", 0.0);
        const BrainUint number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;

        neuron->_bias = neuron_bias;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            neuron->_w[index] = (BrainReal)node_get_content_as_double(subcontext);
        }
    }

    BRAIN_OUTPUT(deserialize_neuron)
}

void
serialize_neuron(BrainNeuron neuron, Writer writer)
{
    BRAIN_INPUT(serialize_neuron)

    if (neuron && writer)
    {
        const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

        if (start_element(writer, "neuron"))
        {
            BrainUint index_input = 0;
            BrainChar buffer[50];

            sprintf(buffer, "%lf", neuron->_bias);
            add_attribute(writer, "bias", buffer);

            if (neuron->_w)
            {
                for (index_input = 0;
                     index_input < number_of_inputs;
                     ++index_input)
                {
                    sprintf(buffer, "%lf", neuron->_w[index_input]);
                    write_element(writer, "weight", buffer);
                }
            }

            stop_element(writer);
        }
    }

    BRAIN_OUTPUT(serialize_neuron)
}
