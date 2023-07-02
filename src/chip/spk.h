#ifndef _SPK_H_
#define _SPK_H_

#include <utility>
#include <list>
#include <vector>

enum : int {
    side_v,
	side_h
};

struct spk_one {
	double time;
	int side;
	int neuron;

	bool operator()(const spk_one &a, const spk_one &b) const {
		return a.time > b.time;
	}
};

class spk
{
    friend class neuron;
    friend class synapse;
    friend class core;
    /*---------------------fields-----------------*/
    public: double _spkTime;
    public: int _coreNum;
    // Be aware that list occur errors so I changed it to vector
    public: std::vector<pair<int, int>> _spk; // pair<side, neuron_index>.
    public: bool _reset;
	public: bool _st;
	public: int _iso;
    /*---------------------methods----------------*/
    // spk Class constructor.
    public: spk() : _spkTime(0),_spk(),_reset(false),_st(false),_iso(0){}
    // function from previous code
	public: spk(const spk &orig_spk) {
		_spkTime = orig_spk._spkTime;
		for(auto it = orig_spk._spk.begin(); it != orig_spk._spk.end(); it++) {
			if(find(_spk.begin(), _spk.end(), *it) == _spk.end()) {
				_spk.push_back(make_pair(it->first, it->second));
			}
		}
		_reset = orig_spk._reset;
		_st = orig_spk._st;
		_iso = orig_spk._iso;
	}
    // function from previous code -> plz change this to operator
    void merge(const spk &m_spk) {
		for(auto it = m_spk._spk.begin(); it != m_spk._spk.end(); it++) {
			_spk.push_back(*it);
		}
	}
    // assignment operator
    public: spk& operator = (const spk&);
    // relational operators
    public: bool operator () (const spk&) const;
    public: bool operator == (const spk&) const;
    public: bool operator != (const spk&) const;
    public: bool operator <  (const spk&) const;
    public: bool operator <= (const spk&) const;
    public: bool operator >  (const spk&) const;
    public: bool operator >= (const spk&) const;
    // arithmetic operators
    public: spk& operator += (const spk&);
    public: spk& operator -= (const spk&);
    public: friend const spk operator + (const spk&, const spk&);
    public: friend const spk operator - (const spk&, const spk&);

};
//----------------------------------------------------

// comparator
struct spk_cmp {
    bool operator()(pair<double, spk*> a, pair<double, spk*> b) { // <time, spk>
        if(a.first == b.first) return (*a.second)._spk.size() < (*b.second)._spk.size();
        return a.first > b.first;
    }
};

// assignment operator

inline spk& spk::operator = (const spk& t)
{
    _spkTime = t._spkTime;
    _coreNum = t._coreNum;
    _spk = t._spk;
    _reset = t._reset;
	_st = t._st;
	_iso = t._iso;
}

// relational operators

inline bool spk::operator () (const spk& t) const
{
	return (_spkTime == t._spkTime);
}

inline bool spk::operator == (const spk& t) const
{
	return (_spkTime == t._spkTime);
}

inline bool spk::operator != (const spk& t) const
{
	return (_spkTime != t._spkTime);
}

inline bool spk::operator < (const spk& t) const
{
	return (_spkTime < t._spkTime);
}

inline bool spk::operator <= (const spk& t) const
{
	return (_spkTime <= t._spkTime);
}

inline bool spk::operator > (const spk& t) const
{
	return (_spkTime > t._spkTime);
}

inline bool spk::operator >= (const spk& t) const
{
	return (_spkTime >= t._spkTime);
}

// arithmetic operators

inline spk& spk::operator += (const spk& t)
{
    // Need Modification
	return *this;
}

inline spk& spk::operator -= (const spk& t)
{
// Need Modification
    return *this;
}


inline const spk operator + (const spk& t1, const spk& t2)
{// Need Modification
	//return t1 += t2;
}

inline const spk operator -(const spk& t1, const spk& t2)
{// Need Modification
	//return t1 -= t2;
}

#endif /* _SPK_H_ */
