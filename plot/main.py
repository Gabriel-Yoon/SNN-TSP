from plotFunctions import plotSynapseWeightsFromJson, plotSpikeFromJson, plotEventPlotFromJson

filename = "/Users/gabriel/Development/SNN-TSP/build/weight_gp_gm_124000.json"
plotSynapseWeightsFromJson(filename, "junk")
#tsp_data_filename = "/Users/gabriel/Development/SNN-TSP/build/tsp_data.json"
#spike_history_filename = "/Users/gabriel/Development/SNN-TSP/build/spike_history.json"
#plotEventPlotFromJson(tsp_data_filename, spike_history_filename)