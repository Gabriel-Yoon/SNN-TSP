#ifndef _SPIKE_H_
#define _SPIKE_H_

#include <utility>
#include <list>
#include <vector>

struct spike_casing {
	double _spikeTime;
	int _side;
	int _neuronNum;
    bool _reset;
    bool _st;

    spike_casing():_reset(false), _st(false) {}

    /*
    spike_casing(const spike &_spike){
        this->_spikeTime = _spike._spikeTime;
        this->_side = _spike._side;
        this->_neuronNum = _spike._neuronNum;
        this->_reset = true;
        this->_st = true;
    }
    */

    bool operator<(const spike_casing r) const {
        return this->_spikeTime > r._spikeTime;
    }

};

class spike
{
    friend class neuron;
    friend class synapse;
    friend class core;

    // typedef std::list<std::pair<int, int>> spikePosition;
    /*---------------------fields-----------------*/
    public: double _spikeTime;
    public: int _coreNum;
    public: std::list<std::pair<int, int>> _spk; // pair<side, neuron_index>.
    public: bool _reset;
	public: bool _st;
    public: bool _wup;
	public: int _iso; // not using

    /*---------------------methods----------------*/
    // spk Class constructor.
    public: spike() : _spikeTime(0),_spk(),_reset(false),_st(false),_wup(false),_iso(0){}
    public: ~spike(){}
    // function from previous code
	public: spike(const spike &orig_spk) {
		_spikeTime = orig_spk._spikeTime;
		for(auto it = orig_spk._spk.begin(); it != orig_spk._spk.end(); it++) {
			if(find(_spk.begin(), _spk.end(), *it) == _spk.end()) {
				_spk.push_back(std::make_pair(it->first, it->second));
			}
		}
		_reset = orig_spk._reset;
		_st = orig_spk._st;
		_iso = orig_spk._iso;
	}
    
    public: void merge(const spike &m_spk) {
		for(auto it = m_spk._spk.begin(); it != m_spk._spk.end(); it++) {
			_spk.push_back(*it);
		}
	}

    // assignment operator
    public: spike& operator = (const spike&);
    // relational operators
    public: bool operator () (const spike&) const;
    public: bool operator == (const spike&) const;
    public: bool operator != (const spike&) const;
    public: bool operator <  (const spike&) const;
    public: bool operator <= (const spike&) const;
    public: bool operator >  (const spike&) const;
    public: bool operator >= (const spike&) const;
    // arithmetic operators
    public: spike& operator += (const spike&);
    public: spike& operator -= (const spike&);
    public: friend const spike operator + (const spike&, const spike&);
    public: friend const spike operator - (const spike&, const spike&);

};
//----------------------------------------------------

// comparator
struct spk_cmp {
    bool operator()(std::pair<double, spike*> a, std::pair<double, spike*> b) {
        if(a.first == b.first) return a.second->_spk.size() < b.second->_spk.size();
        return a.first > b.first;
    }
};

/*
// comparator of time
struct spikePositionTimeCompare {
    bool* _spikeTimeSame;
	bool* _spikeSideSame;
	
	bool operator()(std::pair<double, spike::spikePosition*> a, std::pair<double, spike::spikePosition*> b) {
        
		
		if(a.first == b.first) return a.second->spikePosition.size() < b.second->spikePosition.size();
        return a.first > b.first;
    }
};
*/

// assignment operator
/*
inline spike& spike::operator = (const spike& t)
{
    _spikeTime = t._spikeTime;
    _coreNum = t._coreNum;
    _spk = t._spk;
    _reset = t._reset;
	_st = t._st;
	_iso = t._iso;
}


// relational operators

inline bool spike::operator () (const spike& t) const
{
	return (_spikeTime == t._spikeTime);
}

inline bool spike::operator == (const spike& t) const
{
	return (_spikeTime == t._spikeTime);
}

inline bool spike::operator != (const spike& t) const
{
	return (_spikeTime != t._spikeTime);
}

inline bool spike::operator < (const spike& t) const
{
	return (_spikeTime < t._spikeTime);
}

inline bool spike::operator <= (const spike& t) const
{
	return (_spikeTime <= t._spikeTime);
}

inline bool spike::operator > (const spike& t) const
{
	return (_spikeTime > t._spikeTime);
}

inline bool spike::operator >= (const spike& t) const
{
	return (_spikeTime >= t._spikeTime);
}

// arithmetic operators

inline spike& spike::operator += (const spike& t)
{
    // Need Modification
	return *this;
}

inline spike& spike::operator -= (const spike& t)
{
// Need Modification
    return *this;
}


inline const spike operator + (const spike& t1, const spike& t2)
{// Need Modification
	//return t1 += t2;
}

inline const spike operator -(const spike& t1, const spike& t2)
{// Need Modification
	//return t1 -= t2;
}
*/
#endif /* _SPK_H_ */
