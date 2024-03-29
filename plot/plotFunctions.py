import os
import sys
import json
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

def plot_data(json_files, output_folder):
    print("ON THE WAY??")
    # Process each JSON file in the JSON_FILES directory
    for json_file in json_files:
        # Get the base name of the JSON file (without the directory path)
        base_name = os.path.basename(json_file)
        base_name = os.path.splitext(base_name)[0]
        output_file = os.path.join(output_folder, f"{base_name}.png")
        if base_name == "weight_gp_gm" :
            print(output_file)
            plotSynapseWeightsFromJson(json_file, output_file)
        if base_name == "spike_history":
            plotEventPlotFromJson(json_file, output_file)
        
    #    plotSynapseWeightsFromJson(data)    # weight_gp_gm.json
    #    plotEventPlotFromJson(data)         # spike_history.json

def plotEventPlotFromJson(filename, spike_history_filename):
    # Read the data from tsp_data.json to get the number of neurons
    with open(filename, 'r') as file:
        data = json.load(file)

    num_city = data.get("num_city", 0)  # Get the number of neurons (if "num_city" field exists)
    total_neurons = num_city * num_city  # Total number of y-axis indexes

    # Read the spike history from spike_history.json
    with open(spike_history_filename, 'r') as spike_file:
        spike_data = json.load(spike_file)

    # Prepare the data for plotting
    neuron_indices = []
    spike_times = []
    for entry in spike_data:
        neuron_indices.append(entry["neuron"])
        spike_times.append(entry["time"])

    # Sort spike_times based on neuron_indices
    sorted_indices, sorted_times = zip(*sorted(zip(neuron_indices, spike_times)))

    # Create the event plot
    fig, ax = plt.subplots()
    ax.eventplot(sorted_times, lineoffsets=sorted_indices, colors='b')

    # Set y-axis limits and ticks
    ax.set_ylim(-0.5, total_neurons - 0.5)
    ax.set_yticks(np.arange(0, total_neurons, num_city))
    ax.set_yticklabels(np.arange(num_city))  # Assuming neurons are indexed from 0 to num_city - 1

    # Add gridlines
    for i in range(num_city, total_neurons, num_city):
        ax.axhline(i - 0.5, color='gray', linestyle='--')

    ax.set_xlabel('Time')
    ax.set_ylabel('Neuron Index')
    ax.set_title('Event Plot')

    plt.show()

def plotSpikeFromJson(filename, output_file):
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
        fig = plt.eventplot(spike_time, lineoffsets=event_height * index, linelengths=0.5)

    # Customize the plot
    plt.xlabel('Time')
    plt.ylabel('Neuron Index')
    plt.title('Spike Timing for Neuron Indices')
    plt.yticks(list(spike_times.keys()), list(spike_times.keys()))

    # Get the path of the current script
    script_path = os.path.abspath(__file__)
    script_directory = os.path.dirname(script_path)

    # Save the plot to a file in the same folder as the script
    output_file = os.path.join(script_directory, 'spikes.png')
    plt.savefig(output_file)

    # Close the figure to free up memory
    plt.close(fig)

def plot_spike_raster(spike_data):
    num_of_neurons = spike_data[0]["num_of_neurons"]
    spikes = [(entry["neuron"], entry["time"]) for entry in spike_data[1:]]

    neuron_ids = [entry[0] for entry in spikes]
    spike_times = [entry[1] for entry in spikes]

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.eventplot(spike_times, lineoffsets=neuron_ids, linelengths=0.7, colors="black")
    ax.set_xlim(min(spike_times) - 0.001, max(spike_times) + 0.001)
    ax.set_ylim(-1, num_of_neurons)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Neuron Number")
    ax.set_title("Spike Raster Plot")
    plt.show()

def spike_raster_plot(json_file):
    with open(json_file, 'r') as f:
        data = json.load(f)

    num_of_neurons = data[0]['num_of_neurons']
    spikes = [(entry['time'], entry['neuron']) for entry in data[1:]]

    spike_times = [[] for _ in range(num_of_neurons)]
    for time, neuron in spikes:
        spike_times[neuron].append(time)

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.eventplot(spike_times, linewidths=1)
    ax.set_title('Spike Raster Plot')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Neuron Index')
    ax.set_yticks(range(num_of_neurons))
    ax.set_ylim(-1, num_of_neurons)
    ax.invert_yaxis()
    # plt.show()
    output_filename = f"SpikeRasterPlot.png"
    plt.savefig(output_filename)

def plot_neuron_potentials(json_file):
    with open(json_file, 'r') as file:
        data = json.load(file)

    neuron_data = data['neuronData']
    num_neurons = len(neuron_data[0]['neuronPotentials'])
    
    fig, axs = plt.subplots(num_neurons, 1, figsize=(8, 3*num_neurons), sharex=True)

    for i, ax in enumerate(axs):
        time_steps = [entry['time'] for entry in neuron_data]
        neuron_potentials = [entry['neuronPotentials'][i] for entry in neuron_data]
        ax.plot(time_steps, neuron_potentials, linewidth=1)

        ax.axhline(y=0, color='grey', linewidth=1, linestyle='--')  # Add grey line at y=0
        ax.set_ylim(-1.02, 1.02) # Set y-axis limits from -1 to 1
        ax.set_yticks([-1, 0, 1])  # Add 0 to y-axis ticks
        ax.legend([f'Neuron {i+1}'])
        ax.grid(False)  # Remove the grid
        ax.axhline(y=1, color='black', linewidth=1, linestyle='--')  # Black line at y=1

    axs[-1].set_xlabel('Time')
    fig.text(0.04, 0.5, 'Neuron Potential', va='center', rotation='vertical', fontsize=12)
    plt.suptitle('Neuron Potentials vs. Time', fontsize=16)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    # plt.show()
    output_filename = f"NeuronPotentialPlot.png"
    plt.savefig(output_filename)

def plotNeuronPotentialsFromJson(filename, output_file):
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
    fig = plt.figure()
    for i, potentials in enumerate(neuronPotentials):
        plt.plot(times[i], potentials, label=f'Neuron {i+1}')

    plt.xlabel('Time')
    plt.ylabel('Neuron Potential')
    plt.title('Neuron Potential vs. Time')
    plt.legend()
    plt.grid(True)
    
    # Save the plot to a file
    plt.savefig(output_file)

    # Close the figure to free up memory
    plt.close(fig)

# Function to plot synapse array weights (gp and gm) from JSON file
def plotSynapseWeightsFromJson(filename, output_file):
    # Read JSON file
    with open(filename, 'r') as file:
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

    plt.savefig(output_file)
    #plt.show()

    # Close the figure to free up memory
    plt.close(fig)

    print("weight figure named : " + output_file +" saved successfully")

def plotPerformance(json_file, tsp_data_file):
    with open(json_file, 'r') as f:
        json_data = json.load(f)

    with open(tsp_data_file, 'r') as f:
        tsp_data = json.load(f)

    distance_matrix = tsp_data.get("distance_matrix")
    if distance_matrix is None:
        raise ValueError("distance_matrix not found in the JSON file")

    # Extract the number of groups and neurons per group
    num_neurons = json_data[0]["num_of_neurons"]
    num_groups = int(num_neurons ** 0.5)
    neurons_per_group = num_neurons // num_groups

    # Initialize a list to store the latest spike time for each group
    latest_spike_times = [0] * num_groups

    # Initialize a list to store the latest spike neuron index for each group
    latest_spike_neurons = [-1] * num_groups

    # Initialize the itinerary route
    route = []

    # Initialize the total distance
    total_distance = 0

    # Iterate through the JSON data to find the latest spike time for each group
    # for entry in json_data[1:]:
    #     neuron = entry["neuron"]
    #     time = entry["time"]
    #     group = neuron // num_groups
    #     latest_spike_times[group] = max(latest_spike_times[group], time)

    # Extract the spike data and update latest spike information
    for entry in json_data[1:]:
        neuron = entry["neuron"]
        time = entry["time"]
        group = neuron // neurons_per_group
        if time > latest_spike_times[group]:
            latest_spike_times[group] = time
            latest_spike_neurons[group] = neuron

        # Add the latest spike neuron index to the itinerary
        route.append(latest_spike_neurons[group] // neurons_per_group + 1)
    
    for i in range(len(route) - 1):
        city1 = route[i] - 1
        city2 = route[i + 1] - 1
        total_distance += distance_matrix[city1][city2]

    print("route", route)

    # spikes = [(entry['time'], entry['neuron']) for entry in data[1:]]

    # spike_times = [[] for _ in range(num_of_neurons)]
    # WTA_spike_times = [[] for _ in range(sqrt(num_of_neurons))]

    # for time, neuron in spikes:
    #     spike_times[neuron].append(time)

    # plt.show()
    # output_filename = f"Performance.png"
    # plt.savefig(output_filename)

def plotSpikeFiringRate(json_filename):
    with open(json_filename, 'r') as json_file:
        data = json.load(json_file)

    Vrest = [entry['Vrest'] for entry in data[1:]]  # Exclude the first entry
    spikes = [entry['spikes'] for entry in data[1:]]  # Exclude the first entry

    sns.set(style="whitegrid")
    plt.figure(figsize=(10, 6))  # Adjust the figure size as needed
    sns.lineplot(x=Vrest, y=spikes)
    plt.xlabel('Resting Voltage(V)')
    plt.ylabel('Firing Rate(Hz)')
    plt.title('LIF Neuron Firing Rate')
    plt.savefig("LIN Neuron Firing Rate")

#//**************************************************************************************************************//
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python plot_script.py <json_files_directory> <output_folder>")
        sys.exit(1)

    json_files_directory = sys.argv[1]
    output_folder = sys.argv[2]

    # Find all JSON files in the specified directory
    json_files = [os.path.join(json_files_directory, file) for file in os.listdir(json_files_directory) if file.endswith('.json')]
    print("PYTHON ON THE WAY!!")
    print(output_folder)

    plot_data(json_files, output_folder)