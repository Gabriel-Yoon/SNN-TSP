import os
from plotFunctions import plotSynapseWeightsFromJson, spike_raster_plot, plot_neuron_potentials, plotPerformance, plotSpikeFiringRate

# Get the path to the directory containing the JSON files
json_directory = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))

# List all the JSON files in the directory
synapse_json_files = [file for file in os.listdir(json_directory) if file.startswith("weight_gp_gm_") and file.endswith(".json")]
spike_history_file = "spike_history.json"
hidden_potential_file = "HiddenMemV.json"
tsp_data_file = "tsp_data.json"
spike_firing_rate_file = "spike_firing_rate_Vrest.json"

# Iterate through each JSON file
for json_file in synapse_json_files:
    # Extract the time value "t" from the filename
    t = int(json_file.split("_")[3].split(".")[0])  # Assuming the file format is "weight_gp_gm_t.json"

    # Plot the synapse weights for each JSON file
    output_filename = f"weight_gp_gm_{t}.png"
    # REMOVE # BELOW
    # plotSynapseWeightsFromJson(os.path.join(json_directory, json_file), output_filename)

if os.path.exists(spike_history_file):
    spike_raster_plot(spike_history_file)
# plotPerformance(spike_history_file, tsp_data_file)

if os.path.exists(hidden_potential_file):
    plot_neuron_potentials(hidden_potential_file)
# plot_neuron_potentials(hidden_potential_file)

if os.path.exists(spike_firing_rate_file):
    plotSpikeFiringRate(spike_firing_rate_file)

#tsp_data_filename = "/Users/gabriel/Development/SNN-TSP/build/tsp_data.json"
#spike_history_filename = "/Users/gabriel/Development/SNN-TSP/build/spike_history.json"
#plotEventPlotFromJson(tsp_data_filename, spike_history_filename)

# filename = "spike_history.json"
# plot_spike_raster(filename)

