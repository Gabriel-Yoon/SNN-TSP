
#ifndef EXPORT
#define EXPORT

#include <iostream>
#include <list>
#include <string>
#include <fstream>  //ofstream
#include <stdio.h>

using namespace std;

class nd_export(){
    private:
    
    public:
    sm_export(){
    }

	// Export method
	void export_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tnow, double tend);
	void export_num_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend);
	void export_wta_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend);
	void export_potential_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend);
	void export_travel_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend);
	void export_performance_info_to_csv(ofstream& exportFile, double tnow);

};

#endif //INC_SM_EXPORT