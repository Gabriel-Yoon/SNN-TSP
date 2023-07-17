from plotFunctions import plotSynapseWeightsFromJson, plotSpikeFromJson, plotEventPlotFromJson, plot_spike_raster


filename = "/Users/gabriel/Development/SNN-TSP/build/weight_gp_gm_38000.json"
plotSynapseWeightsFromJson(filename, "38000.png")
#tsp_data_filename = "/Users/gabriel/Development/SNN-TSP/build/tsp_data.json"
#spike_history_filename = "/Users/gabriel/Development/SNN-TSP/build/spike_history.json"
#plotEventPlotFromJson(tsp_data_filename, spike_history_filename)

filename = "spike_history.json"
plot_spike_raster(filename)

