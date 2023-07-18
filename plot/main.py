from plotFunctions import plotSynapseWeightsFromJson, plotSpikeFromJson, plotEventPlotFromJson, plot_spike_raster


filename = "weight_gp_gm_0.json"
plotSynapseWeightsFromJson(filename, "0.png")
#tsp_data_filename = "/Users/gabriel/Development/SNN-TSP/build/tsp_data.json"
#spike_history_filename = "/Users/gabriel/Development/SNN-TSP/build/spike_history.json"
#plotEventPlotFromJson(tsp_data_filename, spike_history_filename)

# filename = "spike_history.json"
# plot_spike_raster(filename)

