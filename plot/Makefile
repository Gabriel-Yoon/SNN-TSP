OUTPUT_FOLDER := output_$(shell date +'%Y-%m-%d-%H-%M-%S')

.PHONY: all run_all plots

all: plots

run_all:
	@$(MAKE) -sC $(CMAKE_BINARY_DIR) run_all

plots: run_all
	@echo "Generating plots..."
	@cd $(CMAKE_BINARY_DIR)/$(OUTPUT_FOLDER)/plot && python3 plot_script.py

clean:
	@$(MAKE) -sC $(CMAKE_BINARY_DIR) clean
	@rm -rf $(CMAKE_BINARY_DIR)/$(OUTPUT_FOLDER)