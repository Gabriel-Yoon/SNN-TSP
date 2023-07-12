import json
import numpy as np
import matplotlib.pyplot as plt

def plotSpikeFromJson(filename):
    # Open the JSON file
    with open('/Users/gabriel/Development/SNN-TSP/src/build/spike_history.json', 'r') as file:
        # Load the JSON data
        data = json.load(file)

    # Access the time and spike neuron index arrays from the JSON data
    time_array = data['time']
    print(time_array)
    neuron_index_array = data['neuron']
    print(neuron_index_array)
    # Find the unique neuron indices
    unique_indices = np.unique(neuron_index_array)

    # Create an empty dictionary to store the spike times for each neuron index
    spike_times = {index: [] for index in unique_indices}

    # Iterate over the time and neuron index arrays
    for time, neuron_index in zip(time_array, neuron_index_array):
        spike_times[neuron_index].append(time)

    # Plot the spike timing for each neuron index using eventplot
    event_height = 0.8  # Vertical position of the event markers
    for index, spike_time in spike_times.items():
        plt.eventplot(spike_time, lineoffsets=event_height * index, linelengths=0.5)

    # Customize the plot
    plt.xlabel('Time')
    plt.ylabel('Neuron Index')
    plt.title('Spike Timing for Neuron Indices')
    plt.yticks(list(spike_times.keys()), list(spike_times.keys()))

    # Display the plot
    plt.show()


def plotNeuronPotentialsFromJson(filename):
    # Load the JSON data from the file
    with open(filename, 'r') as file:
        data = json.load(file)

    # Extract the neuron potential data
    neuronData = data['neuronData']
    times = []
    neuronPotentials = []

    for entry in neuronData:
        times.append(entry['time'])
        neuronPotentials.append(entry['neuronPotentials'])

    # Plot the neuron potential data
    plt.figure()
    for i, potentials in enumerate(neuronPotentials):
        plt.plot(times[i], potentials, label=f'Neuron {i+1}')

    plt.xlabel('Time')
    plt.ylabel('Neuron Potential')
    plt.title('Neuron Potential vs. Time')
    plt.legend()
    plt.grid(True)
    plt.show()

# Usage
filename = 'data.json'
plotNeuronPotentialsFromJson(filename)