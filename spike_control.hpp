#ifndef SPIKE_CONTROL
#define SPIKE_CONTROL

#include <vector>
#include <string>
#include <list>
#include <queue>
#include <algorithm>

using namespace std;

struct sm_spk_one {
	double time;
	int side;
	int neuron;

	bool operator()(const sm_spk_one &a, const sm_spk_one &b) const {
		return a.time > b.time;
	}
};

struct sm_spk {
	double time;
	list<pair<int, int>> spk; //pair<side, neuron_index>
	bool reset;
	bool st;
	bool iso;

	void merge(const sm_spk &m_spk) {
		for(auto it = m_spk.spk.begin(); it != m_spk.spk.end(); it++) {
			spk.push_back(*it);
		}
	}

	sm_spk():reset(false),st(false),iso(false) {}

	sm_spk(const sm_spk &orig_spk) {
		time = orig_spk.time;
		for(auto it = orig_spk.spk.begin(); it != orig_spk.spk.end(); it++) {
			if(find(spk.begin(), spk.end(), *it) == spk.end()) {
				spk.push_back(make_pair(it->first, it->second));
			}
		}
		reset = orig_spk.reset;
		st = orig_spk.st;
		iso = orig_spk.iso;
	}

	bool operator()(const sm_spk &a, const sm_spk &b) const {
		return a.time > b.time;
	}
};

struct spk_cmp {
    bool operator()(pair<double, sm_spk*> a, pair<double, sm_spk*> b) {
        if(a.first == b.first) return a.second->spk.size() < b.second->spk.size();
        return a.first > b.first;
    }
};

#endif