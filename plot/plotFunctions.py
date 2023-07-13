import json
import numpy as np
import seaborn as sns
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

# Function to plot synapse array weights (gp and gm) from JSON file
def plotSynapseWeightsFromJson(filename):
    # Read JSON file
    with open(filename) as file:
        data = json.load(file)

    # Convert JSON data to a 2D numpy array
    synapseData = np.array(data)

    # Separate gp and gm values
    gpData = np.array([[synapse["gp"] for synapse in row] for row in synapseData])
    gmData = np.array([[synapse["gm"] for synapse in row] for row in synapseData])


    # Create a figure and subplots
    fig, axes = plt.subplots(1, 2, figsize=(20, 8))

    # Create heatmaps using seaborn
    heatmap_gp = sns.heatmap(gpData, cmap='viridis', annot=True, fmt=".2f", cbar=True, ax=axes[0],
                             annot_kws={"size": 6}, cbar_kws={"shrink": 0.6})
    heatmap_gm = sns.heatmap(gmData, cmap='viridis', annot=True, fmt=".2f", cbar=True, ax=axes[1],
                             annot_kws={"size": 6}, cbar_kws={"shrink": 0.6})

    # Adjust the colorbar size and fontsize
    cbar1 = axes[0].collections[0].colorbar
    cbar1.ax.tick_params(labelsize=6)
    cbar1.ax.yaxis.set_tick_params(width=0.5)  # Adjust the width of the colorbar ticks

    cbar2 = axes[1].collections[0].colorbar
    cbar2.ax.tick_params(labelsize=6)
    cbar2.ax.yaxis.set_tick_params(width=0.5)  # Adjust the width of the colorbar ticks


    # Set titles and labels
    axes[0].set_title('Synapse Array Gp Values')
    axes[0].set_xlabel('Hidden Neurons', fontsize=8)
    axes[0].set_ylabel('Visible Neurons', fontsize=8)
    axes[0].tick_params(axis='both', which='both', labelsize=6)
    axes[0].set_aspect('equal')
    axes[1].set_title('Synapse Array Gm Values')
    axes[1].set_xlabel('Hidden Neurons', fontsize=8)
    axes[1].set_ylabel('Visible Neurons', fontsize=8)
    axes[1].tick_params(axis='both', which='both', labelsize=6)
    axes[1].set_aspect('equal')

    # Add grid to highlight every 5 synapses
    num_rows, num_cols = gpData.shape
    for i in range(0, num_rows, 5):
        axes[0].axhline(i, color='white', linewidth=0.5)
    for j in range(0, num_cols, 5):
        axes[0].axvline(j, color='white', linewidth=0.5)
    for i in range(0, num_rows, 5):
        axes[1].axhline(i, color='white', linewidth=0.5)
    for j in range(0, num_cols, 5):
        axes[1].axvline(j, color='white', linewidth=0.5)


    # Adjust the layout with left margin
    plt.subplots_adjust(left=0.5, right=0.55, bottom=0.1, top=0.9, wspace=0.25)
    plt.tight_layout()

    # Display the plot
    plt.show()
