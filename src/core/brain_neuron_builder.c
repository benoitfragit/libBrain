#include "brain_neuron_builder.h"

struct Neuron
{
    BrainDouble*        _in;
    BrainDouble*        _w;
    BrainDouble         _out;
    BrainDouble         _learning_rate;
    BrainDouble         _inertial_factor;
    BrainDouble         _delta;
    BrainDouble*        _correction;
    PtrFunc             _activation;
    PtrFunc             _derivative;
    BrainInt            _number_of_input;
    BrainActivationType _activation_type;
} Neuron;

BrainDouble
get_neuron_output(BrainNeuron neuron)
{
    if (neuron)
    {
        return neuron->_out;
    }

    return 0.0;
}

BrainDouble
get_neuron_weighted_delta(BrainNeuron neuron, const BrainInt index)
{
    if (neuron && 0 <= index && index < neuron->_number_of_input)
    {
        return neuron->_delta * neuron->_w[index];
    }

    return 0.0;
}

void
append_neuron_delta(BrainNeuron neuron, const BrainDouble delta)
{
    if (neuron != NULL)
    {
        neuron->_delta += neuron->_derivative(neuron->_out) * delta;
    }
}

void
propagate_neuron(BrainNeuron neuron, const BrainDouble out, const BrainInt input_index)
{
    if (neuron && 0 <= input_index && input_index < neuron->_number_of_input)
    {
        neuron->_in[input_index] = out;
    }
}

void
set_neuron_input(BrainNeuron neuron,
                 const BrainInt number_of_inputs,
                 const double* in)
{
    BrainInt k = 0;

    if (neuron != NULL && neuron->_number_of_input == number_of_inputs)
    {
        for (k = 0; k < neuron->_number_of_input; ++k)
        {
            neuron->_in[k] = in[k];
        }

        activate_neuron(neuron);
    }
}

BrainDouble
get_neuron_weight(BrainNeuron neuron, const BrainInt weight_index)
{
    if (neuron != NULL
    && 0 <= weight_index
    && weight_index < neuron->_number_of_input)
    {
        return neuron->_w[weight_index];
    }

    return 0.0;
}

BrainDouble
get_neuron_input(BrainNeuron neuron, const BrainInt input_index)
{
    if (neuron != NULL
    && 0 <= input_index
    && input_index < neuron->_number_of_input)
    {
        return neuron->_in[input_index];
    }

    return 0.0;
}

void
activate_neuron(BrainNeuron neuron)
{
    BrainInt j;
    if (neuron != NULL)
    {
        BrainDouble sum = 0.0;

        for (j = 0; j < neuron->_number_of_input + 1; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        neuron->_out = neuron->_activation(sum);
        neuron->_out = (neuron->_out > 0) - (neuron->_out < 0);
        neuron->_delta = 0.0;
    }
}

void
update_neuron(BrainNeuron neuron)
{
    BrainInt index = 0;

    if (neuron != NULL)
    {
        for (index = 0; index < neuron->_number_of_input+1; ++index)
        {
            BrainDouble correction = - neuron->_learning_rate * neuron->_delta * neuron->_in[index] + neuron->_inertial_factor * neuron->_correction[index];

            neuron->_w[index] += correction;
            neuron->_correction[index] = correction;
        }
    }
}

void
delete_neuron(BrainNeuron neuron)
{
    if (neuron)
    {
        if (neuron->_in != NULL)
        {
            free(neuron->_in);
        }

        if (neuron->_w != NULL)
        {
            free(neuron->_w);
        }

        if (neuron->_correction != NULL)
        {
            free(neuron->_correction);
        }

        free(neuron);

    }
}


BrainNeuron
new_neuron_from_context(Context context)
{
    BrainInt    index = 0;
    BrainDouble random_value_limit = 0.0;
    BrainChar*  buffer = NULL;
    BrainNeuron _neuron = NULL;

    if (!context || !is_node_with_name(context, "neuron"))
    {
        return NULL;
    }

    _neuron                   = (BrainNeuron)malloc(sizeof(Neuron));
    _neuron->_learning_rate   = node_get_double(context, "learning-rate", 0.0);
    _neuron->_inertial_factor = node_get_double(context, "inertial-factor", 0.0);
    _neuron->_out             = 0.0;
    _neuron->_delta           = 0.0;
    _neuron->_number_of_input = node_get_int(context, "input", 0);
    _neuron->_in              = (BrainDouble *)malloc((_neuron->_number_of_input + 1) * sizeof(BrainDouble));
    _neuron->_w               = (BrainDouble *)malloc((_neuron->_number_of_input + 1) * sizeof(BrainDouble));
    _neuron->_correction      = (BrainDouble *)malloc((_neuron->_number_of_input + 1) * sizeof(BrainDouble)) ;

    buffer = (BrainChar *)node_get_prop(context, "activation-type");

    _neuron->_activation_type = get_activation_type(buffer);
    _neuron->_activation      = activation(_neuron->_activation_type);
    _neuron->_derivative      = derivative(_neuron->_activation_type);

    memset(_neuron->_correction, 0, (_neuron->_number_of_input + 1) * sizeof(BrainDouble));

    _neuron->_in[_neuron->_number_of_input] = -1.0;

    random_value_limit = 1.0/(BrainDouble)(_neuron->_number_of_input);

    for (index = 0; index < _neuron->_number_of_input + 1; ++index)
    {
        _neuron->_w[index] = (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;
    }

    free(buffer);

    return _neuron;
}

void
dump_neuron(BrainNeuron neuron,
            const BrainInt layer_idx,
            const BrainInt neuron_idx,
            FILE* file)
{
    BrainInt i;
    if (neuron && file)
    {
        for (i = 0; i < neuron->_number_of_input + 1; ++i)
        {
            fprintf(file, "\t<weight value=\"%lf\"", neuron->_w[i]);
            fprintf(file, " layer=\"%d\"", layer_idx);
            fprintf(file, " neuron=\"%d\"", neuron_idx);
            fprintf(file, " input=\"%d\"/>\n",i);
        }
    }
}

BrainInt
get_neuron_number_of_inputs(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input + 1;
    }

    return 0;
}

void
set_neuron_weight(BrainNeuron neuron,
                  const BrainInt index,
                  const BrainDouble weight)
{
    if (neuron != NULL && 0 <= index && index < neuron->_number_of_input + 1)
    {
        neuron->_w[index] = weight;
    }
}